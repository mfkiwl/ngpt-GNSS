% inxtr
%
% February 2016


# NAME

inxtr - read and translate IONEX files.

# SYNOPSIS

**inxtr** -i *IONEX* 
[**-start** *[YYYY/MM/DD ]HH:MM:SS*] 
[**-stop** *[YYYY/MM/DD ]HH:MM:SS*] 
[**-lat** *from/to/step*] 
[**-lon** *from/to/step*]

# DESCRIPTION

This program will read IONEX files, interpolate (both spatialy and in time)
and report TEC values for selected regions/grids and time intervals. All 
output is directed to **stdout**.

# OPTIONS


**-h** **--help**
:   Display (this) help message and exit.

**-i** *IONEX*
:   Specify the input IONEX file.

**-start** *[YYYY/MM/DDT]HH:MM:SS*
:   Specify the first epoch to interpolate. In case only
    a time argument is provided (i.e. *HH:MM:SS*) it is
    assumed that the day is the (first) day in the
    IONEX file. If not prvided, it is set to the first
    epoch in the file. Note that the time-scale used within
    withi the IONEX files is **UT**.

**-stop** *[YYYY/MM/DDT]HH:MM:SS*
:   Specify the last epoch to interpolate. In case only
    a time argument is provided (i.e. *HH:MM:SS*) it is
    assumed that the day is the (last) day in the 
    IONEX file.If not prvided, it is set to the last
    epoch in the file. Note that the time-scale used within
    within the IONEX files is UT.

**-interval** *SECONDS*
:   Specify the time step in integer seconds for the
    interpolation. If not provided, it is set to
    the value provided in the IONEX header.

**-lat** *lat1/lat2/dlat*
:   The latitude interval for the interpolation. The
    arguments should be decimal degrees (floats).
    If not provided, it is set to the value provided
    in the IONEX file. Note that you can overide the
    **-lat** value via the **-dlat** option.    

**-lon** *lon1/lon2/dlon*
:   The longtitude interval for the interpolation. The
    arguments should be decimal degrees (floats).
    If not provided, it is set to the value provided
    in the IONEX file. Note that you can overide the
    **lon** value via the **-dlon** option.

**-dlat** *LATITUDE STEP*
:   Specify the latitude step in decimal degrees (the
    max precission is two decimal places). This will
    override the value of **-lat** argument (if provided).

**-dlon** *LONGTITUDE STEP*
:   Specify the longtitude step in decimal degrees (the
    max precission is two decimal places). This will
    override the value of **-lon** argument (if provided).

**-diff** *["IONEX1 IONEX2 ..."]*
:   If this switch is specified, then instead of writing
    TEC values, the program will output TEC differences
    (in TECU) between the "master" IONEX file (the one
    passed with the **-i** option) and the ones given in the
    **-diff** argument list. If you want to compare more than
    two files, then provide a whitespace-seperated list,
    included within quotation marks ("").


# EXAMPLES


Compute differences in TEC for point (lon=0, lat=0), between the files:
*igsg0440.03i* - *codg0440.03i* and *igsg0440.03i* - *jplg0440.03i*, for all
epochs in the (master) file:


**inxtr** -i igsg0440.03i -diff "codg0440.03i jplg0440.03i" -lat 0/0/0 -lon 0/0/0

# DIAGNOSTICS

In case you have compiled the `inxtr` in **DEBUG** mode, then
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
