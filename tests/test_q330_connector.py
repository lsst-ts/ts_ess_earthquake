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

import logging
import types
import unittest
from unittest import mock

from lsst.ts.ess import earthquake


class Q330ConnectorTestCase(unittest.IsolatedAsyncioTestCase):
    @mock.patch("lsst.ts.ess.earthquake.q330_connector.ctypes.CDLL", mock.MagicMock())
    async def test_q330_connector(self) -> None:
        config = types.SimpleNamespace(
            host="127.0.0.1", port=6330, serial_id="0123456789ABCDEF"
        )
        topics = types.SimpleNamespace()
        log = logging.getLogger(type(self).__name__)
        q330_connector = earthquake.Q330Connector(config=config, topics=topics, log=log)
        assert q330_connector is not None

        q330_connector.q330_state = earthquake.TState()
        q330_connector.q330_state.info = earthquake.TLibState.LIBSTATE_RUNWAIT.value

        await q330_connector.connect()
        await q330_connector.disconnect()
