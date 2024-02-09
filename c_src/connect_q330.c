
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "libclient.h"
#include "q330.h"

void do_sleep(int sleep_time) {
    printf("Sleep for %d seconds.\n", sleep_time);
    sleep(sleep_time);
    printf("Wake up.\n");
}

void q330_aminiseed_callback() {
    tminiseed aminiseed = get_aminiseed();
    printf("get_aminiseed: station_name='%s' location='%s' chan_number='%d' channel='%s' rate='%d'"
           "timestamp='%f' packet_class='%d' miniseed_action='%d', data_size='%d'.\n",
           aminiseed.station_name, aminiseed.location, aminiseed.chan_number, aminiseed.channel,
           aminiseed.rate, aminiseed.timestamp, aminiseed.packet_class, aminiseed.miniseed_action,
           aminiseed.data_size);
}

void q330_message_callback() {
    tmsg message = get_message();
    printf("get_message: msgcount='%d' code='%d' timestamp='%u' datatime='%d', message='%s%s'.\n",
           message.msgcount, message.code, message.timestamp, message.datatime, message.result,
           message.suffix);
}

void q330_miniseed_callback() {
    tminiseed miniseed = get_miniseed();
    printf("get_miniseed: station_name='%s' location='%s' chan_number='%d' channel='%s' rate='%d'"
           "timestamp='%f' packet_class='%d' miniseed_action='%d', data_size='%d'.\n",
           miniseed.station_name, miniseed.location, miniseed.chan_number, miniseed.channel, miniseed.rate,
           miniseed.timestamp, miniseed.packet_class, miniseed.miniseed_action, miniseed.data_size);
}

void q330_secdata_callback() { printf("q330_secdata_callback\n"); }

void q330_state_callback() {
    tstate state = get_state();
    printf("get_state: state_type='%d' station_name='%s' subtype='%d' info='%d' state_name='%s'.\n",
           state.state_type, state.station_name, state.subtype, state.info, state.state_name);
}

int main() {
    int short_sleep_time = 1;
    int long_sleep_time = 25;
    uint64_t serial_id = 0x01000018753C8C49;
    string250 hostname = "139.229.178.22";
    word baseport = 6330;

    tinit qinit;
    qinit.serial_id = serial_id;
    strcpy(qinit.hostname, hostname);
    qinit.baseport = baseport;
    qinit.debug = false;
    qinit.aminiseed_callback = q330_aminiseed_callback;
    qinit.message_callback = q330_message_callback;
    qinit.miniseed_callback = q330_miniseed_callback;
    qinit.secdata_callback = q330_secdata_callback;
    qinit.state_callback = q330_state_callback;
    q330_init(qinit);

    q330_create_context();
    do_sleep(short_sleep_time);
    q330_unregistered_ping();
    do_sleep(short_sleep_time);
    q330_register();
    do_sleep(long_sleep_time);
    q330_change_state(LIBSTATE_IDLE);
    do_sleep(short_sleep_time);
    q330_change_state(LIBSTATE_TERM);
    do_sleep(short_sleep_time);
    q330_destroy_context();
}
