bin_PROGRAMS = testReceiver \
		testAntenna \
		testObstype \
		testGrid \
		testAntex \
		testGeodesy

testReceiver_SOURCES  = test_receiver.cpp
testReceiver_CXXFLAGS = -std=c++14 -g -Wall -Wextra -pedantic -W -Wshadow \
			-Winline -Wdisabled-optimization -DDEBUG \
			-I$(top_srcdir)/src -L$(top_srcdir)/src
testReceiver_LDADD    = $(top_srcdir)/src/libngpt.a

testAntenna_SOURCES   = test_antenna.cpp
testAntenna_CXXFLAGS  = -std=c++14 -g -Wall -Wextra -pedantic -W -Wshadow \
			-Winline -Wdisabled-optimization -DDEBUG \
			-I$(top_srcdir)/src -L$(top_srcdir)/src
testAntenna_LDADD     = $(top_srcdir)/src/libngpt.a

testObstype_SOURCES   = test_obstype.cpp
testObstype_CXXFLAGS  = -std=c++14 -g -Wall -Wextra -pedantic -W -Wshadow \
			-Winline -Wdisabled-optimization -DDEBUG \
			-I$(top_srcdir)/src -L$(top_srcdir)/src
testObstype_LDADD     = $(top_srcdir)/src/libngpt.a

testGrid_SOURCES      = test_grid.cpp
testGrid_CXXFLAGS     = -std=c++14 -g -Wall -Wextra -pedantic -W -Wshadow \
			-Winline -Wdisabled-optimization -DDEBUG \
			-I$(top_srcdir)/src -L$(top_srcdir)/src

testAntex_SOURCES     = test_antex.cpp
testAntex_CXXFLAGS    = -std=c++14 -g -Wall -Wextra -pedantic -W -Wshadow \
			-Winline -Wdisabled-optimization -DDEBUG \
			-I$(top_srcdir)/src -L$(top_srcdir)/src
testAntex_LDADD       = $(top_srcdir)/src/libngpt.a

testGeodesy_SOURCES     = test_geodesy.cpp
testGeodesy_CXXFLAGS    = -std=c++14 -g -Wall -Wextra -pedantic -W -Wshadow \
			-Winline -Wdisabled-optimization -DDEBUG \
			-I$(top_srcdir)/src -L$(top_srcdir)/src
testGeodesy_LDADD       = $(top_srcdir)/src/libngpt.a
