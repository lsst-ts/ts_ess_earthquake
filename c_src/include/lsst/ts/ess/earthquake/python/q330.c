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

#include <stdbool.h>
#include <stdio.h>
#include "libclient.h"
#include "libmsgs.h"
#include "libstrucs.h"
#include "q330.h"

tpar_create creation_info;
tcontext station_context;
uint64_t q330id_serial_id;
string250 q330id_address;
int q330id_baseport;
bool debug = false;

string95 result;
string63 state_name;
tminiseed aminiseed;
tmsg message;
tminiseed miniseed;
tonesec onesec;
tstate state;

void (*aminiseed_callback_)();
void (*message_callback_)();
void (*miniseed_callback_)();
void (*secdata_callback_)();
void (*state_callback_)();

/**
 * Callback for an aminiseed event.
 *
 * @param p Pointer to a tminiseed_call struct.
 */
void my_aminiseed_callback(pointer p) {
    tminiseed_call *data = (tminiseed_call *)p;
    if (debug) {
        printf("[Q330] my_aminiseed_callback: %s %s %s\n", data->station_name, data->location, data->channel);
    }
    aminiseed.station_name = data->station_name;
    aminiseed.location = data->location;
    aminiseed.chan_number = data->chan_number;
    aminiseed.channel = data->channel;
    aminiseed.rate = data->rate;
    aminiseed.cl_session = data->cl_session;
    aminiseed.cl_offset = data->cl_offset;
    aminiseed.timestamp = data->timestamp;
    aminiseed.filter_bits = data->filter_bits;
    aminiseed.packet_class = data->packet_class;
    aminiseed.miniseed_action = data->miniseed_action;
    aminiseed.data_size = data->data_size;
    aminiseed.data_address = data->data_address;
    if (aminiseed_callback_) {
        aminiseed_callback_();
    }
}

/**
 * Callback for a message event.
 *
 * @param p Pointer to a tmsg_call struct.
 */
void my_msg_callback(pointer p) {
    tmsg_call *data = (tmsg_call *)p;
    lib_get_msg(data->code, &result);
    if (debug) {
        printf("[Q330] my_msg_callback: msgcount='%d' code='%d' timestamp='%u' datatime='%d' msg='%s%s'.\n",
               data->msgcount, data->code, data->timestamp, data->datatime, result, data->suffix);
    }
    message.msgcount = data->msgcount;
    message.code = data->code;
    message.timestamp = data->timestamp;
    message.datatime = data->datatime;
    message.result = result;
    message.suffix = data->suffix;
    if (message_callback_) {
        message_callback_();
    }
}

/**
 * Callback for a miniseed event.
 *
 * @param p Pointer to a tminiseed_call struct.
 */
void my_miniseed_callback(pointer p) {
    tminiseed_call *data = (tminiseed_call *)p;
    if (debug) {
        printf("[Q330] my_miniseed_callback: %s %s %s\n", data->station_name, data->location, data->channel);
    }
    miniseed.station_name = data->station_name;
    miniseed.location = data->location;
    miniseed.chan_number = data->chan_number;
    miniseed.channel = data->channel;
    miniseed.rate = data->rate;
    miniseed.cl_session = data->cl_session;
    miniseed.cl_offset = data->cl_offset;
    miniseed.timestamp = data->timestamp;
    miniseed.filter_bits = data->filter_bits;
    miniseed.packet_class = data->packet_class;
    miniseed.miniseed_action = data->miniseed_action;
    miniseed.data_size = data->data_size;
    miniseed.data_address = data->data_address;
    if (miniseed_callback_) {
        miniseed_callback_();
    }
}

/**
 * Callback for an secdata event.
 *
 * @param p Pointer to a tonesec_call struct.
 */
void my_secdata_callback(pointer p) {
    tonesec_call *data = (tonesec_call *)p;
    if (debug) {
        printf("[Q330] my_secdata_callback: %s %s", data->station_name, data->location);
    }
    onesec.total_size = data->total_size;
    onesec.station_name = data->station_name;
    onesec.location = data->location;
    onesec.chan_number = data->chan_number;
    onesec.channel = data->channel;
    onesec.padding = data->padding;
    onesec.rate = data->rate;
    onesec.cl_session = data->cl_session;
    onesec.reserved = data->reserved;
    onesec.cl_offset = data->cl_offset;
    onesec.timestamp = data->timestamp;
    onesec.filter_bits = data->filter_bits;
    onesec.qual_perc = data->qual_perc;
    onesec.activity_flags = data->activity_flags;
    onesec.io_flags = data->io_flags;
    onesec.data_quality_flags = data->data_quality_flags;
    onesec.src_channel = data->src_channel;
    onesec.src_subchan = data->src_subchan;
    int32_t num_samples = data->rate;
    if (data->rate < 0) {
        num_samples = 1;
    }
    for (int i = 0; i < num_samples; i++) {
        onesec.samples[i] = data->samples[i];
    }

    if (secdata_callback_) {
        secdata_callback_();
    }
}

/**
 * Callback for an secdata event.
 *
 * @param p Pointer to a tstate_call struct.
 */
void my_state_callback(pointer p) {
    tstate_call *data = (tstate_call *)p;
    enum tlibstate state_info = data->info;
    lib_get_statestr(state_info, &state_name);
    if (debug) {
        printf("[Q330] my_state_callback: '%u' == '%s'\n", data->info, state_name);
    }
    state.state_type = data->state_type;
    state.station_name = data->station_name;
    state.subtype = data->subtype;
    state.info = data->info;
    state.state_name = state_name;
    if (state_callback_) {
        state_callback_();
    }
}

/**
 * Get the latest aminiseed.
 *
 * @return The latest aminiseed as a tminiseed struct.
 */
tminiseed get_aminiseed() { return aminiseed; }

/**
 * Get the latest message.
 *
 * @return The latest message as a tmesg struct.
 */
tmsg get_message() { return message; }

/**
 * Get the latest miniseed.
 *
 * @return The latest miniseed as a tminiseed struct.
 */
tminiseed get_miniseed() { return miniseed; }

/**
 * Get the latest onesec.
 *
 * @return The latest onesec as a tonesec struct.
 */
tonesec get_onesec() { return onesec; }

/**
 * Get the latest state.
 *
 * @return The latest state as a tstate struct.
 */
tstate get_state() { return state; }

/**
 * Convenience function to create a creation_info struct.
 */
void create_creation_info() {
    memcpy(creation_info.q330id_serial, &q330id_serial_id, sizeof(uint64_t));
    creation_info.q330id_dataport = LP_TEL1;
    strncpy(creation_info.q330id_station, "UNKN", 5);
    creation_info.host_timezone = 0;
    strcpy(creation_info.host_software, "UNKNOWN");
    strcpy(creation_info.opt_contfile, "");
    creation_info.opt_verbose =
            VERB_SDUMP | VERB_RETRY | VERB_REGMSG | VERB_LOGEXTRA | VERB_AUXMSG | VERB_PACKET;
    creation_info.opt_zoneadjust = 0;
    creation_info.opt_secfilter = OSF_ALL;
    creation_info.opt_client_msgs = 10;
#ifndef OMIT_SEED
    creation_info.opt_compat = 0;
    creation_info.opt_minifilter = OMF_ALL;
    creation_info.opt_aminifilter = 0;
    creation_info.amini_exponent = 14;
    creation_info.amini_512highest = -1000;
    creation_info.mini_embed = 0;
    creation_info.mini_separate = 1;
    creation_info.mini_firchain = 0;
    creation_info.call_minidata = my_miniseed_callback;
    creation_info.call_aminidata = my_aminiseed_callback;
#endif
    creation_info.resp_err = LIBERR_NOERR;
    creation_info.call_state = my_state_callback;
    creation_info.call_messages = my_msg_callback;
    creation_info.call_secdata = my_secdata_callback;
    creation_info.call_lowlatency = NULL;
    creation_info.call_baler = NULL;
    creation_info.file_owner = NULL;
}

/**
 * Convenience function to create a registration_info struct.
 */
tpar_register create_registration_info() {
    tpar_register registration_info;
    uint64_t auth = strtoll("0", NULL, 16);
    memcpy(registration_info.q330id_auth, &auth, sizeof(uint64_t));
    strcpy(registration_info.q330id_address, q330id_address);
    registration_info.q330id_baseport = q330id_baseport;
    // This is “HOST_ETH” for Ethernet, “HOST_SER” for serial, or “HOST_TCP” for TCP using Tunnel330.
    registration_info.host_mode = HOST_ETH;
    strcpy(registration_info.host_interface, "");
    registration_info.host_mincmdretry = 5;
    registration_info.host_maxcmdretry = 40;
    registration_info.host_ctrlport = 0;
    registration_info.host_dataport = 0;
    registration_info.opt_latencytarget = 0;
    registration_info.opt_closedloop = 0;
    registration_info.opt_dynamic_ip = 0;
    registration_info.opt_hibertime = 0;
    registration_info.opt_conntime = 0;
    registration_info.opt_connwait = 0;
    registration_info.opt_regattempts = 0;
    registration_info.opt_ipexpire = 0;
    registration_info.opt_buflevel = 0;
    return registration_info;
}

/**
 * Convenience function to initialize the lib330 library.
 */
void q330_init(tinit qinit) {
    debug = qinit.debug;
    if (debug) {
        printf("[Q330] Set debug to %d\n", debug);
    }
    q330id_serial_id = qinit.serial_id;
    if (debug) {
        printf("[Q330] Set serial id '0x%llX'.\n", q330id_serial_id);
    }
    strcpy(q330id_address, qinit.hostname);
    if (debug) {
        printf("[Q330] Set hostname '%s'.\n", q330id_address);
    }
    q330id_baseport = qinit.baseport;
    if (debug) {
        printf("[Q330] Setting baseport '%d'.\n", q330id_baseport);
    }
    aminiseed_callback_ = qinit.aminiseed_callback;
    message_callback_ = qinit.message_callback;
    miniseed_callback_ = qinit.miniseed_callback;
    secdata_callback_ = qinit.secdata_callback;
    state_callback_ = qinit.state_callback;
}

/**
 * Convenience function to convert the q330id_serial 32 bit int array to a 64 bit int.
 */
uint64_t q330_get_serial_id() {
    uint32_t leastSignificantWord = creation_info.q330id_serial[0];
    uint32_t mostSignificantWord = creation_info.q330id_serial[1];
    uint64_t ser_id = (uint64_t)mostSignificantWord << 32 | leastSignificantWord;
    return ser_id;
}

/**
 * Convenience function to create the lib330 connection context.
 */
void q330_create_context() {
    create_creation_info();
    if (debug) {
        printf("[Q330] Creating context for Q330 with serial number 0x%llX.\n", q330_get_serial_id());
    }
    lib_create_context(&station_context, &creation_info);
    if (debug) {
        printf("[Q330] Context creation error code %d\n", creation_info.resp_err);
    }
}

/**
 * Convenience function to perform a ping without registering with the Q330 earthquake sensor.
 */
void q330_unregistered_ping() {
    tpar_register registration_info = create_registration_info();
    if (debug) {
        printf("[Q330] Pinging Q330 with serial number 0x%llX.\n", q330_get_serial_id());
    }
    enum tliberr errcode = lib_unregistered_ping(station_context, &registration_info);
    if (debug) {
        printf("[Q330] Ping error code %d\n", errcode);
    }
}

/**
 * Convenience function to register with the Q330 earthquake sensor.
 */
void q330_register() {
    tpar_register registration_info = create_registration_info();
    if (debug) {
        printf("[Q330] Registering with Q330 with serial number 0x%llX.\n", q330_get_serial_id());
    }
    enum tliberr errcode = lib_register(station_context, &registration_info);
    if (debug) {
        printf("[Q330] Registration error code %d\n", errcode);
    }
}

/**
 * Convenience function to change the state of the Q330 earthquake sensor.
 */
void q330_change_state(enum tlibstate new_state) {
    lib_change_state(station_context, new_state, LIBERR_NOERR);
}

/**
 * Convenience function to destroy the lib330 connection context.
 */
void q330_destroy_context() {
    if (debug) {
        printf("[Q330] Destroying context for Q330 with serial number 0x%llX.\n", q330_get_serial_id());
    }
    pq330 q330;
    pmem_manager pm, pmn;
    q330 = station_context;
    station_context = NIL;
    pthread_mutex_destroy(addr(q330->mutex));
    pthread_mutex_destroy(addr(q330->msgmutex));
    pm = q330->memory_head;
    while (pm) {
        pmn = pm->next;
        free(pm->base);
        free(pm);
        pm = pmn;
    }
    pm = q330->thrmem_head;
    while (pm) {
        pmn = pm->next;
        free(pm->base);
        free(pm);
        pm = pmn;
    }
}
