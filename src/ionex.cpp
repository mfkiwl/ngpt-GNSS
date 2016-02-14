#include <cassert>
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <cstring>
#include <algorithm>
#include "ionex.hpp"
#include "grid.hpp"

#ifdef DEBUG
    #include <iostream>
#endif

using ngpt::ionex;
using ngpt::ionex_grd_type;

/// No header line can have more than 80 chars. However, there are cases when
/// they  exceed this limit, just a bit ...
constexpr int MAX_HEADER_CHARS { 82 };

/// According to IONEX specification, each longtitude line can contain a
/// maximum of 16 TEC values (\cite inx1).
constexpr std::size_t MAX_TEC_PER_LINE { 16 };

/**
 *  Size of 'END OF HEADER' C-string.
 *  std::strlen is not 'constexr' so eoh_size can't be one either. Note however
 *  that gcc has a builtin constexpr strlen function (if we want to disable this
 *  we can do so with -fno-builtin).
 */
#ifdef __clang__
    const     std::size_t eoh_size { std::strlen("END OF HEADER") };
#else
    constexpr std::size_t eoh_size { std::strlen("END OF HEADER") };
#endif

/// Max header lines.
constexpr int MAX_HEADER_LINES { 1000 };

/// ionex constructor
ngpt::ionex::ionex(const char* filename)
    : _filename(filename),
      _istream (filename, std::ios::in),
      _version (ionex_version::v10),
      _end_of_head(0),
      // _time_scale(ngpt::time_scale::ut1),
      _first_epoch(),
      _last_epoch(),
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
    if ( !_istream.is_open() ) {
        throw std::runtime_error 
            ("ionex::ionex() -> Cannot open ionex file: " + std::string(filename) );
    }
#ifdef DEUG
    try {
        this->read_header();
    } catch (std::runtime_error& e) {
        std::cerr<<"\nFailed to construct IONEX; header error.";
    }
#endif
    if ( this->read_header() ) {
          if ( _istream.is_open() ) { _istream.close(); }
          throw std::runtime_error
            ("ionex::ionex() -> Failed to read IONEX header.");
    }
}

/**  [Help function] Read an ionex datetime formated as:
 *   YYYY MM DD HH MM SS (all integers as 6I6)
 *   A return status other than 0 denotes an error.
 * 
 *   \parameter[in] c a c-string holding the date string.
 *   \parameter[in] d a datetime_ms instance where the (resolved) date is
 *                    assigned to.
 * 
 *   \returns         an integer denoting the convertion status; if not zero,
 *                    the conversion failed.
 */
int
_read_ionex_datetime_(char* c, ionex::datetime_ms* d)
noexcept
{
    int prev_errno {errno};
    errno = 0;
    char* start = c;
    char* end;
    int fields[] = { 9999, 99, 99, 99, 99, 99 };

    for (int i=0; i<6; ++i) {
        fields[i] = std::strtol(start, &end, 10);
        start = end;
    }
    
    if (errno == ERANGE) {
        errno = prev_errno;
        return 1;
    }

    try {
        ionex::datetime_ms newd (ngpt::year{fields[0]},
                                 ngpt::month{fields[1]},
                                 ngpt::day_of_month{fields[2]},
                                 ngpt::milliseconds{
                                  (  fields[3]*60L*60L
                                   + fields[4]*60L
                                   + fields[5])*1000L}
                                 );
        *d = newd;
        return 0;
    } catch (std::out_of_range& e) {
        errno = prev_errno;
        return 1;
    }
}

/** Read an ionex header and assign this instance's member variables. After
 *  the headre is (successefuly) read, the instance should have valid:
 *
 *  IONEX FIELD          | MEMBER VAR
 *  ---------------------|-----------------------------
 *  IONEX VERSION        | _version
 *  EPOCH OF FIRST MAP   | _first_epoch
 *  EPOCH OF LAST MAP    | _last_epoch
 *  INTERVAL             | _interval
 *  # OF MAPS IN FILE    | _maps_in_file
 *  MAPPING FUNCTION     |
 *  ELEVATION CUTOFF     | _min_elevation
 *  OBSERVABLES USED     |
 *  # OF STATIONS        |
 *  # OF SATELLITES      |
 *  BASE RADIUS          |
 *  MAP DIMENSION        | _map_dimension
 *  HGT1 / HGT2 / DHGT   | _hgt1, _hgt2, _dhgt
 *  LAT1 / LAT2 / DLAT   | _lat1, _lat2, _dlat
 *  LON1 / LON2 / DLON   | _lon1, _lon2, _dlon
 *  EXPONENT             | _exp
 *  START OF AUX DATA(*) |
 *  END AUX DATA(*)      |
 *  END OF HEADER        | _end_of_head
 *
 *  (*) When encountered, they are read and skipped; from "START OF AUX DATA"
 *  until "END AUX DATA".
 *
 * \warning In DEBUG mode this will throw in case of an error.
 *
 * \todo TODO better validate fields after reading header (are all fields read ok??)
 *            should fucking know that before going on!
 */
int
ionex::read_header()
#ifndef DEBUG
noexcept
#endif
{
    char line    [MAX_HEADER_CHARS];
    char sysmodel[4];
    char mapfun  [5];
    char* start, *end;
    int prev_errno = errno;

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
    line[8] = '\0';
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

    // Keep on readling lines until 'END OF HEADER'.
    // ----------------------------------------------------
    int dummy_it = 0;
    _istream.getline(line, MAX_HEADER_CHARS);
    while (dummy_it < MAX_HEADER_LINES && 
            strncmp(line+60, "END OF HEADER", eoh_size) )
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
            if ( _map_dimension != 2 || errno == ERANGE ) {
                errno = prev_errno;
#ifdef DEBUG
                std::cerr <<"\n[DEBUG] Oh shit! This map-dimension is not supported";
                std::cerr <<"\n        Need to add code bitch!";
                throw std::runtime_error
                    ("ionex::read_header() -> Invalid map dimension");
#endif
                    return 1;
            }
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
#endif
            while (  dummy_it < MAX_HEADER_LINES
                  && _istream.getline(line, MAX_HEADER_CHARS)
                  && strncmp(line+60, "END OF AUX DATA", 15)) {
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
            errno = prev_errno;
            throw std::runtime_error
            ("[DEBUG] ionex::read_header -> Invalid IONEX line");
#endif
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

/**  Compute # of maps for constant height, i.e. how many from _lat1 to _lat2
 *   with a step of _dlat.
 * 
 *   \warning This function uses the fact that the (latitude) grid is given
 *            with a precision of 10e-2 degrees.
 */ 
std::size_t
ionex::latitude_maps()
const noexcept
{
    long lat1 = static_cast<long>(_lat1 * 100);
    long lat2 = static_cast<long>(_lat2 * 100);
    long dlat = static_cast<long>(_dlat * 100);
    long maps = (lat2 - lat1) / dlat + 1;
    assert( maps > 0 );
    return static_cast<std::size_t>( maps );
}

/**  Compute how many lines should be read off from a const-latitude map.
 * 
 *   \warning This function uses the fact that the (longtitude) grid is given
 *            with a precision of 10e-1 degrees.
 */ 
std::size_t
ionex::longtitude_lines()
const noexcept
{
    long lon1 = static_cast<long>(_lon1 * 100);
    long lon2 = static_cast<long>(_lon2 * 100);
    long dlon = static_cast<long>(_dlon * 100);
    long vals = (lon2 - lon1) / dlon + 1;
    assert( vals > 0 );
    long rows = vals / MAX_TEC_PER_LINE + (vals % MAX_TEC_PER_LINE > 0);
    return static_cast<std::size_t>( rows );
}

/**  Compute the number of tec values recorded for a const-epoch map.
 *
 *   \warning This function uses the fact that the (longtitude) grid is given
 *            with a precision of 10e-1 degrees.
 */
std::size_t
ionex::tec_vals_per_epoch()
const noexcept
{
    std::size_t lat_points { this->latitude_maps() };
    long        lon_points { (static_cast<long>(_lat2*100)
                            - static_cast<long>(_lat1*100))
                             /static_cast<long>(_dlat*100)+1
                           };
    assert( lon_points > 0 );
    return lat_points * static_cast<std::size_t>(lon_points);
}

/**  Read and skip a whole map (const epoch) . The buffer should be placed in 
 *   a position such that the first line to be read from the input stream is:
 *   "LAT/LON1/LON2/H". After the function has returned, the buffer should be 
 *   placed after the line: "END OF TEC MAP".
 *   The tec map is read, and absolutely nothing is done with it.
 *   By whole map, i mean all const-latitude maps for a given epoch.
 *
 *   \returns An integer denoting the exit status; anything other than 0, 
 *            denotes failure.
 */
int
ionex::skip_tec_map()
{
    static char line [MAX_HEADER_CHARS];
    char* c;
    ionex_grd_type lat = _lat1;
    ionex_grd_type ltmp;
    bool ascending ( _lat2 > _lat1 ? true : false );
    int prev_errno = errno;

    // number of const-latitude longtitude lines.
    std::size_t num_of_tec_lines = this->longtitude_lines();
    
    while ( _istream.getline(line, MAX_HEADER_CHARS) ) {
        // next line should be 'LAT/LON1/LON2/DLON/H'
        if ( std::strncmp(line+60, "LAT/LON1/LON2/DLON/H", 20) ) {
#ifdef DEBUG
            std::cerr<<"\n[DEBUG] Error reading TEC map 'LAT/LON1/LON2/DLON/H'";
            std::cerr<<"\n        found line: " << line;
            throw std::runtime_error
            ("ionex::skip_tec_map() -> Invalid line");
#endif
            return 1;
        }
        // read and validate the current latitude
        ltmp = std::strtof(line+2, &c);
        if ( (int)(ltmp*100) != (int)(lat*100) || errno == ERANGE ) {
            errno = prev_errno;
#ifdef DEBUG
            std::cerr<<"\n[DEBUG]What the fuck man! Read invalid latitude.";
            std::cerr<<"\n       Expected: "<<lat<<", found: "<<ltmp;
            throw std::runtime_error("ionex::skip_tec_map() -> Invalid latitude");
#endif
            return 1;
        }

        // ok, now we should read these fucking TEC vals; format: I5 max 16 values
        // per line.
        for (std::size_t i=0; i<num_of_tec_lines; ++i) {
            if ( !_istream.getline(line, MAX_HEADER_CHARS) ) {
#ifdef DEBUG
                std::cerr<<"\n[DEBUG] Fucking weird! Failed to read tec map!";
                throw std::runtime_error("ionex::skip_tec_map() -> Invalid line");
#endif
                return 1;
            }
        }

        // did we end yet (better compare ints than floats)?
        lat += _dlat;
        if ( ascending == ((long)(lat*100)>=(long)(_lat2*100)) ) {
            break;
        }
    }

    // should now read 'END OF TEC MAP'
    if (  !_istream.getline(line, MAX_HEADER_CHARS)
        ||std::strncmp(line+60, "END OF TEC MAP", 14) )
    {
#ifdef DEBUG
        std::cerr<<"\n[DEBUG] Expected \"END OF TEC MAP\" but found:";
        std::cerr<<"\n        "<<line;
        throw std::runtime_error
            ("ionex::skip_tec_map() -> Invalid line");
#endif
            return 1;
    }

    // all done probably correctly.
    return 0;
}

/**  Read a TEC map for a given epoch. All values are going to be read into the
 *   vector tec_vals, at the order they are read. This means that the vector
 *   will be of the form (at exit):
\verbatim
    vec[0]   tec value at -> lat1, lon1
    vec[1]   tec value at -> lat1, lon1+dlon
    vec[2]   tec value at -> lat1, lon1+2*dlon
    ...
    vec[n]   tec value at -> lat1+dlat, lon1
    vec[n+1] tec value at -> lat1+dlat, lon1+dlon
    ...
\endverbatim
 *   It is expected that the first line to be read from the input stream is
 *   the line "LAT/LON1/LON2/DLON/H".
 *   The function will stop after rading the line: "END OF TEC MAP".
 *
 *  \param[in] pcv_vals An int vector of size large enough to hold all tec
 *             values to be read from the map.
 *
 *  \returns An integer denoting the exit status; everything other than 0,
 *           denotes an error.
 *
 *  \warning -# The size of the vector should be large enough to hold the read
 *           values. The size of the vector will not be modified at any way.
 *           -# Note that the get the actual TEC values from the values stored
 *           in the vector, you will need to use the IONEX's exponent.
 *
 */
int
ionex::read_tec_map(std::vector<int>& pcv_vals)
{
    static char line[MAX_HEADER_CHARS];
    std::size_t index = 0;
    
    // how many consti-latitude maps should we read ?
    std::size_t lat_maps  (this->latitude_maps() );
    
    // each const-latitude map has how many tec lines ?
    std::size_t lon_lines (this->longtitude_lines() );

    // reset errno
    int prev_errno = errno;
    errno = 0;

    // check that the input vector is large enough
    assert( pcv_vals.size() >= this->tec_vals_per_epoch() );

    for (std::size_t i=0; i<lat_maps; ++i) {
        if ( this->read_latitude_map(lon_lines, pcv_vals, index) ) {
            errno = prev_errno;
            return 1;
        }
    }
    
    // should now read 'END OF TEC MAP'
    if ( ! _istream.getline(line, MAX_HEADER_CHARS)
        || std::strncmp(line+60, "END OF TEC MAP", 14) ) {
#ifdef DEBUG
        std::cerr<<"\n[DEBUG] Expected \"END OF TEC MAP\" but found:";
        std::cerr<<"\n        "<<line;
        throw std::runtime_error
            ("ionex::read_tec_map() -> Invalid line");
#endif
            errno = prev_errno;
            return 1;
    }

    return 0;
}

/**  Read a TEC map (for a given latitude) off from the stream and store the
 *   values in the vec vector, starting at vec[index]. The function will modify
 *   the index value, such that at return it will denote the last index inserted
 *   into the vector plus one (i.e. if index is J at input and the function
 *   reads N tec values, the index value at output will be J+N). The tec values
 *   are **not** pushed back, but inserted (via the vector::operator[]), so
 *   make sure the input vector is large enough.
 *
 *   \param[in] num_of_tec_lines The number of lines to read off from a const
 *                               latitude map (i.e. ionex::longtitude_lines()).
 *   \param[in] vec              The (int) vector where read values are stored.
 *                               This function will start storing values at
 *                               vec[index].
 *   \param[in] index            Where to start storing tec values within the
 *                               input vec vector.
 *                               At exit, this will be set to the first non-set
 *                               element (i.e. if index was 0 at input and the
 *                               function reads and assigns 10 elements, index
 *                               at output will be 10).
 * 
 *  \warning -# The buffer should be placed in a position such that the next line
 *           to be read is "LAT/LON1/LON2/DLON/H".
 * 
 */ 
int
ionex::read_latitude_map(std::size_t num_of_tec_lines,
                         std::vector<int>& vec,
                         std::size_t& index)
{
    static char line [MAX_HEADER_CHARS];
    datetime_ms d;
    char* start, *end;
    ionex_grd_type lat, lon1, lon2, dlon, hgt;
    long lval;
    int  prev_errno = errno;
    errno = 0;
    
    // next line should be 'LAT/LON1/LON2/DLON/H'
    if ( !_istream.getline(line, MAX_HEADER_CHARS) 
         || std::strncmp(line+60, "LAT/LON1/LON2/DLON/H", 20) )
    {
        errno = prev_errno;
#ifdef DEBUG
        std::cerr<<"\n[DEBUG] Error reading TEC map 'LAT/LON1/LON2/DLON/H'";
        std::cerr<<"\n        found line: " << line;
        throw std::runtime_error
        ("ionex::read_map() -> Invalid line");
#endif
        return 1;
    }
    // resolve the lat/lon limits.
    start = line+2;
    lat  = std::strtof(start, &end);
    start += 6;
    lon1 = std::strtof(start, &end);
    start += 6;
    lon2 = std::strtof(start, &end);
    start += 6;
    dlon = std::strtof(start, &end);
    start += 6;
    hgt  = std::strtof(start, &end);
    if (   lon1 != _lon1
        || lon2 != _lon2
        || dlon != _dlon
        || hgt  != _hgt1
        || errno == ERANGE )
    {
        errno = prev_errno;
#ifdef DEBUG
        std::cerr << "\n[DEBUG] Oh Fuck! this longtitude seems corrupt!";
        std::cerr << "\n        line: " << line;
        std::string lat_str = std::to_string(lat);
        throw std::runtime_error("ionex::read_map() -> Invalid line ("+lat_str+")");
#endif
        return 1;
    }

    // ok, now we should read these fucking TEC vals; format: I5 max 16 values
    // per line.
    for (std::size_t i=0; i<num_of_tec_lines; ++i) {
        if ( !_istream.getline(line, MAX_HEADER_CHARS) ) {
            errno = prev_errno;
#ifdef DEBUG
            std::cerr<<"\n[DEBUG] Fucking weird! Failed to read tec map!";
            throw std::runtime_error("ionex::read_map() -> Invalid line");
#endif
            return 1;
        }
        start = end = line;
        for (lval = std::strtol(start, &end, 10);
             start != end;
             lval = std::strtol(start, &end, 10) )
        {
            vec[index] = static_cast<int>( lval );
            ++index;
            start = end;
        }
    }

    // check for transformation errors!
    if ( errno == ERANGE ) {
        errno = prev_errno;
#ifdef DEBUG
        std::cerr<<"\n[DEBUG] Fuck! Reading tec values failed!";
        throw std::runtime_error("ionex::read_map() -> Invalid line");
#endif
        return 1;
    }

    // all done probably correctly.
    return 0;
}

/** Extract TEC values for a given list of points, for all epochs included in 
 *  the IONEX instance, within the interval [from, to]; or all epochs, if from
 *  and to are NULL. For example, if points holds (p1, p2, ..., pn2), then
 *  (on exit), the tec_vals will be formed as:
 *  tec_vals[0][0] -> tec at point p0, at epoch epoch_vector[0]
 *  tec_vals[0][1] -> tec at point p0, at epoch epoch_vector[1]
 *  ...
 *  tec_vals[0][m] -> tec at point p0, at epoch epoch_vector[m]
 *  tec_vals[1][0] -> tec at point p1, at epoch epoch_vector[0]
 *  tec_vals[1][1] -> tec at point p1, at epoch epoch_vector[1]
 *  ...
 *  tec_vals[1][m] -> tec at point p1, at epoch epoch_vector[m]
 *  ....
 *
 *  \param[in] points A vector of coordinates of type (longtitude, latitude)
 *                    in (decimal) degrees (two decimal places are considered)
 *  \param[in] epoch_vector This vector will contain (at exit) the epochs for
 *                    which the function computed TEC values.
 *  \param[in] tec_vals A vector of vectors of ints! For each point in points,
 *                    there will be a corresponding vector of ints in tec_vals
 *                    holding tec values at epoch_vector epochs.
 *  \returns          A vector of int vectors; for each point given, the function
 *                    will create for a vector of tec values for each epoch in the
 *                    epoch_vector
 *  \warning          All (input) vectors should be large enough to handle the
 *                    values assigned to them. Watch for junk if you pass vectors
 *                    of size larger that the number of epochs collected.
 * 
 */
int
ionex::get_tec_at(const std::vector<std::pair<ionex_grd_type,ionex_grd_type>>& points,
                  std::vector<datetime_ms>& epoch_vector,
                  std::vector<std::vector<int>>& tec_vals,
                  datetime_ms* from,
                  datetime_ms* to
                )
{
    // for every point we want, we must find an index for it (within the grid)
    // so that we can extract it's value. Let's make a 2D grid. For ease, let's
    // set the grid points to long (instead of ints) so that e.g. lat=37.5
    // lon=23.7 will be 3750 and 2370; i.e. use of factor of 100.
    int factor (100);
    typedef ngpt::grid_skeleton<long, ngpt::grid_dimension::two_dim> gstype;
    gstype grid(_lon1*factor, _lon2*factor, _dlon*factor,
                _lat1*factor, _lat2*factor, _dlat*factor);

    if ( !from ) from = &this->_first_epoch;
    if ( !to   ) to   = &this->_last_epoch;

    // for each point in the vector, we are going to need the surounding nodes 
    // (so that we extract these values and interpolate).
    /*
    typedef gstype::node gnode;
    typedef std::tuple<gnode, gnode, gnode, gnode> cell;
    std::vector<cell> cells;
    cells.reserve( points.size() );
    for ( auto const& i : points ) {
        cells.push_back(grid.neighbor_nodes(i.first*factor, i.second*factor));
    }
    */

    // get me a vector large enough to hold a whole map
    std::vector<int> tec_map (grid.size(), 0);

    // go to 'END OF HEADER'
    char line[MAX_HEADER_CHARS];
    _istream.seekg(_end_of_head, std::ios::beg);
    std::size_t map_num = 0;
    char* c;
    //std::size_t eph_index = 0;
    
    if ( !_istream.getline(line, MAX_HEADER_CHARS) ) {
#ifdef DEBUG
        std::cerr<<"\n[DEBUG] WTF? Failed to read input line!";
        throw std::runtime_error
            ("ionex::get_tec_at() -> Invalid line!");
#endif
        return 1;
    }

    datetime_ms cur_dt = _first_epoch;
    
    while ( map_num < _maps_in_file
            && _istream
            && !std::strncmp(line+60, "START OF TEC MAP", 16) ) {

        *(line+6) = '\0';
        assert( static_cast<long>(map_num+1) == std::strtol(line, &c, 10) );
        if ( ! _istream.getline(line, MAX_HEADER_CHARS)
            || std::strncmp(line+60, "EPOCH OF CURRENT MAP", 20) 
            || _read_ionex_datetime_(line, &cur_dt) )
        {
#ifdef DEBUG
            std::cerr<<"\n[DEBUG] Expected line \"EPOCH OF CURRENT MAP\", found:";
            std::cerr<<"\n        "<<line;
            throw std::runtime_error
                ("ionex::get_tec_at() -> Invalid line!");
#endif
            return 1;
        }

        if ( cur_dt >= *from && cur_dt <= *to ) {
            // read the map into memmory
            if ( this->read_tec_map(tec_map) ) {
#ifdef DEBUG
                std::cerr<<"\n[DEBUG] Failed reading map nr "<<map_num;
                throw std::runtime_error
                    ("ionex::get_tec_at() -> failed reading maps.");
#endif
                return 1;
            }

            // ok. we got the map and we need to extract the cells for all points
            // index; points to current point
            std::size_t j = 0; // point index
            for (const auto& p : points) {
                tec_vals[j].emplace_back( grid.bilinear_interpolation(
                                            p.first,
                                            p.second,
                                            /*cells[j]*/
                                            tec_map.data())
                                        );
                ++j;
            }
            epoch_vector.emplace_back( cur_dt );
            //++eph_index;
        } else {
            if ( skip_tec_map() ) {
#ifdef DEBUG
                std::cerr<<"\n[DEBUG] Failed skipping map nr "<<map_num;
                throw std::runtime_error
                    ("ionex::get_tec_at() -> failed reading maps.");
#endif
                return 1;
            }
        }
        _istream.getline(line, MAX_HEADER_CHARS);
        ++map_num;
    }

    // if everything went as planned, we should have read all maps in file.
    return !(map_num == _maps_in_file);
}

/** Parse the epooch-related arguments as given to the ionex::interpolate
 *  function. The possible options are:
 *  -# If the epochs vector has size other than 0, then these epochs are used;
 *  ifrom is set to the first epoch in the epochs vector and to is set to the
 *  last
 *  -# If the epochs vector has size equal to 0, then:
 *      - If from is < first epoch in file, it is set equal to first epoch in file
 *      - If to is > last epoch in file, it is set equal to last epoch in file
 *      - if interval is <= 0, then all maps in the interval [from, to] are used
 *  In case the epochs vector is empty and the interval is set to 0, then the
 *  epochs vector will be returned empty, and should be filled with all epochs.
 *  In all other cases, the epochs vector (at exit) should contain all epochs
 *  be extracted in the interval [first_epoch_in_file, last_epoch_in_file].
 *
 *  \returns An integer denoting the status:
 *      -# -1 : the epoch vector is empty; should be filled with all epochs in file
 *      -#  0 : all cool
 *      -# >0 : error
 */
int
ionex::parse_epoch_arguments(std::vector<datetime_ms>& epochs,
                             datetime_ms* from,
                             datetime_ms* to,
                             int& interval)
{
    // sanity check
    if ( from && *from > this->_last_epoch ) {
        std::cerr<<"\n"<<this->filename()<<" says : Too far in the future";
        return 1;
    }
    if ( to && *to < this->_first_epoch ) {
        std::cerr<<"\n"<<this->filename()<<" says : Too far in the past";
        return 1;
    }

    if ( epochs.empty() ) {
        if ( !from ) { from = &this->_first_epoch; }
        if ( !to   ) { to   = &this->_last_epoch;  }
        if ( interval > 0 ) {
            for (datetime_ms tmp = *from; tmp <= *to;
                                    tmp.add_seconds( interval*1000L ) ) {
                epochs.push_back( tmp );
            }
        } else if ( interval == 0 ) {
            return -1;
        } else {
            return 1;
        }
    } else {
        if (   epochs[0]               > this->_last_epoch
            || epochs[epochs.size()-1] < this->_first_epoch )
        {
            std::cerr<<"\n"<<this->filename()<<" says : Too far away";
            std::cerr<<"\nEpoch vector contains from "<< epochs[0].stringify() <<" to " <<epochs[epochs.size()-1].stringify();
            std::cerr<<"\nFirst epoch: "<<this->_first_epoch.stringify()<<" to "<<this->_last_epoch.stringify();
            return 1;
        }
        from = &epochs[0];
        to   = &epochs[epochs.size()-1];
    }
    return 0;
}

/**
 *  \param[in] ifrom  Starting epoch; if not set it will be equal to the first
 *                    epoch in the IONEX file. If it is prior to the first epoch
 *                    in the file, it will be adjusted.
 *  \param[in] ito    Ending epoch; if not set it will be equal to the last
 *                    epoch in the IONEX file. If it is past the last epoch
 *                    in the file, it will be adjusted.
 *  \param[in] interval The time step with which to extract the TEC values. If
 *                    set to '0', it will be set equal to the interval in the
 *                    IONEX file. The value denotes (integer) seconds.
 */
std::vector<std::vector<double>>
ionex::interpolate(const std::vector<std::pair<ionex_grd_type,ionex_grd_type>>& points,
                   std::vector<datetime_ms>& epochs,
                   datetime_ms* ifrom,
                   datetime_ms* ito,
                   int interval
                  )
{
    int status = this->parse_epoch_arguments(epochs, ifrom, ito, interval);
    if ( status > 0 ) {
#ifdef DEBUG
        std::cerr<<"\n[DEBUG] Failed to resolve interpolation epochs.";
#endif
        throw std::runtime_error
            ("ionex::interpolate() -> failed to resolve / unmatched epochs.");
    }

    // we must provide an epoch vector
    // this vector, will hold all epochs recorded in the IONEX file.
    std::vector<datetime_ms> epoch_vector_1;
    epoch_vector_1.reserve( this->_maps_in_file );

    // we must also provide a vector of vectors, where
    // vec[i][j] is the tec value for station #i at epoch #j
    std::vector<std::vector<int>> tec_vals_1 ( points.size() );
    for (std::size_t i=0; i<tec_vals_1.size(); ++i) {
        tec_vals_1[i].reserve( epoch_vector_1.capacity() );
    }

    // get the tec/epoch values that are recorded in the IONEX file, for
    // the points in the list; if we are going to interpolate, it's better to
    // collect tec priori to a after the first/last dates.
    datetime_ms __from { *ifrom };
    datetime_ms __to   { *ito   };
    if ( status == 0 ) {
        long __two_hours = 1000L * 2 * 3600L;
        __from.remove_seconds( __two_hours );
        __to.add_seconds( __two_hours );
    }
    if ( this->get_tec_at(points, epoch_vector_1, tec_vals_1, &__from, &__to) ) {
#ifdef DEBUG
        std::cerr<<"\n[DEBUG] Failed to read te/epochs from IONEX.";
#endif
        throw std::runtime_error
            ("ionex::interpolate() -> failed to read tecs/epochs.");
    }

    // Sweet! If status < 0, then no interpolation is needed
    if ( status < 0 ) {
        epochs = std::move(epoch_vector_1);
        std::vector<std::vector<double>> tecs ( tec_vals_1.size(),
                                    std::vector<double>(epochs.size()) );
        for (std::size_t i=0; i<tec_vals_1.size(); ++i) {
            std::transform(tec_vals_1[i].cbegin(),
                           tec_vals_1[i].cend(),
                           tecs[i].begin(),
                           [&](int t){ return static_cast<double>(t) *
                            static_cast<double>(std::pow(10, this->_exp)); }
                          );
        }
        return tecs;
    }
    
    // we need to interpolate in time! We need to find the TEC values for
    // all epochs in the epochs vector.
    // a new vector of vectors to hold the interpolated TEC values (per station)
    std::vector<std::vector<double>> tec_vals_2 ( points.size(),
                                  std::vector<double>(epochs.capacity(), 9999) );

    // remove garbabe values(dates) from the end of epoch_vector_1
    auto time_prev = epoch_vector_1.cbegin();
    auto time_next = time_prev;
    std::size_t i(0), j(0), k(0);
    double coefi, coefj;
    for (auto cur_time  = epochs.cbegin(); cur_time < epochs.cend(); ++cur_time ) {

        time_next = std::upper_bound(epoch_vector_1.cbegin(),
                                     epoch_vector_1.cend(),
                                     *cur_time);
        if ( time_next == epoch_vector_1.cend() ) {
            time_next = time_prev + 1;
        } else {
            time_prev = time_next - 1;
        }

        i = std::distance(epoch_vector_1.cbegin(), time_prev);
        j = std::distance(epoch_vector_1.cbegin(), time_next);

        k = std::distance(epochs.cbegin(), cur_time);
        if ( time_prev != time_next ) {
            auto s1 = static_cast<double>( time_next->delta_sec(*cur_time).as_underlying_type() );
            auto s2 = static_cast<double>( time_next->delta_sec(*time_prev).as_underlying_type() );
            coefi = s1/s2;
            s1 = static_cast<double>( cur_time->delta_sec(*time_prev).as_underlying_type() );
            s2 = static_cast<double>( time_next->delta_sec(*time_prev).as_underlying_type() );
            coefj = s1/s2;
        } else {
            coefi = 1.0f;
            coefj = 0.0f;
        } // FIXME missing values 99999 !!!!!!!!!!!!!!!!!
        for ( std::size_t point=0 ; point<points.size() ; ++point ) {
            tec_vals_2[point][k] = coefi*tec_vals_1[point][i] 
                                 + coefj*tec_vals_1[point][j];
        }
    }

    return tec_vals_2;
}
