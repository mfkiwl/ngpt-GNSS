bin_PROGRAMS = testReceiver \
			   testAntenna

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
