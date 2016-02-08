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
        except (TypeError, ZeroDivisionError), error:
            print("Antenna: {} has no AZImouth information.".format(
                    self.antenna_type))


    def set_PCV(self, data):
        '''Sets phase centre variations.
        Converts a 1D list of PCV values to a 2D numpy.ndarray, with
        dimensions len(azimouth) x len(zenith_distance).
        '''
        self.__max_pcv = max(data)
        self.__min_pcv = min(data)

        try:            # if the antex has azimouth info
            self.pcv = numpy.asarray(data).reshape(
                    len(self.zenith_distance), len(self.azimouth)
                    ).transpose()
        except:         # NOAZI
            self.pcv = numpy.asarray(data)


    def plot_antenna(self,
            kind='surface', output_format=None, plot_altitude=False):
        '''Plots PCV.
        kind is one of 'contour', 'surface' (default), or 'wireframe'.
        output_format should be one of 'png', 'pdf', '[e]ps', 'svg', or
        None (default) for interactive viewing of the plot.
        If plot_altitude is True, plot altitude values instead of
        zenith distance.
        '''
        fig = mpl_pyplot.figure()

        try:                        # if the antex has azimouth info
            z, az = numpy.meshgrid(self.zenith_distance, self.azimouth)

            # setup the plot (title, axes, etc.)
            fig.suptitle(re.sub(r'\s+', ' ', self.antenna_type),
                    fontsize=14, fontweight='bold')
            ax  = fig.gca(projection='3d')
            if plot_altitude:
                ax.set_xlabel('Altitude ($^\circ$)')
            else:
                ax.set_xlabel('Zenith Distance ($^\circ$)')
            ax.set_ylabel('Azimouth ($^\circ$)')
            ax.set_zlabel('PCV (mm)')

            # plot
            if kind == 'contour':
                ax  = fig.gca()
                levels = numpy.arange(self.__min_pcv-1,
                        self.__max_pcv+1)
                norm = matplotlib.cm.colors.Normalize(
                        vmax=abs(self.pcv).max(),
                        vmin=-abs(self.pcv).max())
                cmap = matplotlib.cm.PRGn
                cs = ax.contourf(z, az, self.pcv,
                        levels, norm=norm,
                        cmap=matplotlib.cm.get_cmap(cmap,
                        len(levels)-1))
                mpl_pyplot.colorbar(cs)
                cset = ax.contourf(z, az, self.pcv,
                        zdir='z', offset=2*self.__min_pcv,
                        cmap=matplotlib.cm.coolwarm)
            elif kind == 'surface':
                ax.plot_surface(z, az, self.pcv,
                        rstride=5, cstride=5, alpha=0.3)
            elif kind == 'wireframe':
                ax.plot_wireframe(z, az, self.pcv,
                        rstride=5, cstride=5)

        except TypeError, error:    # NOAZI
            # setup the plot (title, axes, etc.)
            fig.suptitle(" ".join((
                    re.sub(r'\s+', '_', self.antenna_type), "(NOAZI)")),
                    fontsize=14, fontweight='bold')
            ax  = fig.gca()
            if args.plot_altitude:
                ax.set_xlabel('Altitude ($^\circ$)')
            else:
                ax.set_xlabel('Zenith Distance ($^\circ$)')
            ax.set_ylabel('PCV (mm)')

            # plot
            ax.plot(self.zenith_distance, self.pcv,
                    'yo-')

        # save or show the plot
        if output_format is not None:
            figure_filename = ".".join(
                    (re.sub(r'\s+', '_', self.antenna_type),
                    output_format)).lower()
            fig.savefig(figure_filename,
                    dpi=100, bbox_inches='tight',
                    format=output_format)
        else:
            mpl_pyplot.show()

        return 0
