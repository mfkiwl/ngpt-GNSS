bin_PROGRAMS = atxtr \
		inxtr

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
	-DDEBUG \
	-DALLOW_JSON_OUT

atxtr_SOURCES     = atxtr.cpp
atxtr_CXXFLAGS    = $(MCXXFLAGS) -I$(top_srcdir)/src -L$(top_srcdir)/src
atxtr_LDADD       = $(top_srcdir)/src/libngpt.la

inxtr_SOURCES     = inxtr.cpp
inxtr_CXXFLAGS    = $(MCXXFLAGS) -I$(top_srcdir)/src -L$(top_srcdir)/src
inxtr_LDADD       = $(top_srcdir)/src/libngpt.la
