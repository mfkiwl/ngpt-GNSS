noinst_PROGRAMS = testReceiver \
		testAntenna \
		testObstype \
		testGrid \
		testAntex \
		testGeodesy \
		testDatetime \
		testIonex

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
	-DCURSE_IN_GREEK

testReceiver_SOURCES  = test_receiver.cpp
testReceiver_CXXFLAGS = $(MCXXFLAGS) -I$(top_srcdir)/src -L$(top_srcdir)/src
testReceiver_LDADD    = $(top_srcdir)/src/libngpt.la

testAntenna_SOURCES   = test_antenna.cpp
testAntenna_CXXFLAGS  = $(MCXXFLAGS) -I$(top_srcdir)/src -L$(top_srcdir)/src
testAntenna_LDADD     = $(top_srcdir)/src/libngpt.la

testObstype_SOURCES   = test_obstype.cpp
testObstype_CXXFLAGS  = $(MCXXFLAGS) -I$(top_srcdir)/src -L$(top_srcdir)/src
testObstype_LDADD     = $(top_srcdir)/src/libngpt.la

testGrid_SOURCES      = test_grid.cpp
testGrid_CXXFLAGS     = $(MCXXFLAGS) -I$(top_srcdir)/src -L$(top_srcdir)/src

testAntex_SOURCES     = test_antex.cpp
testAntex_CXXFLAGS    = $(MCXXFLAGS) -I$(top_srcdir)/src -L$(top_srcdir)/src
testAntex_LDADD       = $(top_srcdir)/src/libngpt.la

testGeodesy_SOURCES   = test_geodesy.cpp
testGeodesy_CXXFLAGS  = $(MCXXFLAGS) -I$(top_srcdir)/src -L$(top_srcdir)/src
testGeodesy_LDADD     = $(top_srcdir)/src/libngpt.la

testDatetime_SOURCES   = test_datetime.cpp
testDatetime_CXXFLAGS  = $(MCXXFLAGS) -I$(top_srcdir)/src -L$(top_srcdir)/src
testDatetime_LDADD     = $(top_srcdir)/src/libngpt.la

testIonex_SOURCES   = test_ionex.cpp
testIonex_CXXFLAGS  = $(MCXXFLAGS) -I$(top_srcdir)/src -L$(top_srcdir)/src
testIonex_LDADD     = $(top_srcdir)/src/libngpt.la
