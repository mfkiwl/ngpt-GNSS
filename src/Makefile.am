lib_LIBRARIES = libngpt.a

## Only use the following flags for debuging purposes
libngpt_a_CXXFLAGS = -std=c++14\
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

dist_include_HEADERS   = receiver.hpp \
												 antenna.hpp \
												 satsys.hpp \
												 grid.hpp

dist_libngpt_a_SOURCES = receiver.cpp \
												 antenna.cpp \
												 satsys.cpp
