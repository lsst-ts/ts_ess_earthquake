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

__all__ = ["EarthquakeDataClient"]

import logging
import types
import typing

import yaml
from lsst.ts import salobj
from lsst.ts.ess import common

from .q330_connector import Q330Connector


class EarthquakeDataClient(common.data_client.BaseReadLoopDataClient):
    """Class to read data from an earthquake sensor and publish it as ESS
    telemetry.

    Parameters
    ----------
    config : `types.SimpleNamespace`
        The configuration, after validation by the schema returned
        by `get_config_schema` and conversion to a types.SimpleNamespace.
    topics : `salobj.Controller`
        The telemetry topics this model can write, as a struct with attributes
        such as ``tel_temperature``.
    log : `logging.Logger`
        Logger.
    simulation_mode : `int`, optional
        Simulation mode; 0 for normal operation.
    auto_reconnect : `bool`
        Automatically disconnect and reconnect in case of a read error
        (default: False)?

    Notes
    -----
    The config is required to contain "max_read_timeouts". If it doesn't, a
    RuntimeError is raised at instantiation.
    """

    def __init__(
        self,
        config: types.SimpleNamespace,
        topics: salobj.Controller | types.SimpleNamespace,
        log: logging.Logger,
        simulation_mode: int = 0,
        auto_reconnect: bool = False,
    ) -> None:
        super().__init__(
            config=config,
            topics=topics,
            log=log,
            simulation_mode=simulation_mode,
            auto_reconnect=auto_reconnect,
        )
        self.q330_connector = Q330Connector(config=config, topics=topics, log=log)

    @classmethod
    def get_config_schema(cls) -> dict[str, typing.Any]:
        """Get the config schema as jsonschema dict."""
        return yaml.safe_load(
            """
$schema: http://json-schema.org/draft-07/schema#
description: Schema for ControllerDataClient
type: object
properties:
  host:
    description: IP address of the TCP/IP interface.
    type: string
    format: hostname
  port:
    description: Port number of the TCP/IP interface.
    type: integer
  serial_id:
    description: Hexadecimal serial ID of the earthquake sensor.
    type: string
  max_read_timeouts:
    description: Maximum number of read timeouts before an exception is raised.
    type: integer
    default: 5
  sensor_name:
    description: The name of the sensor.
    type: string
  location:
    description: Sensor location (used for all telemetry topics).
    type: string
required:
  - host
  - port
  - serial_id
  - max_read_timeouts
  - location
additionalProperties: false
"""
        )

    def descr(self) -> str:
        """Return a brief description, without the class name.

        This should be just enough information to distinguish
        one instance of this client from another.
        """
        return f"[host={self.config.host}, port={self.config.port}]"

    async def connect(self) -> None:
        """Connect to the earthquake sensor.

        This will not be called if already connected.
        See the Raises section for exceptions subclasses should raise.

        Raises
        ------
        ConnectionError
            If the data server rejects the connection.
            This may happen if the data server is down
            or the configuration specified an invalid address.
        asyncio.TimeoutError
            If a connection cannot be made in reasonable time.
        Exception
            (or any subclass) if any other serious problem occurs.
        """
        await self.q330_connector.connect()

    async def disconnect(self) -> None:
        """Disconnect from the earthquake sensor.

        This must always be safe to call, whether connected or not.
        """
        await self.q330_connector.disconnect()

    async def read_data(self) -> None:
        """Read data.

        Notes
        -----
        This method is left empty because all data reading is implemented in a
        separate thread in the Lib330 C library.
        """
        # Empty on purpose.
        pass
