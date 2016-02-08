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
import sys
import textwrap

# ngpt
import ngpt
from ngpt import antex



def antex(arguments):
    '''Function to deal with 'antex' subcommand.
    '''
    with \
        open(arguments.pcv_grid_file, 'r') \
            if arguments.pcv_grid_file is not sys.stdin \
            else sys.stdin \
        as input_stream:
        antennae = antex.parse_antenna_stream.parse_input(
                 input_stream)

    for ant in antennae:
        ant.plot_antenna(kind=arguments.plot_style,
                output_format=arguments.save_format,
                plot_altitude=False)

    return 0


def ionex(arguments):
    '''Function to deal with 'ionex' subcommand.
    '''
    pass


def main():
    ''' Drive the script.
    '''
    # prepare the command-line argument parser
    parser = argparse.ArgumentParser(
            description = u"""ngpt.py
                    Script to manage output of 'ngpt' C++ tools.

                    'ngpt.py' drives the module indicated by its first
                    positional argument.  The arguments and options that
                    follow, affect the module.

                    Currently, the only supported module is 'antex',
                    which plots GNSS antenna Phase Center Variation
                    correction grid, as extracted from the 'atxtr'
                    program.""",

    #~ By default, 'antex-view.py' will read data from stdin, but you can
    #~ change this behavior via the '-f' option.  You can simultaniously
    #~ plot multiple antenna pcv grids/corrections.  The plots will be
    #~ (interactively) shown when all reading is done; should you want to
    #~ save them, use the '-s' option (if you do not want to see the
    #~ interactive plots a all, use '-n').  Normally, you should be able to
    #~ save files in all commonly used formats, including: 'png', 'pdf',
    #~ '[e]ps', 'svg'.  The format of the input data is strict and even
    #~ slight changes may cause the program to exit with failure.  NOAZI
    #~ PCV corrections, ie. antennas with non-azimouth-dependent PCV
    #~ corrections can (and will) be plotted if the 'AZI' line is:
    #~ 'AZI: 0 0 0'.  An exit status other than 0 (zero), denotes ERROR.

    #~ --------- example input data ---------------------
    #~ ANT: LEIAR25.R4      NONE
    #~ ZEN: 0 90 1
    #~ AZI: 0 360 1
    #~ 0.003804
    #~ ...
    #~ ...
    #~ ...
    #~ --------- end of input data ---------------------

    #~ See 'atxtr' for more info.
            epilog = textwrap.dedent("""Copyright 2015,
            National Technical University of  Athens.

                    This work is free.
                    You can redistribute it and/or modify it under the
                    terms of the Do What The Fuck You Want To Public
                    License, version 2, as published by Sam Hocevar.\n
                    See http://www.wtfpl.net/ for more details.\n\n
                    Send bugs to:\n
                        \txanthos[AT]mail.ntua.gr,\n
                        \tdanast[AT]mail.ntua.gr,\n
                        \tvanzach[AT]mail.ntua.gr."""),
            formatter_class=argparse.RawTextHelpFormatter)

    subparsers = parser.add_subparsers()

    antex_parser = subparsers.add_parser('antex')

    antex_parser.add_argument('-f', '--file',
        action   = 'store',
        required = False,
        help = u"""The input file containing the PCV corrections.
    The format of this file is strict;
    see the 'atx.grd.example' file for details.
    Note that more than one antennas PCVs can
    be plotted in one run.
    By default, antex-view.py will read from stdin.""",
        metavar = 'PVC_GRID',
        dest = 'pcv_grid_file',
        default = sys.stdin)

    #~ parser.add_argument('--debug',
        #~ action = 'store_true',
        #~ help = u"""Enable debug mode.""",
        #~ dest = 'debug_mode')

    #~ parser.add_argument('-i', '--altitude',
        #~ action = 'store_true',
        #~ help = u"""Instead of plotting the zenith distance (versus
    #~ azimouth), plot the altitude.
    #~ Note that the zenith distance is the complement of
    #~ altitude (ie. 90° - altitude).""",
        #~ dest = 'plot_altitude')

    antex_parser.add_argument('-s', '--save-as',
        action = 'store',
        required = False,
        help = u"""Export the plot(s) to an output file.
    Only specify the format (ie. extension) of the
    exported file; the name will be automatically
    deduced using the antenna name/radome/serial,
    where all whitespace will be truncated to '_'.
    Supported formats (normally) include:
    png, pdf, [e]ps, svg.""",
        nargs    = '?',
        metavar  = 'SAVE_FORMAT',
        dest     = 'save_format',
        choices  = ['png', 'pdf', 'eps', 'ps', 'svg'],
        default = None)

    antex_parser.add_argument('-t', '--plot-style',
        action = 'store',
        required = False,
        help = u"""Choose the plot style.
    You can choose between:
        - 'surface': compile a 3D plot (default),
        - 'wireframe': compile a 3D wireframe plot,
        - 'contour': compile a 2D contour plot.
    Note that this is only relevant in
    azimouth-dependent PCV grids.""",
        metavar = 'PLOT_STYLE',
        dest = 'plot_style',
        choices = ['surface', 'wireframe', 'contour'],
        default = 'surface')

    antex_parser.add_argument('-n', '--non-interactive',
        action = 'store_true',
        help = u"""Do not show (supress) the plot(s).
    This can be useful if the user only wants the
    plot(s) to be saved.""",
        dest = 'no_interactive_plot')

    antex_parser.set_defaults(func=antex)

    ionex_parser = subparsers.add_parser('ionex')


    #  Parse command line arguments
    arguments = parser.parse_args()

    arguments.func(arguments)

    return 0


if __name__ == '__main__':
    main()
