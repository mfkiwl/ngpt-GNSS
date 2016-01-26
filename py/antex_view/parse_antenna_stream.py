# -*- coding: utf-8 -*-
#
# Copyright 2015, National Technical University of Athens.
#
# This work is free.
# You can redistribute it and/or modify it under the terms of the
#   "Do What The Fuck You Want To Public License", version 2,
#   as published by Sam Hocevar.
# See http://www.wtfpl.net/ for more details.
#
# Send bugs to:
#       xanthos[AT]mail.ntua.gr,
#       danast[AT]mail.ntua.gr,
#       vanzach[AT]mail.ntua.gr



# modules of the standard library
import sys

# antex_view
import antex_view



def parse_input(stream):
    '''Parse input file or stdin.
    Return list of Antenna objects.
    '''
    antennae = []                   # list to hold multiple antennae
    pcv = []                        # list to hold PCV values

    for rec in stream.readlines():
        kind = rec[:-1].split()[0]
        # depending on the type of the record ('kind'),
        #   call the appropriate function;
        #   if 'kind' is not a known string, assume this is a
        #   data record, and proceed accordingly
        if kind == '[DEBUG]':
            pass
        elif kind == 'ANT:':
            antenna = parse_antenna_record(rec[:-1])
        elif kind == 'ZEN:':
            z = parse_size_record(rec[:-1])
            antenna.set_zenith_distance(z)
        elif kind == 'AZI:':
            az = parse_size_record(rec[:-1])
            antenna.set_azimouth(az)
        elif kind == 'EOA':
            antenna.set_PCV(pcv)        # update pcv attribute
            pcv = []                    # initialize pcv list
            antennae.append(antenna)
        else:
            pcv.append(parse_data_record(rec[:-1]))

    return antennae


def parse_antenna_record(rec):
    '''Parse 'ANT:' records.
    Return Antenna object.
    '''
    try:
        if rec.split()[0] != 'ANT:':
            raise ValueError("Not an ANTENNA specification line!")
        else:
            return antex_view.antenna.Antenna(rec[5:])
    except ValueError, error:
        print("ValueError: {}".format(error[0]))
        sys.exit(-2)


def parse_size_record(rec):
    '''Parse the grid details for azimouth or zenith distance.
    Return a 3-tuple of floats.
    Aplies to 'ZEN:' and 'AZI:' records.
    '''
    kind = rec.split()[0]
    try:
        if kind not in ['ZEN:', 'AZI:']:
            raise ValueError("".join(("Invalid specification line for ",
                    kind, "!")))
        else:
            return map(float, rec.split()[1:])
    except ValueError, error:
        print("ValueError: {}".format(error[0]))
        sys.exit(-2)


def parse_data_record(rec):
    '''Parse a PCV value.
    '''
    try:
        return float(rec)
    except ValueError, error:
        print("ValueError: {}".format(error[0]))
        sys.exit(-2)
