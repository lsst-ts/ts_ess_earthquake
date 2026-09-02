/*
 * This file is part of ts_ess_earthquake.
 *
 * Developed for the Vera C. Rubin Observatory Telescope and Site Systems.
 * This product includes software developed by the LSST Project
 * (https://www.lsst.org).
 * See the COPYRIGHT file at the top-level directory of this distribution
 * for details of code ownership.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

/*   Lib330 common type definitions
     Copyright 2006 Certified Software Corporation

    This file is part of Lib330

    Lib330 is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Lib330 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Lib330; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Edit History:
   Ed Date       By  Changes
   -- ---------- --- ---------------------------------------------------
    0 2006-09-09 rdr Created
*/
#ifndef libtypes_h
#include "libtypes.h"
#endif

const tclock default_clock = {
  0, /* timezone offset in seconds */
  10, /* loss in lock in minutes before degrading 1% */
  100, /* PLL Locked quality */
  90, /* PLL Tracking quality */
  80, /* PLL Holding quality */
  80, /* Currently Locked, PLL Off */
  0, /* Spare */
  60, /* has been locked highest quality */
  10, /* has been locked lowest quality */
  0, /* Never been locked quality */
  0 } ; /* minimum seconds between clock messages */
const tbauds bauds = {300, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400} ;
