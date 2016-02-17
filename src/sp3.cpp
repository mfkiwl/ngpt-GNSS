#include <stdexcept>
#include <cassert>
#include <cstring>
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
    long  lint;
    int   line_nr(0), prev_errno(errno);
    char  posVelFlag;
    char  dataUsed[6];
    char  coordSys[6];
    char  orbType[4];
    char  agency[5];

    // The stream should be open by now!
    assert( this->_istream.is_open() );

    // Go to the top of the file.
    _istream.seekg(0);

    // --------------------------------------
    //  Read the first line.
    // --------------------------------------
    ++line_nr;
    if ( !_istream.getline(line, MAX_HEADER_CHARS) ) {
        std::string line_str = std::to_string(line_nr);
        throw std::runtime_error
            ("sp3::read_header() -> Failed reading line #"+ line_str);
    }
    posVelFlag = line[2];
    lint = std::strtol(line+3, &end, 10); // next column is blank, so we're cool
    ngpt::year yr((int)lint);
    lint = std::strtol(line+8, &end, 10);
    ngpt::month mn((int)lint);
    lint = std::strtol(line+11, &end, 10);
    ngpt::day_of_month dm((int)lint);
    lint = std::strtol(line+14, &end, 10);
    ngpt::hours hr((int)lint);
    lint = std::strtol(line+17, &end, 10);
    ngpt::minutes mn((int)lint);
    double decimal_sec (std::strtod(line+20, &end));
    lint = std::strtol(line+32, &end, 10);
    int num_of_epochs ((int)lint);
    std::memcpy(dataUsed, line+40, 5);
    std::memcpy(coordSys, line+46, 5);
    std::memcpy(orbType,  line+52, 3);
    std::memcpy(agency,   line+56, 4);
    dataUsed[5] = coordSys[5] = orbType[3] = agency[4] = '\0';
    if ( errno == ERANGE ) {
        errno = prev_errno;
        std::string line_str = std::to_string(line_nr);
        throw std::runtime_error
            ("sp3::read_header() -> Failed reading line #"+line_str);
    }
    
    // --------------------------------------
    //  Read the second line.
    // --------------------------------------
    ++line_nr;
    if ( !_istream.getline(line, MAX_HEADER_CHARS) ) {
        std::string line_str = std::to_string(line_nr);
        throw std::runtime_error
            ("sp3::read_header() -> Failed reading line #"+ line_str);
    }
    lint = std::strtol(line+3, &end, 10);
    int gps_w((int)lint);
    double sec_of_week (std::strod(line+8, &end));
    double eph_interval(std::strod(line+24, &end));
    lint = std::strtol(line+39, &end, 10);
    ngpt::modified_julian_day mjd((int)lint);
    double fract_day(std::strtod(line+45, &end));
    if ( errno == ERANGE ) {
        errno = prev_errno;
        std::string line_str = std::to_string(line_nr);
        throw std::runtime_error
            ("sp3::read_header() -> Failed reading line #"+line_str);
    }

    // --------------------------------------
    //  Read the second third line.
    // --------------------------------------
    ++line_nr;
    if ( !_istream.getline(line, MAX_HEADER_CHARS) ) {
        std::string line_str = std::to_string(line_nr);
        throw std::runtime_error
            ("sp3::read_header() -> Failed reading line #"+ line_str);
    }
    lint = std::strtol(line+4, &end, 10);
    int num_of_sats ((int)lint);
    
    if ( errno == ERANGE ) {
        errno = prev_errno;
        std::string line_str = std::to_string(line_nr);
        throw std::runtime_error
            ("sp3::read_header() -> Failed reading line #"+line_str);
    }

}
