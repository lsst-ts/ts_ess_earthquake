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
import types
from collections.abc import Callable

from lsst.ts import salobj

from .q330_utils import TInit, TLibState, TMsg, TState, TStateType

EXTENSIONS = {"Linux": "so", "Darwin": "dylib"}


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

    def _get_aminiseed_callback(self) -> Callable:
        """Closure for getting the aminiseed callback."""

        @ctypes.CFUNCTYPE(ctypes.c_void_p)
        def aminiseed_callback() -> None:
            self.log.debug("aminiseed_callback")

        return aminiseed_callback

    def _get_message_callback(self) -> Callable:
        """Closure for getting the message callback."""

        @ctypes.CFUNCTYPE(ctypes.c_void_p)
        def message_callback() -> None:
            message = self.libq330.get_message()
            self.log.debug(
                f"message_callback: {message.msgcount=}, {message.code=}, {message.timestamp=}, "
                f"{message.datatime=}, message.message={message.result.decode('utf-8')}"
                f"{message.suffix.decode('utf-8')}."
            )

        return message_callback

    def _get_miniseed_callback(self) -> Callable:
        """Closure for getting the miniseed callback."""

        @ctypes.CFUNCTYPE(ctypes.c_void_p)
        def miniseed_callback() -> None:
            self.log.debug("miniseed_callback")

        return miniseed_callback

    def _get_secdata_callback(self) -> Callable:
        """Closure for getting the secdata callback."""

        @ctypes.CFUNCTYPE(ctypes.c_void_p)
        def secdata_callback() -> None:
            self.log.debug("secdata_callback")

        return secdata_callback

    def _get_state_callback(self) -> Callable:
        """Closure for getting the state callback."""

        @ctypes.CFUNCTYPE(ctypes.c_void_p)
        def state_callback() -> None:
            self.q330_state = self.libq330.get_state()
            assert self.q330_state is not None
            if self.q330_state.state_type == 0:
                self.log.debug(
                    "state_callback: "
                    f"state_type={self.q330_state.state_type}={TStateType(self.q330_state.state_type).name}, "
                    f"station_name={self.q330_state.station_name.decode('utf-8')}, "
                    f"subtype={self.q330_state.subtype}, "
                    f"info={self.q330_state.info=}={TLibState(self.q330_state.info).name}, "
                    f"state_name={self.q330_state.state_name.decode('utf-8')}."
                )

        return state_callback

    async def connect(self) -> None:
        """Connect to the earthquake sensor.

        Before connecting, the lib330 library is initialized and a connection
        context is created. Connecting is performed by sending a register
        request to the earthquake sensor.
        """
        self.libq330.get_message.restype = TMsg
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
        while self.q330_state is None:
            await asyncio.sleep(0.1)
        self.log.debug("Q330 state is not None.")
        # Now wait until the sensor is ready to start.
        while not (
            self.q330_state.state_type == TStateType.ST_STATE.value
            and self.q330_state.info == TLibState.LIBSTATE_RUNWAIT.value
        ):
            await asyncio.sleep(0.1)
        self.log.debug("Ready to send telemetry.")
        # Ready, so start the telemetry stream.
        self.libq330.q330_change_state(TLibState.LIBSTATE_RUN.value)
        self.log.debug("Telemetry stream started.")

    async def disconnect(self) -> None:
        """Disconnect from the earthquake sensor.

        Disconnecting is performed by instructing the lib330 library to destroy
        the conneciton context.
        """
        self.libq330.q330_change_state(TLibState.LIBSTATE_IDLE.value)
        await asyncio.sleep(1.0)
        self.libq330.q330_change_state(TLibState.LIBSTATE_TERM.value)
        await asyncio.sleep(1.0)
        self.libq330.q330_destroy_context()
