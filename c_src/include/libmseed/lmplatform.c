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

/***************************************************************************
 * lmplatform.c:
 *
 * Platform portability routines.
 *
 * modified: 2010.304
 ***************************************************************************/

/* Define _LARGEFILE_SOURCE to get ftello/fseeko on some systems (Linux) */
#define _LARGEFILE_SOURCE 1

#include "lmplatform.h"

/***************************************************************************
 * lmp_ftello:
 *
 * Return the current file position for the specified descriptor using
 * the system's closest match to the POSIX ftello.
 ***************************************************************************/
off_t lmp_ftello(FILE *stream) {
#if defined(LMP_WIN)
    return (off_t)ftell(stream);

#else
    return (off_t)ftello(stream);

#endif
} /* End of lmp_ftello() */

/***************************************************************************
 * lmp_fseeko:
 *
 * Seek to a specific file position for the specified descriptor using
 * the system's closest match to the POSIX fseeko.
 ***************************************************************************/
int lmp_fseeko(FILE *stream, off_t offset, int whence) {
#if defined(LMP_WIN)
    return (int)fseek(stream, (long int)offset, whence);

#else
    return (int)fseeko(stream, offset, whence);

#endif
} /* End of lmp_fseeko() */
