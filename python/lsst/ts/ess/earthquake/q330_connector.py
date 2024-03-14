# This file is part of ts_ess_earthquake.
#
# Developed for the Vera C. Rubin Observatory Telescope and Site Systems.
# This product includes software developed by the LSST Project
# (https://www.lsst.org).
# See the COPYRIGHT file at the top-level directory of this distribution
# for details of code ownership.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

__all__ = ["Q330Connector"]

import asyncio
import ctypes
import logging
import pathlib
import platform
import queue
import threading
import types
from collections.abc import Callable

from lsst.ts import salobj, utils

from .q330_utils import (
    ChannelDataHolder,
    TInit,
    TLibState,
    TMsg,
    TOnesec,
    TState,
    TStateType,
)

EXTENSIONS = {"Linux": "so", "Darwin": "dylib"}

# Unix time seconds at January 1st, 2000, 0h 0m 0.0s UTC. This is needed to
# convert the Q330 timestamps to Unix time.
JAN_FIRST_2000 = 946684800.0

# Wait time [s] for the telemetry task.
TELEMETRY_WAIT = 1.0

# Wait time [s] for the telemetry thread to be stopped.
THREAD_STOP_WAIT = 5.0

# Dict with telemetry topic names.
TOPIC_NAME_DICT = {
    "BH": "earthquakeBroadBandHighGain",
    "BL": "earthquakeBroadBandLowGain",
    "HH": "earthquakeHighBroadBandHighGain",
    "HL": "earthquakeHighBroadBandLowGain",
    "LH": "earthquakeLongPeriodHighGain",
    "LL": "earthquakeLongPeriodLowGain",
    "UH": "earthquakeUltraLongPeriodHighGain",
    "VH": "earthquakeVeryLongPeriodHighGain",
}


class Q330Connector:
    """Class to connect to a Q330 earthquake sensor.

    Parameters
    ----------
    config : `types.SimpleNamespace`
        The configuration, after validation by the schema returned
        by `get_config_schema` and conversion to a types.SimpleNamespace.
    topics : `salobj.Controller`
        The telemetry topics this sensor can write, as a struct with
        attributes.
    log : `logging.Logger`
        Logger.

    Attributes
    ----------
    libq330 : `ctypes.CDLL`
        The C library needed to connect to the Q330 sensor.
    aminiseed_callback : `ctypes.CFUNCTYPE`[`ctypes.c_void_p`]
        The callback for aminiseed events.
    message_callback : `ctypes.CFUNCTYPE`[`ctypes.c_void_p`]
        The callback for message events.
    miniseed_callback : `ctypes.CFUNCTYPE`[`ctypes.c_void_p`]
        The callback for miniseed events.
    secdata_callback : `ctypes.CFUNCTYPE`[`ctypes.c_void_p`]
        The callback for secdata events.
    state_callback : `ctypes.CFUNCTYPE`[`ctypes.c_void_p`]
        The callback for state events.
    """

    def __init__(
        self,
        config: types.SimpleNamespace,
        topics: salobj.Controller | types.SimpleNamespace,
        log: logging.Logger,
    ) -> None:
        self.config = config
        self.topics = topics
        self.log = log.getChild(type(self).__name__)

        # Load the shared library using ctypes.
        pf = platform.system()
        extension = EXTENSIONS[pf]
        libname = (
            pathlib.Path(__file__).parent / "data" / "lib330" / f"libq330.{extension}"
        )
        self.libq330 = ctypes.CDLL(str(libname))

        self.q330_state: TState | None = None

        self.aminiseed_callback = self._get_aminiseed_callback()
        self.message_callback = self._get_message_callback()
        self.miniseed_callback = self._get_miniseed_callback()
        self.secdata_callback = self._get_secdata_callback()
        self.state_callback = self._get_state_callback()

        # Initialize the telemetry topics.
        for topic_name in ["BH", "BL", "HH", "HL", "LH", "LL", "UH", "VH"]:
            topic: salobj.topics.WriteTopic = self._get_topic_for_name(topic_name)
            topic.set(sensorName=self.config.sensor_name, location=self.config.location)

        # Dict holding the earthquake data until the telemetry can be sent.
        self.channel_data = dict()
        for topic_name in ["BH", "BL", "HH", "HL", "LH", "LL", "UH", "VH"]:
            self.channel_data[topic_name] = ChannelDataHolder(
                topic_name="",
                timestamp=0.0,
                accelerationEastWest=[],
                accelerationNorthSouth=[],
                accelerationZenith=[],
            )

        # Telemetry processing Queue, Thread and Event.
        self.telemetry_queue: queue.Queue = queue.Queue()
        self.telemetry_thread: threading.Thread | None = None
        self.telemetry_event = threading.Event()

    def _get_topic_for_name(self, topic_name: str) -> salobj.topics.WriteTopic:
        """Convenience method to look up the SAL topic for a topic name.

        Parameters
        ----------
        topic_name : `str`
            The topic name.

        Returns
        -------
        salobj.topics.WriteTopic
            The topic with the topic name.
        """
        telemetry_topic_name = TOPIC_NAME_DICT[topic_name]
        return getattr(self.topics, f"tel_{telemetry_topic_name}")

    def _timestamp_to_tai(self, timestamp: float) -> float:
        """Convenience method to convert a Q330 timestamp to TAI.

        Parameters
        ----------
        timestamp : `float`
            The Q330 timestamp.

        Returns
        -------
        float
            The TAI in Unix seconds.
        """
        return utils.tai_from_utc(timestamp + JAN_FIRST_2000, "unix")

    def _get_aminiseed_callback(self) -> Callable:
        """Closure for getting the aminiseed callback."""

        @ctypes.CFUNCTYPE(ctypes.c_void_p)
        def aminiseed_callback() -> None:
            """Callback method for aminiseed data.

            This method is never expected to be called due to the way the Q330
            library is configured.
            """
            self.log.debug("aminiseed_callback")

        return aminiseed_callback

    def _get_message_callback(self) -> Callable:
        """Closure for getting the message callback."""

        @ctypes.CFUNCTYPE(ctypes.c_void_p)
        def message_callback() -> None:
            """Callback method for message data.

            Only log the received message data at DEBUG level and do not
            process further.
            """
            message = self.libq330.get_message()
            self.log.debug(
                f"{message.msgcount=}, {message.code=}, "
                f"message.timestamp={message.timestamp}={self._timestamp_to_tai(message.timestamp)}, "
                f"{message.datatime=}, message.message={message.result.decode('utf-8')}"
                f"{message.suffix.decode('utf-8')}."
            )

        return message_callback

    def _get_miniseed_callback(self) -> Callable:
        """Closure for getting the miniseed callback."""

        @ctypes.CFUNCTYPE(ctypes.c_void_p)
        def miniseed_callback() -> None:
            """Callback method for miniseed data.

            The data is not useful for our purposes so it is ignored.
            """
            self.log.debug("miniseed_callback")

        return miniseed_callback

    def _get_secdata_callback(self) -> Callable:
        """Closure for getting the secdata callback."""

        @ctypes.CFUNCTYPE(ctypes.c_void_p)
        def secdata_callback() -> None:
            """Callback method for onesec data.

            Process the onesec data which contains the earthquake telemetry
            data. The data get prepared for processing and then passed on to
            the _process_samples method.
            """
            one_sec = self.libq330.get_onesec()
            num_samples = one_sec.rate
            sample_rate = one_sec.rate
            if num_samples < 0:
                num_samples = 1
                sample_rate = -1.0 / one_sec.rate
            start_date = self._timestamp_to_tai(one_sec.timestamp)
            end_date = start_date + (num_samples - 1) / sample_rate
            samples: list[float] = []
            sample_num = 0
            channel_name = one_sec.channel.decode("utf-8")
            for sample in one_sec.samples:
                samples.append(sample)
                sample_num += 1
                if sample_num >= num_samples:
                    break
            self.log.debug(
                f"station_name={one_sec.station_name.decode('utf-8')}, "
                f"location={one_sec.location.decode('utf-8')}, "
                f"chan_number={one_sec.chan_number}, "
                f"channel={channel_name}, "
                f"rate={one_sec.rate}, "
                f"{sample_rate=}, "
                f"start_date={one_sec.timestamp}={start_date}, "
                f"end_date={end_date}, "
                f"samples={', '.join(map(str, samples))}."
            )
            self._process_samples(start_date, channel_name, samples)

        return secdata_callback

    def _process_samples(
        self, timestamp: float, channel_name: str, samples: list[float]
    ) -> None:
        """Process the onesec samples.

        Parameters
        ----------
        timestamp : `float`
            TAI time in Unix seconds for the first sample.
        channel_name : `ChannelName`
            The channel name.
        samples : `list`[`float`]
            The samples to process.

        Notes
        -------
        The E, N and Z components of earthquake data are received in separate
        messages. The data for the three components is collected and queued for
        sending as telemetry when channel data with a newer timestamp is
        received.
        """
        topic_name = channel_name[:2]
        if topic_name in ["BH", "BL", "HH", "HL", "LH", "LL", "UH", "VH"]:
            if self.channel_data[topic_name].timestamp != timestamp:
                cdh = self.channel_data[topic_name]
                if cdh.topic_name:
                    self.log.debug(
                        f"Appending [{cdh.topic_name=}, {cdh.timestamp=}, "
                        f"{cdh.accelerationEastWest=}, {cdh.accelerationNorthSouth=}, "
                        f"{cdh.accelerationZenith=}] telemetry."
                    )
                    self.telemetry_queue.put(cdh)
                self.channel_data[topic_name] = ChannelDataHolder(
                    topic_name=topic_name,
                    timestamp=timestamp,
                    accelerationEastWest=[],
                    accelerationNorthSouth=[],
                    accelerationZenith=[],
                )
            match channel_name[2]:
                case "E":
                    self.channel_data[topic_name].accelerationEastWest = samples
                case "N":
                    self.channel_data[topic_name].accelerationNorthSouth = samples
                case "Z":
                    self.channel_data[topic_name].accelerationZenith = samples
                case _:
                    self.log.error(f"Unknown channel {channel_name}.")

    def _get_state_callback(self) -> Callable:
        """Closure for getting the state callback."""

        @ctypes.CFUNCTYPE(ctypes.c_void_p)
        def state_callback() -> None:
            """Callback method for state data.

            Receive the state data and use the Lib330 library to start the data
            flow when the LIBSTATE_RUNWAIT `TLibState` is encountered.
            """
            self.q330_state = self.libq330.get_state()
            assert self.q330_state is not None
            if self.q330_state.state_type == 0:
                self.log.debug(
                    f"state_type={self.q330_state.state_type}={TStateType(self.q330_state.state_type).name}, "
                    f"station_name={self.q330_state.station_name.decode('utf-8')}, "
                    f"subtype={self.q330_state.subtype}, "
                    f"info={self.q330_state.info=}={TLibState(self.q330_state.info).name}, "
                    f"state_name={self.q330_state.state_name.decode('utf-8')}."
                )
                if self.q330_state.info == TLibState.LIBSTATE_RUNWAIT:
                    self.libq330.q330_change_state(TLibState.LIBSTATE_RUN.value)
                    self.log.debug("Telemetry stream started.")

        return state_callback

    async def connect(self) -> None:
        """Connect to the earthquake sensor.

        Before connecting, the lib330 library is initialized and a connection
        context is created. Connecting is performed by sending a register
        request to the earthquake sensor.
        """
        self.libq330.get_message.restype = TMsg
        self.libq330.get_onesec.restype = TOnesec
        self.libq330.get_state.restype = TState

        serial_id = int(self.config.serial_id, 16)
        self.log.debug(f"{serial_id=}")
        init = TInit(
            serial_id=serial_id,
            hostname=self.config.host.encode("utf-8"),
            baseport=self.config.port,
            debug=False,
            aminiseed_callback=self.aminiseed_callback,
            message_callback=self.message_callback,
            miniseed_callback=self.miniseed_callback,
            secdata_callback=self.secdata_callback,
            state_callback=self.state_callback,
        )
        self.libq330.q330_init(init)
        self.libq330.q330_create_context()
        self.libq330.q330_register()

        await self.stop_telemetry_thread()
        self.log.debug("Starting telemetry thread.")
        self.telemetry_thread = threading.Thread(target=self._telemetry_worker)
        self.telemetry_thread.start()
        self.log.debug("Done starting telemetry thread.")

    def _telemetry_worker(self) -> None:
        telemetry_loop = asyncio.new_event_loop()
        asyncio.set_event_loop(telemetry_loop)
        self.log.debug("Starting telemetry loop.")
        asyncio.run(self._process_telemetry())

    async def _process_telemetry(self) -> None:
        """Process pending telemetry.

        Check the queue for pending telemetry and write it if there is any.
        """
        if not self.telemetry_event:
            raise RuntimeError("No telemetry event.")
        while True:
            if self.telemetry_event.is_set():
                break
            self.log.debug(
                f"Length of telemetry queue = {self.telemetry_queue.qsize()}"
            )
            if self.telemetry_queue.empty():
                await asyncio.sleep(TELEMETRY_WAIT)
                continue
            cdh = self.telemetry_queue.get(False)
            topic = self._get_topic_for_name(cdh.topic_name)
            self.log.debug(
                f"Writing [{cdh.topic_name=}, {cdh.timestamp=}, "
                f"{cdh.e=}, {cdh.n=}, {cdh.z=}] telemetry."
            )
            await topic.set_write(timestamp=cdh.timestamp, e=cdh.e, n=cdh.n, z=cdh.z)

    async def disconnect(self) -> None:
        """Disconnect from the earthquake sensor.

        Disconnecting is performed by instructing the lib330 library to destroy
        the conneciton context.
        """
        await self.stop_telemetry_thread()

        self.libq330.q330_change_state(TLibState.LIBSTATE_IDLE.value)
        await asyncio.sleep(1.0)
        self.libq330.q330_change_state(TLibState.LIBSTATE_TERM.value)
        await asyncio.sleep(1.0)
        self.libq330.q330_destroy_context()

    async def stop_telemetry_thread(self) -> None:
        self.telemetry_event.clear()
        if self.telemetry_thread:
            self.log.debug("Stopping telemetry thread.")
            self.telemetry_event.set()
            self.telemetry_thread.join(timeout=THREAD_STOP_WAIT)
            self.log.debug("Done stopping telemetry thread.")
