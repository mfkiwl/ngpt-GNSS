#! /usr/bin/python

from mpl_toolkits.mplot3d import axes3d
from matplotlib import cm
import matplotlib.pyplot as plt
import numpy as np
import argparse
from argparse import RawTextHelpFormatter
import sys

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
    line = fin.readline()
    l    = line.split()
    if l[0] != 'AZI:':
        print >> sys.stderr, 'ERROR. Invalid \'Azi\' line:'
        print >> sys.stderr, '       [' + line.strip() + ']'
        sys.exit(1)
    azi1, azi2, dazi = map(float, l[1:])
    # return np.arange(azi1, azi2, dazi)
    return azi1, azi2, dazi

parser = argparse.ArgumentParser(
    formatter_class = RawTextHelpFormatter,
    description     = ''
    'Utility to plot GNSS antenna Phase Center Variation correction\n'
    'grid(s), as extracted from the ngpt atxtr program.'
    )

parser.add_argument('-f', '--file',
    action   = 'store',
    required = True,
    help     = ''
    'The input file containing the PCV corrections.\n'
    'The format of this file is strict; see the\n'
    '\"atx.grd.example\" file for details. Note that\n'
    'more than one antennas PCVs can be plotted in one\n'
    'run.',
    metavar  = 'PVC_GRID',
    dest     = 'pcv_grid_file'
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

##  Parse command line arguments
args = parser.parse_args()

FIG_NR = 0 
new_antenna = True
with open(args.pcv_grig_file, 'r') as fin:
    while new_antenna:
        try:
            antenna = get_antenna_model(fin)
            pcv = []
        except:
            break
        FIG_NR += 1
        zen1, zen2, dzen = get_zen_axis(fin)
        azi1, azi2, dazi = get_azi_axis(fin)
        ## how many lines should i read ?? TODO limits are not drawn so NOT:
        #max_lines = int((zen2 - zen1)/dzen + 1) * int((azi2 - azi1)/dazi + 1)
        ## BUT
        max_lines = int((zen2 - zen1)/dzen) * int((azi2 - azi1)/dazi)
        idx       = 0
        while idx < max_lines:
            line = fin.readline()
            l = line.split()
            if len(l) != 1:
                print >> sys.stderr, 'ERROR. Invalid line PCV line'
                print >> sys.stderr, '       [' + line.strip() + ']'
                sys.exit(1)
            pcv.append(float(l[0]))
            idx += 1
            print 'read',idx,'/',max_lines, '(FIG=', FIG_NR,')'

        ##  Setup plot env
        ## -------------------------------------------------------------------
        fig = plt.figure(FIG_NR)
        fig.suptitle(antenna, fontsize=14, fontweight='bold')
        # ax  = fig.add_subplot(111, projection='3d')
        ax = fig.gca(projection='3d')
        if args.plot_altitude:
            ax.set_xlabel('Altitude ($^\circ$)')
        else:
            ax.set_xlabel('Zenith Distance ($^\circ$)')
        ax.set_ylabel('Azimouth ($^\circ$)')
        ax.set_zlabel('Pcv (mm)')

        ##  Make a (const) zero surface plot
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
        Z = np.zeros((len(zen), len(azi)))
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
            cset = ax.contourf(X, Y, Z, zdir='z', offset=2*min(pcv), cmap=cm.coolwarm)
            #cset = ax.contourf(X, Y, Z, zdir='x', offset= -15,         cmap=cm.coolwarm)
            #cset = ax.contourf(X, Y, Z, zdir='y', offset=340,         cmap=cm.coolwarm)

        ##  Re-scale
        ax.set_xlim(0, 90.0)  ## zenith
        ax.set_ylim(0, 360.0) ## azimuth
        ax.set_zlim(2*min(pcv), max(pcv)+1)

plt.show()
