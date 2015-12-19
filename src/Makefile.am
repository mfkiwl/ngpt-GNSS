lib_LIBRARIES = libngpt.a

## Only use the following flags for debuging purposes
libngpt_a_CXXFLAGS = \
	-std=c++14 \
	-g \
	-Wall \
	-Wextra \
	-Werror \
	-pedantic \
	-W \
	-Wshadow \
	-Winline \
	-Wdisabled-optimization \
	-DDEBUG

dist_include_HEADERS   = \
	receiver.hpp \
	antenna.hpp \
	datetime.hpp \
	satsys.hpp \
	obstype.hpp \
	grid.hpp \
	antpcv.hpp \
	antex.hpp

dist_libngpt_a_SOURCES = \
	dtalg.cpp \
	obstype.cpp \
	receiver.cpp \
	antenna.cpp \
	satsys.cpp \
	antex.cpp
