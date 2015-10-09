autoreconf -i

## to build with g++, if (default) version < 5
CXX=g++-5 ./configure

## to build with clang++
CXX=clang++ ./configure CXXFLAGS='-stdlib=libc++'

make
