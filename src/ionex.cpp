#include <cassert>
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <cstring>
#include "ionex.hpp"

#ifdef DEBUG
    #include <iostream>
#endif

using ngpt::ionex;

/// No header line can have more than 80 chars. However, there are cases when
/// they  exceed this limit, just a bit ...
constexpr int MAX_HEADER_CHARS { 82 };

/**
 *  Size of 'END OF HEADER' C-string.
 *  std::strlen is not 'constexr' so eoh_size can't be one either. Note however
 *  that gcc has a builtin constexpr strlen function (if we want to disable this
 *  we can do so with -fno-builtin).
 */
#ifdef __clang__
    const     std::size_t stm_size { std::strlen("START OF TEC MAP") };
#else
    constexpr std::size_t stm_size { std::strlen("START OF TEC MAP") };
#endif

/// Max header lines.
constexpr int MAX_HEADER_LINES { 1000 };

/// ionex constructor
ngpt::ionex::ionex(const char* filename)
    : _filename(filename),
      _istream (filename, std::ios::in),
      _version (ionex_version::v10),
      _end_of_head(0),
      _time_scale(ngpt::time_scale::ut1),
      _first_epoch( ngpt::min_date<>() ),
      _last_epoch( ngpt::max_date<>() ),
      _interval(0),
      _maps_in_file(0),
      _min_elevation(0),
      _base_radius(0),
      _map_dimension(2),
      _hgt1(0), _hgt2(0), _dhgt(0),
      _lat1(0), _lat2(0), _dlat(0),
      _lon1(0), _lon2(0), _dlon(0),
      _exp(-1)
{
    if ( !_istream.is_open() )
    {
        throw std::runtime_error 
            ("Cannot open ionex file: " + std::string(filename) );
    }
#ifdef DEUG
    try
    {
        this->read_header();
    }
    catch (std::runtime_error& e)
    {
        std::cerr<<"\nFailed to construct IONEX; header error.";
    }
#endif
    if ( this->read_header() )
    {
          if ( _istream.is_open() ) { _istream.close(); }
    }
}

/// [Help function] Read an ionex datetime formated as:
/// YYYY MM DD HH MM SS (all integers as 6I6)
/// A return status other than 0 denotes an error.
int
_read_ionex_datetime_(char* c, ionex::datetime_ms* d)
{
    errno = 0;
    char* start = c;
    char* end;
    int fields[] = { 9999, 99, 99, 99, 99, 99 };

    for (int i=0; i<6; ++i)
    {
        fields[i] = std::strtol(start, &end, 10);
        start = end;
    }
    
    if (errno == ERANGE)
    {
        errno = 0;
        return 1;
    }

    try
    {
        ionex::datetime_ms newd (fields[0], fields[1], fields[2],
                fields[3], fields[4], static_cast<double>(fields[5]));

        *d = newd;
        return 0;
    }
    catch (std::out_of_range& e)
    {
        return 1;
    }

    return 2;
}

/* Read an ionex header.
 *
 * \warning In DEBUG mode this will throw in case of an error.
 *
 * TODO better validate fields after reading header (are all fields read ok??)
 * should fucking know that before going on!
 */
int
ionex::read_header()
{
    char line    [MAX_HEADER_CHARS];
    char sysmodel[4];
    char mapfun  [5];
    char* start, *end;

    // we are going to use strtol() so reset errno
    // see http://en.cppreference.com/w/cpp/string/byte/strtol
    errno = 0;

    // The stream should be open by now!
    assert( this->_istream.is_open() );

    // Go to the top of the file.
    _istream.seekg(0);

    // Read the first line. Get version, file type and system/model.
    // ----------------------------------------------------
    _istream.getline(line, MAX_HEADER_CHARS);
    // strtod will keep on reading untill a non-valid
    // char is read. Fuck this, lets split the string
    // so that it only reads the valid chars (for version).
    *(line+8) = '\0';
    float fvers = std::strtod(line, &end);
    if (std::abs(fvers - 1.0) < .001) {
        this->_version = ionex::ionex_version::v10;
    } else {
#ifdef DEBUG
        throw std::runtime_error
        ("[DEBUG] ionex::read_header -> Invalid IONEX version.");
#endif
        return 1;
    }
    if ( line[20] != 'I' ) {
#ifdef DEBUG
        throw std::runtime_error
        ("[DEBUG] ionex::read_header -> Invalid IONEX file-type");
#endif
        return 1;
    }
    std::memcpy(sysmodel, line+39, 3);
    sysmodel[3] = '\0';

    // Keep on readling lines until 'START OF TEC MAP'.
    // ----------------------------------------------------
    int dummy_it = 0;
    _istream.getline(line, MAX_HEADER_CHARS);
    while (dummy_it < MAX_HEADER_LINES && 
            strncmp(line+60, "START OF TEC MAP", stm_size) )
    {
        if ( !strncmp(line+60, "EPOCH OF FIRST MAP", 18) )
        {
            if ( _read_ionex_datetime_(line, &_first_epoch) )
            {
#ifdef DEBUG
                std::cerr <<"\n[DEBUG] Failed to resolve \"EPOCH OF FIRST MAP\"";
                throw std::runtime_error
                ("[DEBUG] ionex::read_header -> Invalid IONEX line");
#endif
                return 1;
            }
        }
        else if ( !strncmp(line+60, "EPOCH OF LAST MAP", 17) )
        {
            if ( _read_ionex_datetime_(line, &_last_epoch) )
            {
#ifdef DEBUG
                std::cerr <<"\n[DEBUG] Failed to resolve \"EPOCH OF LAST MAP\"";
                throw std::runtime_error
                ("[DEBUG] ionex::read_header -> Invalid IONEX line");
#endif
                return 1;
            }
        }
        else if ( !strncmp(line+60, "INTERVAL", 8) )
        {
            *(line+6) = '\0';
            _interval = std::strtol(line, &end, 10);
        }
        else if ( !strncmp(line+60, "# OF MAPS IN FILE", 17) )
        {
            *(line+6) = '\0';
            _maps_in_file = std::strtol(line, &end, 10);
        }
        else if ( !strncmp(line+60, "MAPPING FUNCTION", 16) )
        {
            std::memcpy(mapfun, line+2, 4);
        }
        else if ( !strncmp(line+60, "ELEVATION CUTOFF", 16) )
        {
            *(line+10) = '\0';
            _min_elevation = std::strtod(line, &end);
        }
        else if ( !strncmp(line+60, "OBSERVABLES USED", 16) )
        {
            /// TODO do i fucking want this ??
        }
        else if ( !strncmp(line+60, "# OF STATIONS", 13) )
        {
            /// TODO do i fucking want this ??
        }
        else if ( !strncmp(line+60, "# OF SATELLITES", 15) )
        {
            /// TODO do i fucking want this ??
        }
        else if ( !strncmp(line+60, "BASE RADIUS", 11) )
        {
            *(line+10) = '\0';
            _base_radius = std::strtod(line, &end);
        }
        else if ( !strncmp(line+60, "MAP DIMENSION", 13) )
        {
            *(line+6) = '\0';
            _map_dimension = std::strtol(line, &end, 10);
        }
        else if ( !strncmp(line+60, "HGT1 / HGT2 / DHGT", 18) )
        {
            start = line+2;
            _hgt1 = std::strtod(start, &end);
            start = end;
            _hgt2 = std::strtod(start, &end);
            start = end;
            _dhgt = std::strtod(start, &end);
        }
        else if ( !strncmp(line+60, "LAT1 / LAT2 / DLAT", 18) )
        {
            start = line+2;
            _lat1 = std::strtod(start, &end);
            start = end;
            _lat2 = std::strtod(start, &end);
            start = end;
            _dlat = std::strtod(start, &end);
        }
        else if ( !strncmp(line+60, "LON1 / LON2 / DLON", 18) )
        {
            start = line+2;
            _lon1 = std::strtod(start, &end);
            start = end;
            _lon2 = std::strtod(start, &end);
            start = end;
            _dlon = std::strtod(start, &end);
        }
        else if ( !strncmp(line+60, "EXPONENT", 8) )
        {
            *(line+10) = '\0';
            _exp = std::strtol(line, &end, 10);
        }
        else if ( !strncmp(line+60, "START OF AUX DATA", 17) )
        {
#ifdef DEBUG
            std::cerr<<"\n[DEBUG] Fuck! This IONEX has AUX data.";
            std::cerr<<"\n        Don't know what to do with this shit!";
            _istream.getline(line, MAX_HEADER_CHARS);
#endif
            while (dummy_it < MAX_HEADER_LINES && 
                strncmp(line+60, "END OF AUX DATA", 15)) {
#ifdef DEBUG
                std::cerr << "\n[DEBUG] Skipping: [" << line << "]";
#endif
            }
        }
        // check for str to numeric translation error
        if ( errno == ERANGE )
        {
#ifdef DEBUG
            std::cerr << "\n[DEBUG] Failed to translate line: ";
            std::cerr << "\n        line:[" << line;
            errno = 0;
            throw std::runtime_error
            ("[DEBUG] ionex::read_header -> Invalid IONEX line");
#endif
            errno = 0;
            return 1;
        }
        _istream.getline(line, MAX_HEADER_CHARS);
        dummy_it++;
    }
    if (dummy_it >= MAX_HEADER_LINES) {
#ifdef DEBUG
        throw std::runtime_error
        ("[DEBUG] ionex::read_header -> Could not find 'END OF HEADER'.");
#endif
        return 1;
    }

    this->_end_of_head = _istream.tellg();
    
    return 0;
}

/// Read a TEC map off from sthe stream
/// \warning The buffer should be placed exactly after the line:
/// "START OF TEC MAP"
///
int
ionex::read_map()
{
    static char line [MAX_HEADER_CHARS];
    datetime_ms d;
    
    // The stream should be open by now!
    assert( this->_istream.is_open() );
    
    // next line we read should be 'EPOCH OF CURRENT MAP'
    _istream.getline(line, MAX_HEADER_CHARS);
    if ( strncmp(line+60, "EPOCH OF CURRENT MAP", 20) ||
        _read_ionex_datetime_(line, &d) )
    {
#ifdef DEBUG
        std::cerr<<"\n[DEBUG] Error reading TEC map 'EPOCH OF CURRENT MAP'";
        std::cerr<<"\n        found line: " << line;
        throw std::runtime_exception
        ("ionex::read_map() -> Invalid line");
#endif
        return 1;
    }

    // newxt line should be 'LAT/LON1/LON2/DLON/H'

}
