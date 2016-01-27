antex_view.py
-------------

Utility to plot GNSS antenna Phase Center Variation correction grid,
as extracted from the *ngpt* ``atxtr`` program.

By default, ``antex-view.py`` will read data from stdin, but you can
change this behavior via the ``-f`` option.  You can simultaniously plot
multiple antenna PCV grids/corrections.  The plots will be
(interactively) shown when all reading is done; should you want to save
them, use the ``-s`` option (if you do not want to see the interactive
plots a all, use ``-n``).  Normally, you should be able to save files in
all commonly used formats, including: *png*, *pdf*, *[e]ps*, *svg*.  The
format of the input data is strict and even slight changes may cause the
program to exit with failure.  NOAZI PCV corrections, ie. antennas with
no azimouth-dependent PCV corrections can (and will) be plotted if the
``AZI`` line is: ``AZI: 0 0 0``.  An Antenna block ends with the ``EOA``
(End-Of-Antenna) record.  An exit status other than 0 (zero), denotes 
ERROR.

Example input data::

    ANT: LEIAR25.R4      NONE
    ZEN: 0 90 1
    AZI: 0 360 1
    0.003804
    ...
    ...
    ...
    EOA

See ``atxtr`` for more info.

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
