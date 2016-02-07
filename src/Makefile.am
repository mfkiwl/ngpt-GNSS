lib_LTLIBRARIES = libngpt.la

## Only use the following flags for debuging purposes
libngpt_la_CXXFLAGS = \
	-std=c++14 \
	-g \
	-pg \
	-Wall \
	-Wextra \
	-Werror \
	-pedantic \
	-W \
	-Wshadow \
	-Winline \
	-Wdisabled-optimization \
	-DDEBUG

dist_include_HEADERS = \
	receiver.hpp \
	antenna.hpp \
	datetime_v2.hpp \
	satsys.hpp \
	obstype.hpp \
	grid.hpp \
	antpcv.hpp \
	antex.hpp \
	ionex.hpp \
	geodesy.hpp \
	geoconst.hpp \
	car2ell.hpp \
	car2top.hpp \
	ell2car.hpp \
	ellipsoid.hpp

dist_libngpt_la_SOURCES = \
	dtalg.cpp \
	obstype.cpp \
	receiver.cpp \
	antenna.cpp \
	satsys.cpp \
	antex.cpp \
	ionex.cpp \
	top2daz.cpp
