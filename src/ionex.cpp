#include <cassert>
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <cstring>
#include "ionex.hpp"
#include "grid.hpp"

#ifdef DEBUG
    #include <iostream>
#endif

using ngpt::ionex;
using ngpt::ionex_grd_type;

typedef std::tuple<std::size_t, ionex_grd_type,
                   std::size_t, ionex_grd_type,
                   std::size_t, ionex_grd_type,
                   std::size_t, ionex_grd_type>  tec_cell;

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
            if ( _map_dimension != 2 ) {
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

/// Compute # of maps for constant height.
///
/// \warning This function uses the fact that the (latitude) grid is given
/// with a precision of 1e-1 degrees.
///
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

/// Compute how many lines should be read off from a const-latitude map instant
///
/// \warning This function uses the fact that the (longtitude) grid is given
/// with a precision of 1e-1 degrees.
///
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

/// Skip a whole map. The buffer should be placed in a position such that the
/// first line to be read is: "LAT/LON1/LON2/H". After the function has
/// returned, the buffer should be placed after the line: "END OF TEC MAP"
int
ionex::skip_tec_map()
{
    static char line [MAX_HEADER_CHARS];
    char* c;
    ionex_grd_type lat = _lat1;
    ionex_grd_type ltmp;
    std::size_t num_of_tec_lines = this->longtitude_lines();
    bool ascending ( _lat2 > _lat1 ? true : false );
    
    while ( _istream.getline(line, MAX_HEADER_CHARS) )
    {
        // next line should be 'LAT/LON1/LON2/DLON/H'
        if ( std::strncmp(line+60, "LAT/LON1/LON2/DLON/H", 20) )
        {
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
        if ( (int)(ltmp*100) != (int)(lat*100) ) {
#ifdef DEBUG
            std::cerr<<"\n[DEBUG]What the fuck man! Read invalid latitude.";
            std::cerr<<"\n       Expected: "<<lat<<", found: "<<ltmp;
            throw std::runtime_error("ionex::skip_tec_map() -> Invalid latitude");
#endif
            return 1;
        } else {
#ifdef DEBUG
//            std::cout<<"\n[DEBUG] Reading and ignoring map for lat = "<<ltmp;
#endif
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

/*
 * Read a TEC map for a given epoch. All values are going to be read into the
 * vector tec_vals, at the order they are read. This means that the vector will
 * be of the form:
\verbatim
    vec[0]   -> lat1, lon1
    vec[1]   -> lat1, lon1+dlon
    vec[2]   -> lat1, lon1+2*dlon
    ...
    vec[n]   -> lat1+dlat,   lon1
    vec[n+1] -> lat1+2*dlat, lon1+dlon
    ...
\endverbatim
 * Must leave the input buffer after the line 'END OF TEC MAP"
 *
 * \warning -# The buffer should be placed in a position such that the next line
 *          to be read is "LAT/LON1/LON2/DLON/H".
 *          -# The size of the vector should be large enough to hold the read
 *          values. The size of the vector will not be modified at any way.
 */
int
ionex::read_tec_map(std::vector<int>& pcv_vals)
{
    static char line[MAX_HEADER_CHARS];

    // stream should definitely be open!
    // TODO move this somewhere else. don't fucking need to always check.
    assert( _istream.is_open() );
    
    std::size_t index = 0;
    
    // how many const latitude maps should we read ?
    std::size_t lat_maps  (this->latitude_maps() );
    // each const-latitude map has how many tec lines ?
    std::size_t lon_lines (this->longtitude_lines() );

    // reset errno
    errno = 0;

    for (std::size_t i=0; i<lat_maps; ++i)
    {
        if ( this->read_latitude_map(lon_lines, pcv_vals, index) ) {
            return 1;
        }
    }
    
    // should now read 'END OF TEC MAP'
    if ( ! _istream.getline(line, MAX_HEADER_CHARS)
        || std::strncmp(line+60, "END OF TEC MAP", 14) )
    {
#ifdef DEBUG
        std::cerr<<"\n[DEBUG] Expected \"END OF TEC MAP\" but found:";
        std::cerr<<"\n        "<<line;
        throw std::runtime_error
            ("ionex::read_tec_map() -> Invalid line");
#endif
            return 1;
    }

    return 0;
}

/*
 *  Read a TEC map (for a given latitude) off from the stream and store the
 *  values in the vec vector, starting at vec[index]. The function will modify
 *  the index value, such that at return it will denote the last index inserted
 *  into the vector plus one.
 * 
 *  \warning -# The buffer should be placed in a position such that the next line
 *           to be read is "LAT/LON1/LON2/DLON/H".
 *           -# Also, always clear errno before calling this!
 *           -# When this function returns, errno should be what is was before
 *           the function call.
 * 
 *  \param[in] num_of_tec_lines The number of lines to read off from a const
 *                              latitude map.
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
    
    // next line should be 'LAT/LON1/LON2/DLON/H'
    if ( !_istream.getline(line, MAX_HEADER_CHARS) 
         || std::strncmp(line+60, "LAT/LON1/LON2/DLON/H", 20) )
    {
#ifdef DEBUG
        std::cerr<<"\n[DEBUG] Error reading TEC map 'LAT/LON1/LON2/DLON/H'";
        std::cerr<<"\n        found line: " << line;
        throw std::runtime_error
        ("ionex::read_map() -> Invalid line");
#endif
        return 1;
    }
    start = line+2;
    lat  = std::strtof(start, &end);
    start += 6;
    lon1 = std::strtof(start, &end);
    start +=6;
    lon2 = std::strtof(start, &end);
    start +=6;
    dlon = std::strtof(start, &end);
    start +=6;
    hgt  = std::strtof(start, &end);
#ifdef DEBUG
    if ( lon1!=_lon1 || lon2!=_lon2 || dlon!=_dlon || hgt != _hgt1 )
    {
        errno = prev_errno;
        std::cerr << "\n[DEBUG] Oh Fuck! this longtitude seems corrupt!";
        std::cerr << "\n        line: " << line;
        std::string lat_str = std::to_string(lat);
        throw std::runtime_error("ionex::read_map() -> Invalid line ("+lat_str+")");
    }
#endif

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
    if ( errno )
    {
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

ionex_grd_type
bilinear_interpolate(float longtitude, float latitude,
                     std::vector<int>& tec_vals,
                     tec_cell&         cell,
                     std::size_t       lon_grid_size)
{
    // resolve the tuple
    std::size_t x0_idx = std::get<0>( cell );
    std::size_t y0_idx = std::get<4>( cell );
#ifdef DEBUG
    std::size_t x1_idx = std::get<2>( cell );
    std::size_t y1_idx = std::get<6>( cell );
    assert( x1_idx == x0_idx + 1 );
    assert( y1_idx == y0_idx + 1 );
#else
    std::size_t x1_idx = x0_idx + 1;
    std::size_t y1_idx = y0_idx + 1;
#endif

    // do not forget the scale factor !!
    ionex_grd_type x0 = std::get<1>(cell)/100.0f;
    ionex_grd_type x1 = std::get<3>(cell)/100.0f;
    ionex_grd_type y0 = std::get<5>(cell)/100.0f;
    ionex_grd_type y1 = std::get<7>(cell)/100.0f;

    // get the tec values at the indexes we want
    std::size_t low_left = y0_idx * lon_grid_size + x0_idx;
#ifdef DEBUG
    assert( low_left+1+lon_grid_size < tec_vals.size() );
#endif
    ionex_grd_type f00 = static_cast<ionex_grd_type>(tec_vals[low_left]);
    ionex_grd_type f01 = static_cast<ionex_grd_type>(tec_vals[low_left+lon_grid_size]);
    ionex_grd_type f10 = static_cast<ionex_grd_type>(tec_vals[low_left+1]);
    ionex_grd_type f11 = static_cast<ionex_grd_type>(tec_vals[low_left+1+lon_grid_size]);

#ifdef DEBUG
//std::cout <<"\nInterpolating at: ("<<longtitude<<", "<<latitude<<")";
//std::cout <<"\n("<<x0_idx<<", "<<y1_idx<<") ----   ("<<x1_idx<<", "<<y1_idx<<")";
//std::cout <<"\n  |                  |";
//std::cout <<"\n  |                  |";
//std::cout <<"\n  |                  |";
//std::cout <<"\n("<<x0_idx<<", "<<y0_idx<<") ----   ("<<x1_idx<<", "<<y0_idx<<")";
//std::cout<<"\n";
//std::cout <<"\nInterpolating at: ("<<longtitude<<", "<<latitude<<")";
//std::cout <<"\n("<<x0<<", "<<y1<<")="<<f01<<" ----   ("<<x1<<", "<<y1<<")="<<f11;
//std::cout <<"\n  |                  |";
//std::cout <<"\n  |                  |";
//std::cout <<"\n  |                  |";
//std::cout <<"\n("<<x0<<", "<<y0<<")="<<f00<<" ----   ("<<x1<<", "<<y0<<")="<<f10;
//std::cout<<"\n";
#endif

    // bilinear interpolation, see
    // https://en.wikipedia.org/wiki/Bilinear_interpolation
    ionex_grd_type x     { longtitude };
    ionex_grd_type y     { latitude };
    ionex_grd_type denom { (x1-x0)*(y1-y0) };

    return  ((x1-x)*(y1-y)/denom)*f00
           +((x-x0)*(y1-y)/denom)*f10
           +((x1-x)*(y-y0)/denom)*f01
           +((x-x0)*(y-y0)/denom)*f11;
}

/* 
 *  \param[in] points A vector of coordinates of type (longtitude, latitude)
 *                    in (decimal) degrees (two decimal places are considered)
 *  \param[in] ifrom  Starting epoch; if not set it will be equal to the first
 *                    epoch in the IONEX file. If it is prior to the first epoch
 *                    in the file, it will be adjusted.
 *  \param[in] ito    Ending epoch; if not set it will be equal to the last
 *                    epoch in the IONEX file. If it is past the last epoch
 *                    in the file, it will be adjusted.
 *  \param[in] interval The time step with which to extract the TEC values. If
 *                    set to '0', it will be set equal to the interval in the
 *                    IONEX file. The value denotes (integer) seconds.
 * 
 * \TODO Fuck this throws in a million places. Fucking do something!
 */
std::vector<int>
ionex::get_tec_at(const std::vector<std::pair<ionex_grd_type,ionex_grd_type>>& points,
                datetime_ms* ifrom, datetime_ms* ito, int interval)
{
    datetime_ms from ( ifrom ? (*ifrom) : _first_epoch );
    datetime_ms to   ( ito   ? (*ito)   : _last_epoch  );
    if (interval < 0 ) {
#ifdef DEBUG
        std::cerr<<"\n[DEBUG] Fuck off setting interval to 0";
        interval = 0;
#endif
    }

    if ( from > to ) {
        throw std::runtime_error
            ("ionex::get_tec_at() -> Invalid epochs passed!");
    }

    // TODO if from == to

    if ( from < _first_epoch ) {
#ifdef DEBUG
        std::cerr<< "\n[DEBUG] First epoch is beyond the first epoch in file; adjusted.";
#endif
        from = _first_epoch;
    }
    if ( to > _last_epoch ) {
#ifdef DEBUG
        std::cerr<< "\n[DEBUG] Ending epoch is beyond the last epoch in file; adjusted.";
#endif
        to = _last_epoch;
    }

    // for every point we want, we must find an index for it (within the grid)
    // so that we can extract it's value. Let's make a 2D grid. For ease, let's
    // set the grid points to long (instead of ints) so that e.g. lat=37.5
    // lon=23.7 will be 3750 and 2370; i.e. use of factor of 100.
    int factor (100);
#ifdef DEBUG
    GridSkeleton<long,  true, Grid_Dimension::TwoDim> 
        grid(_lon1*factor, _lon2*factor, _dlon*factor,
             _lat1*factor, _lat2*factor, _dlat*factor);
#else
    GridSkeleton<long, false, Grid_Dimension::TwoDim> 
        grid(_lon1*factor, _lon2*factor, _dlon*factor,
             _lat1*factor, _lat2*factor, _dlat*factor);
#endif

    // for each point in the vector, we are going to need the indexes of the
    // surounding nodes (so that we extract these values and interpolate).
    std::vector<tec_cell> indexes;
    indexes.reserve( points.size() );
    for ( auto const& i : points ) {
        indexes.emplace_back(grid.neighbor_nodes( 
                                 i.first*factor, i.second*factor ));
    }

    // get me a vector large enough to hold a whole map
    std::vector<int> tec_map (grid.size(), 0);

    // skip all maps until we find the one we are interested in
    char line[MAX_HEADER_CHARS];
    _istream.seekg(_end_of_head, std::ios::beg);
    int map_num = 1;
    char* c;
    
    if ( !_istream.getline(line, MAX_HEADER_CHARS) )
    {
#ifdef DEBUG
        std::cerr<<"\n[DEBUG] WTF? Failed to read input line!";
#endif
        throw std::runtime_error("ionex::get_tec_at() -> Invalid line!");
    }

    datetime_ms cur_dt = _first_epoch;
    while ( cur_dt < from ) {
        std::cout<<"\nNeed to get to the first epoch";
        if ( _istream && std::strncmp(line+60, "START OF TEC MAP", 16) ) {
#ifdef DEBUG
            std::cerr<<"\n[DEBUG] Expected line \"START OF TEC MAP\", found:";
            std::cerr<<"\n        "<<line;
#endif
            throw std::runtime_error("ionex::get_tec_at() -> Invalid line!");
        }
        *(line+6) = '\0';
        assert( map_num == std::strtol(line, &c, 10) );
        if ( ! _istream.getline(line, MAX_HEADER_CHARS)
            || std::strncmp(line+60, "EPOCH OF CURRENT MAP", 20) 
            || _read_ionex_datetime_(line, &cur_dt) )
        {
#ifdef DEBUG
            std::cerr<<"\n[DEBUG] Expected line \"EPOCH OF CURRENT MAP\", found:";
            std::cerr<<"\n        "<<line;
#endif
            throw std::runtime_error("ionex::get_tec_at() -> Invalid line!");
        }
        // skip all the fucking shit that follow
        if ( this->skip_tec_map() ) {  
            throw std::runtime_error("ionex::get_tec_at() -> Invalid line!");
        }
        _istream.getline(line, MAX_HEADER_CHARS);
        ++map_num;
    }

    while ( cur_dt < to ) {
        if ( !_istream || std::strncmp(line+60, "START OF TEC MAP", 16) ) {
#ifdef DEBUG
            std::cerr<<"\n[DEBUG] Expected line \"START OF TEC MAP\", found:";
            std::cerr<<"\n        "<<line;
#endif
            throw std::runtime_error("ionex::get_tec_at() -> Invalid line!");
        }
        
        std::cout<<"\nreading map: " << line;
        *(line+6) = '\0';
        assert( map_num == std::strtol(line, &c, 10) );
        std::cout<<"\nmap number="<<map_num;
        if ( ! _istream.getline(line, MAX_HEADER_CHARS)
            || std::strncmp(line+60, "EPOCH OF CURRENT MAP", 20) 
            || _read_ionex_datetime_(line, &cur_dt) )
        {
#ifdef DEBUG
            std::cerr<<"\n[DEBUG] Expected line \"EPOCH OF CURRENT MAP\", found:";
            std::cerr<<"\n        "<<line;
#endif
            throw std::runtime_error("ionex::get_tec_at() -> Invalid line!");
        }
        
        // read the map into memmory
        if ( this->read_tec_map(tec_map) ) {
#ifdef DEBUG
            std::cerr<<"\n[DEBUG] Failed reading map nr "<<map_num;
#endif
            throw std::runtime_error("ionex::get_tec_at() -> failed reading maps.");
        }

        // ok. we got the map and we need to extract the cells for all points
        //TODO
        std::size_t j = 0;
        for (const auto& p : points) {
            bilinear_interpolate(p.first, p.second, tec_map, indexes[j], 
                                grid.x_axis_pts());
            ++j;
        }
        _istream.getline(line, MAX_HEADER_CHARS);
        ++map_num;
    }

    return std::vector<int> (5,0);   
}
