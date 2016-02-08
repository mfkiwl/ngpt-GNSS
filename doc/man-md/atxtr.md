% atxtr
%
% February 2016


# NAME

atxtr - read and translate ANTEX files.

# SYNOPSIS

**atxtr** -a *ANTEX* [**-m** *"ANTENNA_1 ANTENNA2..."*] 

# DESCRIPTION

This program will read, interpolate and report GNSS antenna Phase Center
Variation (PCV) corrections from an ANTEX file. All output is directed
to **stdout**.

# OPTIONS

**-h** **--help**
:   Display (this) help message and exit.

**-a** *ANTEX*
:   Specify the input ANTEX file.

**-m** *"ANTENNA_1,ANTENNA_2,..."*
:   Specify the antenna model. Use quotation marks
    ("") to correctly parse whitespaces. The antenna model must follow the 
    IGS conventions and the ANTEX file format specifications. You can
    specify a serial number, by typing it directly after the RADOME (as in 
    ANTEX); if the serial is not matched, but the antenna (i.e. MODEL+RADOME)
    is, then this generic antnna will be selected.

**-l**, **--list**
:   List all available antennas recorded in the given ANTEX file and exit.

**-dzen** *ZENITH_STEP*
:   Specify the zenith step in degrees. The interpolation will be performed on 
    the interval *[ZEN1, ZEN2]* with a step size of *ZENITH_STEP* degrees. 
    The *ZEN1*, *ZEN2* are read off from the ANTEX file. Default value is *1.0*.

**-dazi** *AZIMOUTH_STEP*
:   If the antenna PCV pattern includes azimouth-dependent
    corrections, then this will set the step size for
    the azimouth interval. The interpolation will be
    performed on the interval *[0, 360]* with a step size of
    *AZIMOUTH_STEP* degrees. Default value is *1.0*.
     
**-azi** *from/to/step*
:   Specify the range for the azimouth axis. Azimouth
    grid will span the interval *[from, to]* with a step 
    size of *step* degrees. Default is *[0/360/1]*. This
    will automatically fall back to *[0/0/0]* if no 
    azimouth-dependent pcv values are available. Note
    that this option will overwrite the **-dazi** option.

**-zen** *from/to/step*
:   Specify the range for the zenith ditance axis. The
    grid will span the interval *[from, to]* with a step
    size of step degrees. Default is *[0/90/1]*. Note
    that this option will overwrite the **-dzen** option.

**-diff**
:   Instead of printing each antenna's pcv corrections,
    print the diffrences between pcv values. The first
    antenna in the list is considered as reference and
    for each antenna in the specified list the respective
    discrepancies are computed.

# EXAMPLE

`atxtr` -a igs08.atx -m **"**TRM41249.00     TZGD,LEIATX1230+GNSS NONE**"**


# DIAGNOSTICS

In case you have compiled the `atxtr` in **DEBUG** mode, then
diagnostics could be issued; these are (mostly) lines starting
with the string **"[DEBUG]"**.

# COPYRIGHT


Copyright (C) **National Technical Univsersity of Athens**, **Dionysos Satellite Observatory**


Everyone is permitted to copy and distribute verbatim or modified
copies of this license document, and changing it is allowed as long
as the name is changed.

           DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
  TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

 0. You just DO WHAT THE FUCK YOU WANT TO.


# AUTHORS

Xanthos Papanikolaou xanthos[AT]mail.ntua.gr, Demitris Anastasiou danast[AT]mail.ntua.gr and 
Vangelis Zacharis vanzach[AT]survey.ntua.gr
