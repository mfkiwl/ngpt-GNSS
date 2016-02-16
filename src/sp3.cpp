#include <stdexcept>
#include <cassert>
#include "sp3.hpp"

/// No header line can have more than 80 chars.
constexpr int MAX_HEADER_CHARS { 82 };

ngpt::sp3::sp3(const char* f)
    : _filename(f),
      _istream (f, std::ios::in),
      _end_of_head(0),
      _first_epoch(),
      _last_epoch()
{
    if ( !_istream.is_open() ) {
        throw std::runtime_error
            ("sp3::sp3() -> Cannot open sp3 file: " + std::string(f) );
    }
}

int
ngpt::sp3::read_header()
{
    char  line [MAX_HEADER_CHARS];
    char* end;
    char  posVelFlag;
    long  lint;

    // The stream should be open by now!
    assert( this->_istream.is_open() );

    // Go to the top of the file.
    _istream.seekg(0);

    // Read the first line.
    _istream.getline(line, MAX_HEADER_CHARS);
    posVelFlag = line[2];
    lint = std::strtol(line+3, &end, 10); // next column is blank, so we're cool
    ngpt::year yr((int)lint);
    lint = std::strtol(line+8, &end, 10); // next column is blank, so we're cool
    ngpt::month mn((int)lint);
    lint = std::strtol(line+11, &end, 10); // next column is blank, so we're cool
    ngpt::day_of_month dm((int)lint);
    lint = std::strtol(line+14, &end, 10); // next column is blank, so we're cool
    ngpt::hour hr((int)lint);
    lint = std::strtol(line+17, &end, 10); // next column is blank, so we're cool
    ngpt::minute mn((int)lint);
    lint = std::strtod(line+20, &end); // next column is blank, so we're cool
    ngpt::day_of_month dm((int)lint);

    
}
