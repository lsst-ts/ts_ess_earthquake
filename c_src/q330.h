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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef q330_h
#define q330_h

#include <stdbool.h>
#include <stdint.h>

typedef struct { /* parameters to initialize Q330 */
    uint64_t serial_id;
    string250 hostname;
    word baseport;
    bool debug;
    void *aminiseed_callback;
    void *message_callback;
    void *miniseed_callback;
    void *secdata_callback;
    void *state_callback;
} tinit;

typedef struct {        /* format for miniseed and archival miniseed */
    char *station_name; /* network and station */
    char *location;
    byte chan_number; /* channel number according to tokens */
    char *channel;
    integer rate;                          /* sampling rate */
    longword cl_session;                   /* closed loop session number */
    double cl_offset;                      /* closed loop time offset */
    double timestamp;                      /* Time of data, corrected for any filtering */
    word filter_bits;                      /* OMF_xxx bits */
    enum tpacket_class packet_class;       /* type of record */
    enum tminiseed_action miniseed_action; /* what this packet represents */
    word data_size;                        /* size of actual miniseed data */
    pointer data_address; /* pointer to miniseed record, which is a byte array of size data_size */
} tminiseed;

typedef struct {       /* parameters of a Q330 message */
    longword msgcount; /* number of messages */
    word code;
    longword timestamp;
    longword datatime;
    char *result; /* string representation of code */
    char *suffix; /* optional additional information */
} tmsg;

typedef struct {         /* for 1 second and low latency callback */
    longword total_size; /* number of bytes in buffer passed */
    char *station_name;  /* network and station */
    char *location;
    byte chan_number; /* channel number according to tokens */
    char *channel;
    word padding;
    integer rate;              /* sampling rate */
    longword cl_session;       /* closed loop session number */
    longword reserved;         /* must be zero */
    double cl_offset;          /* closed loop time offset */
    double timestamp;          /* time of data, corrected for any filtering */
    word filter_bits;          /* OSF_xxx bits */
    word qual_perc;            /* time quality percentage */
    word activity_flags;       /* same as in Miniseed */
    word io_flags;             /* same as in Miniseed */
    word data_quality_flags;   /* same as in Miniseed */
    byte src_channel;          /* source blockette channel */
    byte src_subchan;          /* source blockette sub-channel */
    longint samples[MAX_RATE]; /* decompressed samples */
} tonesec;

typedef struct {                 /* format for state callback */
    enum tstate_type state_type; /* reason for this message */
    char *station_name;
    longword subtype; /* to further narrow it down */
    longword info;    /* new highest message for ST_MSG, new state for ST_STATE, or new status available for
                         ST_STATUS */
    char *state_name; /* string representation of state_type */
} tstate;

void q330_init(tinit qinit);
uint64_t q330_get_serial_id();
tminiseed get_aminiseed();
tmsg get_message();
tminiseed get_miniseed();
tonesec get_onesec();
tstate get_state();
void q330_create_context();
void q330_unregistered_ping();
void q330_register();
void q330_change_state(enum tlibstate new_state);
void q330_destroy_context();

#endif  // !q330_h
