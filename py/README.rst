=======
ngpt.py
=======

Script to manage output of ``ngpt`` C++ tools.

Usage::

    ngpt.py module [module-options]

``ngpt.py`` drives the module indicated by its first positional
argument.  The arguments and options that follow, affect the module.

Currently, the only supported module is ``antex``, which plots GNSS
antenna Phase Center Variation correction grid, as extracted from the
*ngpt* ``atxtr`` program.


antex
-----

``ngpt.py antex`` will read the output of ``atxtr`` from stdin, or a
file (via the ``-f`` option), and will plot antenna PCV
grids/corrections, for every antenna present in the input data.

The plot(s) will be (interactively) shown when all reading is done,
unless the ``-s`` option is used, in which case the plot(s) will be
saved in the specified format.  Normally, you should be able to save
files in all commonly used formats, including: *png*, *pdf*, *[e]ps*,
*svg*.

The format of the input data is strict and even slight changes may cause
the program to exit with failure.  Example input data::

    ANT: AOAD/M_B        NONE
    ZEN: 0 90 1
    AZI: 0 360 1
    -0.0054
    ...
    ...
    ...
    EOA
    ANT: LEIAR25.R4      NONE
    ZEN: 0 90 1
    AZI: 0 360 1
    0.003804
    ...
    ...
    ...
    EOA

An Antenna block ends with the ``EOA`` (End-Of-Antenna) record.  NOAZI
PCV corrections, ie. antennas with no azimouth-dependent PCV corrections
can be plotted if the ``AZI`` line is: ``AZI: 0 0 0``.

An exit status other than 0 (zero), denotes ERROR.

See ``atxtr`` for more info.


ionex
-----

``ngpt.py ionex`` is not yet written.


---------

Copyright 2015, National Technical University of Athens.

This work is free.

You can redistribute it and/or modify it under the terms of the *Do What
The Fuck You Want To* Public License, version 2, as published by Sam
Hocevar.

See http://www.wtfpl.net/ for more details.

Send bugs to:

    - xanthos[AT]mail.ntua.gr,
    - danast[AT]mail.ntua.gr,
    - vanzach[AT]mail.ntua.gr
