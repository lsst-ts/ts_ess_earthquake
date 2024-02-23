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

__all__ = [
    "TInit",
    "TMiniseed",
    "TMsg",
    "TOnesec",
    "TState",
    "OSF",
    "TLibState",
    "TMiniseedAction",
    "TPacketClass",
    "TStateType",
]

import ctypes
import enum


class TInit(ctypes.Structure):
    """TInit struct.

    Parameters
    ----------
    serial_id : `ctypes.c_uint64`
        The serial ID of the Q330 earthquake sensor; a 64 bit unsigned integer.
    hostname : `ctypes.c_char` * 250
        The hostname; a string of length up to 250 characters.
    baseport : `ctypes.c_uint16`
        The baseport; a 16 bit unsigned integer.
    debug : `ctypes.c_bool`
        Enable debugging in the library?
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

    _fields_ = [
        ("serial_id", ctypes.c_uint64),
        ("hostname", ctypes.c_char * 250),
        ("baseport", ctypes.c_uint16),
        ("debug", ctypes.c_bool),
        ("aminiseed_callback", ctypes.CFUNCTYPE(ctypes.c_void_p)),
        ("message_callback", ctypes.CFUNCTYPE(ctypes.c_void_p)),
        ("miniseed_callback", ctypes.CFUNCTYPE(ctypes.c_void_p)),
        ("secdata_callback", ctypes.CFUNCTYPE(ctypes.c_void_p)),
        ("state_callback", ctypes.CFUNCTYPE(ctypes.c_void_p)),
    ]


class TMiniseed(ctypes.Structure):
    """TMiniseed struct.

    Parameters
    ----------
    station_name : `ctypes.c_char_p` * 9
        The name of the sensor; a string of length up to 9 characters.
    location : `ctypes.c_char_p` * 2
        The location of the sensor; a string of length up to 2 characters.
    chan_number : `ctypes.c_byte`
        The channel number; a byte.
    channel : `ctypes.c_char_p` * 3
        The channel; a string of length of always 3 characters.
    rate : `ctypes.c_uint32`
        The sampling rate in samples per second if positive or seconds per
        sample if negative; a 32 bit unsigned integer.
    cl_session : `ctypes.c_int32`
        The closed loop session number; a 32 bit unsigned integer.
    cl_offset : `ctypes.c_double`
        The closed loop time offset; a 32 bit unsigned integer.
    timestamp : `ctypes.c_double`
        The timestamp [sec since 1 January 2000]; a 32 bit unsigned integer.
    filter_bits : `ctypes.c_uint16`
        Bitmask of `OMF` enum values; a 16 bit unsigned integer.
    packet_class : `ctypes.c_int`
        The type of record; a `TPacketClass` enum represented by a signed
        integer.
    miniseed_action : `ctypes.c_int`
        What this packet represents; a `TMiniseedAction` enum represented by a
        signed integer.
    data_size : `ctypes.c_unit16`
        The size of actual miniseed data; a 16 bit unsinged integer.
    data_address : `ctypes.c_pointer`
        A pointer to miniseed record; a byte array of size `data_size`.

    Notes
    -----
    The data in the miniseed record is binary data of size `data_size`.
    """

    _fields_ = [
        ("station_name", ctypes.c_char_p * 9),
        ("location", ctypes.c_char_p * 2),
        ("chan_number", ctypes.c_byte),
        ("channel", ctypes.c_char_p * 3),
        ("rate", ctypes.c_uint32),
        ("cl_session", ctypes.c_int32),
        ("cl_offset", ctypes.c_double),
        ("timestamp", ctypes.c_double),
        ("filter_bits", ctypes.c_uint16),
        ("packet_class", ctypes.c_int),
        ("miniseed_action", ctypes.c_int),
        ("data_size", ctypes.c_uint16),
        ("data_address", ctypes.c_void_p),
    ]


class TMsg(ctypes.Structure):
    """TMsg struct.

    Parameters
    ----------
    msgcount : `ctypes.c_uint32`
        The message count; a 32 bit unsigned integer.
    code : `ctypes.c_int16`
        The message code; a 16 bit signed integer.
    timestamp : `ctypes.c_uint32`
        The timestamp [sec since 1 January 2000]; a 32 bit unsigned integer.
    datatime : `ctypes.c_uint32`
        The data timestamp [sec since 1 January 2000]; a 32 bit unsigned
        integer.
    result : `ctypes.c_char_p`
        The string representation of the code; a pointer to a char.
    suffix : `ctypes.c_char_p`
        The suffix of the message, if any; a pointer to a char.

    Notes
    -----
    The `result` and the `suffix` together form the message sent by the sensor.
    """

    _fields_ = [
        ("msgcount", ctypes.c_uint32),
        ("code", ctypes.c_int16),
        ("timestamp", ctypes.c_uint32),
        ("datatime", ctypes.c_uint32),
        ("result", ctypes.c_char_p),
        ("suffix", ctypes.c_char_p),
    ]


class TOnesec(ctypes.Structure):
    """TOnesec struct.

    Parameters
    ----------
    total_size : `ctypes.c_uint32`
        The total size of the structure that is used; a 32 bit unsigned
        integer. This is unused and will probably be dropped from the struct.
    station_name : `ctypes.c_char_p` * 9
        The name of the sensor; a string of length up to 9 characters.
    location : `ctypes.c_char_p` * 2
        The location of the sensor; a string of length up to 2 characters.
    chan_number : `ctypes.c_byte`
        The channel number; a byte.
    channel : `ctypes.c_char_p` * 3
        The channel; a string of length of always 3 characters.
    padding : `ctypes.c_uint16`
        The padding; a 32 bit unsigned integer.
    rate : `ctypes.c_uint32`
        The sampling rate in samples per second if positive or seconds per
        sample if negative; a 32 bit unsigned integer.
    cl_session : `ctypes.c_int32`
        The closed loop session number; a 32 bit unsigned integer.
    reserved : `ctypes.c_uint32`
        Always zero; a 32 bit unsigned integer.
    cl_offset : `ctypes.c_double`
        The closed loop time offset; a 32 bit unsigned integer.
    timestamp : `ctypes.c_double`
        The timestamp [sec since 1 January 2000]; a 32 bit unsigned integer.
    filter_bits : `ctypes.c_uint16`
        Bitmask of `OSF` enum values; a 16 bit unsigned integer.
    qual_perc : `ctypes.c_uint16`
        The clock quality percentage; a 16 bit unsigned integer.
    activity_flags : `ctypes.c_uint16`
        Activity flag bitmask; a 16 bit unsigned integer.
    io_flags : `ctypes.c_uint16`
        IO flag bitmask; a 16 bit unsigned integer.
    data_quality_flags : `ctypes.c_uint16`
        Data quality flag bitmask; a 16 bit unsigned integer.
    src_channel : `ctypes.c_byte`
        The source blockette channel; a 32 bit unsigned integer.
    src_subchan : `ctypes.c_byte`
        The source blockette sub-channel; a 32 bit unsigned integer.
    samples : `ctypes.c_int32` * 1000
        Aan array of decompressed samples with the number of samples equal to
        `rate` if rate is positive or one sample for sub-Hz sampling rates; an
        array of up to 1000 32 bit signed integers.
    """

    _fields_ = [
        ("total_size", ctypes.c_uint32),
        ("station_name", ctypes.c_char_p * 9),
        ("location", ctypes.c_char_p * 2),
        ("chan_number", ctypes.c_byte),
        ("channel", ctypes.c_char_p * 3),
        ("padding", ctypes.c_uint16),
        ("rate", ctypes.c_uint32),
        ("cl_session", ctypes.c_int32),
        ("reserved", ctypes.c_uint32),
        ("cl_offset", ctypes.c_double),
        ("timestamp", ctypes.c_double),
        ("filter_bits", ctypes.c_uint16),
        ("qual_perc", ctypes.c_uint16),
        ("activity_flags", ctypes.c_uint16),
        ("io_flags", ctypes.c_uint16),
        ("data_quality_flags", ctypes.c_uint16),
        ("src_channel", ctypes.c_byte),
        ("src_subchan", ctypes.c_byte),
        ("samples", ctypes.c_int32 * 1000),
    ]


class TState(ctypes.Structure):
    """TState struct.

    Parameters
    ----------
    state_type : `ctypes.c_int`
        The state type; a `TStateType` enum represented by a nsigned integer.
    station_name : `ctypes.c_char_p`
        The name of the sensor; a pointer to a char.
    subtype : `ctypes.c_uint32`
        The state subtype; a 32 bit integer.
    info : `ctypes.c_uint32`
        The state info; a 32 bit integer.
    state_name : `ctypes.c_char_p`
        The state name; a pointer to a char.

    Notes
    -----
    The values of subtype and info have specific meanings for specific state
    types.
    """

    _fields_ = [
        ("state_type", ctypes.c_int),
        ("station_name", ctypes.c_char_p),
        ("subtype", ctypes.c_uint32),
        ("info", ctypes.c_uint32),
        ("state_name", ctypes.c_char_p),
    ]


class OSF(enum.Enum):
    """One second filtering bit masks."""

    OSF_ALL = 1  # bit set to send all one second data
    OSF_DATASERV = 2  # bit set to send dataserv lcqs
    OSF_1HZ = 4  # bit set to send 1hz main digitizer data
    OSF_EP = 8  # bit set to send 1hz Environmental Processor data


class TLibState(enum.Enum):
    """Enum representing the tlibstate C enum.

    Notes
    -----
    The values need to be explicitly defined since C enum values start at 0 and
    Python enum.auto() starts at 1.
    """

    LIBSTATE_IDLE = 0  # Not connected to Q330
    LIBSTATE_TERM = 1  # Terminated
    LIBSTATE_PING = 2  # Un-registered Ping, returns to LIBSTATE_IDLE when done
    LIBSTATE_CONN = 3  # TCP Connect wait
    LIBSTATE_ANNC = 4  # Announce baler
    LIBSTATE_REG = 5  # Requesting Registration
    LIBSTATE_READCFG = 6  # Reading Configuration
    LIBSTATE_READTOK = 7  # Reading Tokens
    LIBSTATE_DECTOK = 8  # Decoding Tokens and allocating structures
    LIBSTATE_RUNWAIT = 9  # Reading Tokens
    LIBSTATE_RUN = 10  # Decoding Tokens and allocating structures
    LIBSTATE_DEALLOC = 11  # De-allocating structures
    LIBSTATE_DEREG = 12  # De-registering
    LIBSTATE_WAIT = 13  # Waiting for a new registration


class TMiniseedAction(enum.Enum):
    """Enum representing the tminiseed_action C enum.

    Notes
    -----
    The values need to be explicitly defined since C enum values start at 0 and
    Python enum.auto() starts at 1.
    """

    MSA_512 = 0  # new 512 byte packet, for miniseed only
    MSA_ARC = 1  # new archival packet, non-incremental, for aminiseed only
    MSA_FIRST = 2  # new archival packet, incremental, for aminiseed only
    MSA_INC = 3  # incremental update to archival packet, for aminiseed only
    MSA_FINAL = 4  # final incremental update, for aminiseed only
    MSA_GETARC = 5  # request for last archival packet written, for aminiseed
    #                 only
    MSA_RETARC = 6  # client is returning last packet written, for aminiseed
    #                 only


class TPacketClass(enum.Enum):
    """Enum representing the tpacket_class C enum.

    Notes
    -----
    The values need to be explicitly defined since C enum values start at 0 and
    Python enum.auto() starts at 1.
    """

    PKC_DATA = 0
    PKC_EVENT = 1
    PKC_CALIBRATE = 2
    PKC_TIMING = 3
    PKC_MESSAGE = 4
    PKC_OPAQUE = 5


class TStateType(enum.Enum):
    """Enum representing the tstyate_type C enum.

    Notes
    -----
    The values need to be explicitly defined since C enum values start at 0 and
    Python enum.auto() starts at 1.
    """

    ST_STATE = 0  # new operational state
    ST_STATUS = 1  # new status available
    ST_CFG = 2  # new configuration available
    ST_STALL = 3  # change in stalled comlink state
    ST_PING = 4  # subtype has ping type
    ST_TICK = 5  # info has seconds, subtype has usecs
    ST_OPSTAT = 6  # new operational status minute
    ST_TUNNEL = 7  # tunnel response available
