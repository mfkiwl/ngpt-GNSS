### NAME

inxtr - read and translate IONEX files.

### SYNOPSIS

`inxtr -i IONEX [-start [YYYY/MM/DD ]HH:MM:SS] [-stop [YYYY/MM/DD ]HH:MM:SS] [-lat from/to/step] [-lon from/to/step]`

### DESCRIPTION

This program will read IONEX files, interpolate (both spatialy and in time)
and report TEC values for selected regions/grids and time intervals. All 
output is directed to `stdout`.

### OPTIONS


* -h or --help
Display (this) help message and exit.
* -i [IONEX]
Specify the input IONEX file.
* -start [YYYY/MM/DDTHH:MM:SS] or [HH:MM:SS]
Specify the first epoch to interpolate. In case only
a time argument is provided (i.e. "HH:MM:SS") it is
assumed that the day is the (first) day in the
IONEX file. If not prvided, it is set to the first
epoch in the file. Note that the time-scale used within
withi the IONEX files is UT.
* -stop [YYYY/MM/DDTHH:MM:SS] or [HH:MM:SS]
Specify the last epoch to interpolate. In case only
a time argument is provided (i.e. "HH:MM:SS") it is
assumed that the day is the (last) day in the 
IONEX file.If not prvided, it is set to the last
epoch in the file. Note that the time-scale used within
withi the IONEX files is UT.
* -interval [SECONDS]
Specify the time step in integer seconds for the
interpolation. If not provided, it is set to
the value provided in the IONEX header.
* -lat [lat1/lat2/dlat]
The latitude interval for the interpolation. The
arguments should be decimal degrees (floats).
If not provided, it is set to the value provided
in the IONEX file. Note that you can overide the
"dlat" value via the "-dlat" option.
* -lon [lon1/lon2/dlon]
The longtitude interval for the interpolation. The
arguments should be decimal degrees (floats).
If not provided, it is set to the value provided
in the IONEX file. Note that you can overide the
"dlon" value via the "-dlon" option.
* -dlat [LATITUDE STEP]
Specify the latitude step in decimal degrees (the
max precission is two decimal places). This will
override the value of "-lat" argument (if provided).
* -dlon [LONGTITUDE STEP]
Specify the longtitude step in decimal degrees (the
max precission is two decimal places). This will
override the value of "-lon" argument (if provided).

### DIAGNOSTICS

In case you have compiled the `inxtr` in `DEBUG` mode, then
diagnostics could be issued; these are (mostly) lines starting
with the string `[DEBUG]`.

### BUGS

### AUTHOR

Xanthos Papanikolaou xanthos[AT]mail.ntua.gr
Demitris Anastasiou danast[AT]mail.ntua.gr
Vangelis Zacharis vanzach[AT]survey.ntua.gr

