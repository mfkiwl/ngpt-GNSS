bin_PROGRAMS = atxtr

MCXXFLAGS = \
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

atxtr_SOURCES     = atxtr.cpp
atxtr_CXXFLAGS    = $(MCXXFLAGS) -I$(top_srcdir)/src -L$(top_srcdir)/src
atxtr_LDADD       = $(top_srcdir)/src/libngpt.la
