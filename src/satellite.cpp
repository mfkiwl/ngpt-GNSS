#include "satellite.hpp"

/** Resolve a satellite from a string of type: SXX, where
 *  'S' is a satellite system identifier and 'XX' is a two
 *  digit, positive integer.
 */
ngpt::satellite::satellite(const char* c)
    : _sys{ngpt::char_to_satsys(*c)},
      _prn{(c[1]-'0')*10+c[2]-'0'}
{
    assert( c[1]-'0' >= 0 && c[1]-'0' <= 9 );
    assert( c[2]-'0' >= 0 && c[2]-'0' <= 9 );
}
