#include <stdexcept>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <vector>
#include "sp3.hpp"

constexpr std::size_t sat_start_idx  {9}; /// In satellite lines, the sat records start at index: 
constexpr std::size_t sat_stop_idx   {60};/// One past-the-end index
constexpr std::size_t sats_per_line  { (sat_stop_idx-sat_start_idx)/3 };
constexpr std::size_t sats_max_num   {85};
constexpr std::size_t sats_max_lines {5};

/// In an sp3 header, each declared satellite is recorded in a string of 3
/// chars; starting from column 10 up untill column 60. This function will
/// compute the number of lines we have to read to collect all satellites.
int satellite_lines(int sat_nr) noexcept
{
    return (sat_nr-1) / sats_per_line + 1;
}

/// No header line can have more than 80 chars.
constexpr int MAX_HEADER_CHARS { 82 };

ngpt::sp3::sp3(const char* f)
    : _filename(f),
      _istream (f, std::ios::in),
      _end_of_head(0),
      _first_epoch(),
      _last_epoch()
      _satsys(ngpt::satellite_system::mixed)
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
    char* end, *cptr;
    long  lint;
    int   line_nr(0), prev_errno(errno);
    char  posVelFlag;
    char  dataUsed[6];
    char  coordSys[6];
    char  orbType[4];
    char  agency[5];
    char  ints[4] = {'\0', '\0', '\0', '\0'}; /* do not modify ints[3] */
    std::vector<ngpt::satellite> sat_vector;

    // The stream should be open by now!
    assert( this->_istream.is_open() );

    // Go to the top of the file.
    _istream.seekg(0);

    // --------------------------------------
    //  Read line #1.
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
    ngpt::month mt((int)lint);
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
    //  Read line #2.
    // --------------------------------------
    ++line_nr;
    if ( !_istream.getline(line, MAX_HEADER_CHARS) ) {
        std::string line_str = std::to_string(line_nr);
        throw std::runtime_error
            ("sp3::read_header() -> Failed reading line #"+ line_str);
    }
    lint = std::strtol(line+3, &end, 10);
    int gps_w((int)lint);
    double sec_of_week (std::strtod(line+8, &end));
    double eph_interval(std::strtod(line+24, &end));
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
    //  Lines #3 - #7
    //  Read the third line and all subsequent lines that contain satellite
    //  records (that is up to max_sat_lines=5). After this block, we will have
    //  reached (the end of) line 7.
    // --------------------------------------
    ++line_nr;
    if ( !_istream.getline(line, MAX_HEADER_CHARS) ) {
        std::string line_str = std::to_string(line_nr);
        throw std::runtime_error
            ("sp3::read_header() -> Failed reading line #"+ line_str);
    }
    lint = std::strtol(line+4, &end, 10);
    std::size_t num_of_sats ((int)lint);
    assert( num_of_sats <= sats_max_num );
    std::size_t sat_lines = satellite_lines(num_of_sats);
    assert( sat_lines <= sats_max_lines );
    sat_vector.reserve(num_of_sats);
    while ( sat_vector.size() < num_of_sats ) {
        for (cptr = line+sat_start_idx; cptr<line+sat_stop_idx; cptr+=3) {
            sat_vector.emplace_back(cptr);
        }
        if ( !(++line_nr) || !_istream.getline(line, MAX_HEADER_CHARS) ) {
            std::string line_str = std::to_string(line_nr);
            throw std::runtime_error
                ("sp3::read_header() -> Failed reading line #"+ line_str);
        }
    }
    // we have read line_nr lines and need to have read reach line #6
    while ( line_nr < 3+(int)sats_max_lines ) {
        if ( !(++line_nr) || !_istream.getline(line, MAX_HEADER_CHARS) ) {
            std::string line_str = std::to_string(line_nr);
            throw std::runtime_error
                ("sp3::read_header() -> Failed reading line #"+ line_str);
        }
    }
    
    // --------------------------------------
    //  Lines #8 - #12
    //  Read the eighth third line and all subsequent lines that contain satellite
    //  accuracy records (that is up to max_sat_lines=5). After this block, we will have
    //  reached (the end of) line 12.
    // --------------------------------------
    ++line_nr;
    if ( !_istream.getline(line, MAX_HEADER_CHARS) ) {
        std::string line_str = std::to_string(line_nr);
        throw std::runtime_error
            ("sp3::read_header() -> Failed reading line #"+ line_str);
    }
    std::vector<short int> sat_acc;
    sat_acc.reserve(num_of_sats);
    while ( sat_vector.size() < num_of_sats ) {
        for (cptr = line+sat_start_idx; cptr<line+sat_stop_idx; cptr+=3) {
            std::memcpy(ints, cptr, 3);
            lint = std::strtol(ints, &end, 10);
            sat_acc.emplace_back((short int)lint);
        }
        if ( !(++line_nr) || !_istream.getline(line, MAX_HEADER_CHARS) ) {
            std::string line_str = std::to_string(line_nr);
            throw std::runtime_error
                ("sp3::read_header() -> Failed reading line #"+ line_str);
        }
    }
    // we have read line_nr lines and need to have read 12 lines
    while ( line_nr < 3+(int)sats_max_lines*2 ) {
        if ( !(++line_nr) || !_istream.getline(line, MAX_HEADER_CHARS) ) {
            std::string line_str = std::to_string(line_nr);
            throw std::runtime_error
                ("sp3::read_header() -> Failed reading line #"+ line_str);
        }
    }

    // --------------------------------------
    //  Read line #13
    // --------------------------------------
    ++line_nr;
    if ( !_istream.getline(line, MAX_HEADER_CHARS) ) {
        std::string line_str = std::to_string(line_nr);
        throw std::runtime_error
            ("sp3::read_header() -> Failed reading line #"+ line_str);
    }
    this->_satsys = ngpt::char_to_satsys( *(line+3) );
    // TODO what to do with time system ?
    std::memcpy(ints, line+9, 3);
    
    // --------------------------------------
    //  Read line #14
    // -------------------------------------
    ++line_nr;
    if ( !_istream.getline(line, MAX_HEADER_CHARS) ) {
        std::string line_str = std::to_string(line_nr);
        throw std::runtime_error
            ("sp3::read_header() -> Failed reading line #"+ line_str);
    }
    
    // --------------------------------------
    //  Read line #15
    // -------------------------------------
    ++line_nr;
    if ( !_istream.getline(line, MAX_HEADER_CHARS) ) {
        std::string line_str = std::to_string(line_nr);
        throw std::runtime_error
            ("sp3::read_header() -> Failed reading line #"+ line_str);
    }


    // All done
    return 0;

}
