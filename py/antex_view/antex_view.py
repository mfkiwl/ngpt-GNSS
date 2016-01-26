#! /usr/bin/env python
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
import argparse
#~ import itertools
#~ import os
#~ import os.path
import re
import sys

# numpy
try:
    import numpy
except ImportError, error:
    print("ImportError: {}".format(error[0]))
    sys.exit(-1)

# matplotlib
try:
    import matplotlib
    #~ matplotlib.use('PS')
    #~ import matplotlib.dates as mpl_dates
    #~ import matplotlib.gridspec as mpl_gridspec
    import matplotlib.pyplot as mpl_pyplot
    import mpl_toolkits.mplot3d as mplot3d
except ImportError, error:
    print("ImportError: {}".format(error[0]))
    sys.exit(-1)



class Antenna():
    '''Antenna object.
    Attributes:
    - antenna_type: antenna type & dome; string
    - zenith_distance: range; numpy.ndarray
    - azimouth: range or None; numpy.ndarray or None
    - pcv: phase centre variation values; numpy.ndarray
    Methods:
    - set_zenith_distance
    - set_azimouth
    - set_PCV
    - plot_antenna
    '''
    def __init__(self, ant):
        '''Constructor.
        'ant' (string) holds th antenna type & dome.
        '''
        self.antenna_type = ant
        self.zenith_distance = None
        self.azimouth= None
        self.pcv = None
        # private attributes
        self.__max_pcv = 0.
        self.__min_pcv = 0.


    def set_zenith_distance(self, z):
        '''Sets zenith distance.
        Converts 3-tuple of start, end, increment to a 1D numpy.ndarray
        of data-points.
        '''
        self.zenith_distance = numpy.arange(*z)


    def set_azimouth(self, az):
        '''Sets azimouth.
        Converts 3-tuple of start, end, increment to a 1D numpy.ndarray
        of data-points.
        '''
        try:
            self.azimouth = numpy.arange(*az)
        except TypeError, error:
            print("Antenna: {} has no AZImouth information.".format(
                    self.antenna_type))


    def set_PCV(self, data):
        '''Sets phase centre variations.
        Converts a 1D list of PCV values to a 2D numpy.ndarray, with
        dimensions len(azimouth) x len(zenith_distance).
        '''
        self.__max_pcv = max(data)
        self.__min_pcv = min(data)
        print self.__max_pcv, self.__min_pcv
        self.pcv = numpy.asarray(data
                ).reshape(len(self.zenith_distance), len(self.azimouth)
                ).transpose()


    def plot_antenna(self, kind):
        '''Plots PCV.
        kind is one of 'wireframe', 'surface', or 'contour'.
        '''
        z, az = numpy.meshgrid(self.zenith_distance, self.azimouth)

        fig = mpl_pyplot.figure()
        fig.suptitle(re.sub(r'\s+', ' ', self.antenna_type),
                fontsize=14, fontweight='bold')
        ax  = fig.gca(projection='3d')
        ax.set_zlabel('PCV (mm)')

        if kind == 'wireframe':
            ax.plot_wireframe(z, az, self.pcv,
                    rstride=5, cstride=5)
        elif kind == 'surface':
            ax.plot_surface(z, az, self.pcv,
                    rstride=5, cstride=5, alpha=0.3)
        elif kind == 'contour':
            ax  = fig.gca()
            levels = numpy.arange(self.__min_pcv-1, self.__max_pcv+1)
            norm = matplotlib.cm.colors.Normalize(
                    vmax=abs(self.pcv).max(), vmin=-abs(self.pcv).max())
            cmap = matplotlib.cm.PRGn
            cs = ax.contourf(z, az, self.pcv,
                    levels, norm=norm,
                    cmap=matplotlib.cm.get_cmap(cmap, len(levels)-1))
            mpl_pyplot.colorbar(cs)

            cset = ax.contourf(z, az, self.pcv,
                    zdir='z', offset=2*self.__min_pcv,
                    cmap=matplotlib.cm.coolwarm)

        mpl_pyplot.show()



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
            return Antenna(rec[5:])
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



stream = open('sample_atx.grd')
antennae = parse_input(stream)
for a in antennae:
    #~ print type(a)
    print a.antenna_type
    a.plot_antenna('wireframe')
    a.plot_antenna('surface')
    a.plot_antenna('contour')
    #~ print len(a.zenith_distance), len(a.azimouth)
    #~ print type(a.pcv)

