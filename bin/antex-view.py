#! /usr/bin/python

import matplotlib.pyplot as plt
import numpy as np
import argparse
import sys
from mpl_toolkits.mplot3d import axes3d
from matplotlib import cm
from argparse import RawTextHelpFormatter

plot_options = {'wired': False,  ## plot as wire-frame
                'surfc': True }  ## normal, surface plot

def get_antenna_model(buf):
    """ Extract the antenna model from an input stream.
    """
    line = buf.readline()
    print 'ANTENNA LINE=[' + line.strip() + ']'
    l    = line.split()
    if l[0] != 'ANT:':
        print >> sys.stderr, 'ERROR. Invalid \'Ant\' line:'
        print >> sys.stderr, '       [' + line.strip() + ']'
        sys.exit(1)
    return line[5:]

def get_zen_axis(buf):
    """ Extract the zenith distance grid details from an input stream.
    """
    line = buf.readline()
    l    = line.split()
    if l[0] != 'ZEN:':
        print >> sys.stderr, 'ERROR. Invalid \'Zen\' line:'
        print >> sys.stderr, '       [' + line.strip() + ']'
        sys.exit(1)
    zen1, zen2, dzen = map(float, l[1:])
    # return np.arange(zen1, zen2, dzen)
    return zen1, zen2, dzen

def get_azi_axis(buf):
    """ Extract the azimouth grid details from an input stream.
    """
    line = buf.readline()
    l    = line.split()
    if l[0] != 'AZI:':
        print >> sys.stderr, 'ERROR. Invalid \'Azi\' line:'
        print >> sys.stderr, '       [' + line.strip() + ']'
        sys.exit(1)
    azi1, azi2, dazi = map(float, l[1:])
    # return np.arange(azi1, azi2, dazi)
    return azi1, azi2, dazi

def make_2d_plot(antenna, buf, fig_nr, zen1, zen2, dzen):
    pcv = []
    max_lines = int((zen2 - zen1)/dzen)
    idx       = 0
    while idx < max_lines:
        line = buf.readline()
        l    = line.split()
        if len(l) != 1:
            print >> sys.stderr, 'ERROR. Invalid line PCV line'
            print >> sys.stderr, '       [' + line.strip() + ']'
            sys.exit(1)
        pcv.append(float(l[0]))
        idx += 1
        ##print 'read',idx,'/',max_lines, '(FIG=', fig_nr,')'
    ##  Setup plot env
    ## -------------------------------------------------------------------
    fig = plt.figure(fig_nr)
    fig.suptitle(antenna + ' (NOAZI)', fontsize=14, fontweight='bold')
    ax  = fig.gca()
    if args.plot_altitude:
        ax.set_xlabel('Altitude ($^\circ$)')
    else:
        ax.set_xlabel('Zenith Distance ($^\circ$)')
    ax.set_ylabel('Pcv (mm)') 
    X = np.arange(zen1, zen2, dzen)
    Y = np.array(pcv)
    ax.set_xlim(zen1-1, zen2+1)  ## zenith
    ax.set_ylim(min(pcv)-1,max(pcv)+1) ## pcv
    ax.plot(X, Y, 'yo-')

    ## Export (if needed)
    ## -------------------------------------------------------------------
    if args.save_file is not None:
        fig.savefig(args.save_file, dpi=100, bbox_inches='tight')
    return fig


def make_3d_plot(antenna, buf, fig_nr):
    ## Read limits and pcv
    ## -----------------------------------------------------------------------
    pcv = []
    zen1, zen2, dzen = get_zen_axis(buf)
    azi1, azi2, dazi = get_azi_axis(buf)
    if azi1 == azi2 or dazi == 0:
        return make_2d_plot(antenna, buf, fig_nr, zen1, zen2, dzen)
    ## how many lines should i read ?? TODO limits are not drawn so NOT:
    #max_lines = int((zen2 - zen1)/dzen + 1) * int((azi2 - azi1)/dazi + 1)
    ## BUT
    max_lines = int((zen2 - zen1)/dzen) * int((azi2 - azi1)/dazi)
    idx       = 0
    while idx < max_lines:
        line = buf.readline()
        l    = line.split()
        if len(l) != 1:
            print >> sys.stderr, 'ERROR. Invalid line PCV line'
            print >> sys.stderr, '       [' + line.strip() + ']'
            sys.exit(1)
        pcv.append(float(l[0]))
        idx += 1
        ##print 'read',idx,'/',max_lines, '(FIG=', fig_nr,')'
    ##  Setup plot env
    ## -------------------------------------------------------------------
    fig = plt.figure(fig_nr)
    fig.suptitle(antenna, fontsize=14, fontweight='bold')
    ax  = fig.gca(projection='3d')
    if args.plot_altitude:
        ax.set_xlabel('Altitude ($^\circ$)')
    else:
        ax.set_xlabel('Zenith Distance ($^\circ$)')
    ax.set_ylabel('Azimouth ($^\circ$)')
    ax.set_zlabel('Pcv (mm)') 
    ##  Make a (const) zero surface plot
    ##  This does not look good ....
    ##  ------------------------------------------------------------------
    if plot_options['wired'] == True:
        X, Y = np.meshgrid(np.arange(zen1-2*dzen, zen2+2*dzen, dzen),
            np.arange(azi1-2*dazi, azi2+2*dazi, dazi) )
        Z    = np.zeros(shape=X.shape)
        surf = ax.plot_surface(X, Y, Z,
            rstride=5,
            cstride=5,
            cmap=cm.autumn,
            linewidth=0,
            antialiased=False)
    ##  Plot the pcv values
    ##  ------------------------------------------------------------------
    zen  = np.arange(zen1, zen2, dzen)
    azi  = np.arange(azi1, azi2, dazi)
    X, Y = np.meshgrid(zen, azi)
    Z    = np.zeros((len(zen), len(azi)))
    k = 0
    for i in range(0, len(zen)):
        for j in range(0, len(azi)):
            Z[i,j] = pcv[k]
            k += 1
    Z = np.transpose(Z)
    ## Wired-plot
    if plot_options['wired'] == True:
        ax.plot_wireframe(X, Y, Z, rstride=5, cstride=5)
    if plot_options['surfc'] == True:
        ax.plot_surface(X, Y, Z, rstride=5, cstride=5, alpha=0.3)
    ## projections/contours
    cset = ax.contourf(X, Y, Z, zdir='z', offset=2*min(pcv), cmap=cm.coolwarm)
    #cset = ax.contourf(X, Y, Z, zdir='x', offset= -15,         cmap=cm.coolwarm)
    #cset = ax.contourf(X, Y, Z, zdir='y', offset=340,         cmap=cm.coolwarm)
    ##  Re-scale
    ## -----------------------------------------------------------------------
    ax.set_xlim(0, zen2)  ## zenith
    ax.set_ylim(0, azi2) ## azimuth
    ax.set_zlim(2*min(pcv), max(pcv)+1)

    ## Export (if needed)
    ## -------------------------------------------------------------------
    if args.save_file is not None:
        fig.savefig(args.save_file, dpi=100, bbox_inches='tight')
    return fig


parser = argparse.ArgumentParser(
    formatter_class = RawTextHelpFormatter,
    description     = ''
    'Utility to plot GNSS antenna Phase Center Variation correction\n'
    'grid(s), as extracted from the ngpt atxtr program.'
    )

parser.add_argument('-f', '--file',
    action   = 'store',
    required = False,
    help     = ''
    'The input file containing the PCV corrections.\n'
    'The format of this file is strict; see the\n'
    '\"atx.grd.example\" file for details. Note that\n'
    'more than one antennas PCVs can be plotted in one\n'
    'run. By default, antex-view will read from stdin.',
    metavar  = 'PVC_GRID',
    dest     = 'pcv_grid_file',
    default  = sys.stdin
    )

## TODO implement this
parser.add_argument('-i', '--altitude',
    action = 'store_true',
    help   = ''
    'Instead of plotting the zenith distance (versus\n'
    'azimouth), plot the altitude. Note that the zenith\n'
    'distance is the complement of altitude (i.e.\n'
    '90 - altitude).',
    dest   = 'plot_altitude'
    )

parser.add_argument('-s', '--save-as',
    action   = 'store',
    default  = None,
    help     = ''
    'Export the plot to an output file. Depending on\n'
    'the filename extension, the format of the file will\n'
    'be deduced. Supported format (normally) include:\n'
    'png, pdf, [e]ps, svg.',
    required = False,
    metavar  = 'SAVE_AS',
    dest     = 'save_file'
    )

parser.add_argument('-n', '--non-interactive',
    action   = 'store_true',
    help     = ''
    'Do not show (supress) the plot; this can be useful\n'
    'if the user only wants the plot to be saved.',
    dest     = 'no_int_plot'
    )

##  Parse command line arguments
args = parser.parse_args()

FIG_NR      = 0
plots       = []
new_antenna = True

with open(args.pcv_grid_file, 'r') if args.pcv_grid_file is not sys.stdin else sys.stdin as fin:
    while new_antenna:
        try:
            antenna = get_antenna_model(fin)
            FIG_NR += 1
            plots.append( make_3d_plot(antenna, fin, FIG_NR) )
            print 'Ploted antenna:', antenna
        except:
            break
            new_antenna = False

if not args.no_int_plot:
    for p in plots: plt.show()
