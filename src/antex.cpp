#include <cassert>
#include <stdlib.h>
#include <cmath>
#include <stdexcept>
#include <string.h>
#include "antex.hpp"
// #include "obstype.hpp"

#ifdef DEBUG
    #include <iostream>
#endif

using ngpt::antex;

/// According to the ANTEX definition, the antenna serial number is 20 chars.
constexpr std::size_t ANTEX_SERIAL_CHARS { 20 };

static_assert(ANTEX_SERIAL_CHARS == ngpt::antenna_details::antenna_serial_max_chars,
            "Size of serial number  different in antex.cpp and antenna.hpp");

/// No header line can have more than 80 chars. However, there are cases when
/// they  exceed this limit, just a bit ...
constexpr int MAX_HEADER_CHARS { 85 };

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

/**
 *  Max grid line chars; i.e. the maximum number of chars any pcv line can
 *  hold. Typical 'ZEN1 / ZEN2 / DZEN' = '0.0  90.0   5.0' and the format is
 *  mF8.2 which is 19*8 = 152 + (~8 starting chars) = 160.
 *  Let's make enough space for DZEN = 3 -> max chars = 31*8 + 10 = 258
 * 
 *  \warning When using this constant always assert something like
 *           8*(std::size_t)((zen2-zen1)/dzen) < MAX_GRID_CHARS-10
 */
constexpr std::size_t MAX_GRID_CHARS { 258 };

/**
 *  Resolve a frequency type as recorded in an antex file to a valid
 *  ObservatioType.
 */
ngpt::observation_type antex2obstype(const char* s)
{
    ngpt::satellite_system ss { ngpt::char_to_satsys( *s ) };
    int freq { std::stoi(s+1, nullptr) };
    return ngpt::observation_type(ss, ngpt::observable_type::carrier_phase,
                                 freq, '?');
}

/**
 *  \details Antex Constructor, using an antex filename. The constructor will
 *           initialize (set) the _filename attribute and also (try to)
 *           open the input stream (i.e. _istream).
 *           If the file is successefuly opened, the constructor will read
 *           the ANTEX header and assign info.
 */
antex::antex(const char *filename)
    : _filename   {filename},
      _istream    {filename, std::ios_base::in},
      _satsys     {ngpt::satellite_system::mixed},
      _version    {ngpt::antex::ATX_VERSION::v14},
      _type       {ngpt::antex::PCV_TYPE::Absolute},
      _refant     {},
      _end_of_head{0}
{
    if ( !_istream.is_open() ) {
        throw std::runtime_error{
        "Cannot open antex file: "+std::string(filename)};
    }
#ifdef DEBUG
    try {
        this->read_header();
    } catch (std::exception& e) {
        std::cerr << "\n[ERROR] Failed to read header for antex file: "
                  << "\"" <<  filename << "\"";
        std::cerr << e.what();
        if ( _istream.is_open() ) {
            _istream.close();
        }
    }
#endif
    if ( this->read_header() ) {
          if ( _istream.is_open() ) {
              _istream.close();
        }
    }
}

/**
 *  \details Read an Antex (instance) header. The format of the header should
 *           closely follow the antex format specification (version 1.4).
 *           This function will set the instance fields:
 *           - _version (should be 1.4),
 *           - _satsys,
 *           - _type,
 *           - _refant and
 *           - __end_of_head
 *           The function will exit after reading a header line, ending with
 *           the (sub)string 'END OF HEADER'.
 * 
 *  \throw   In DEBUG mode, will throw a std::runtime_error in case any one of 
 *           the required instance fields can not be resolved, or the 
 *           'END OF HEADER' cannot be found after MAX_HEADER_LINES are read.
 *           If not in DEBUG mode, no exception is thrown (by this function).
 * 
 *  \warning 
 *           - The instance's input steam (i.e. _istream) should be open and 
 *           valid.
 *           - Note that the function expects that no header line contains more
 *           than MAX_HEADER_CHARS chars.
 *           - If the header is not read correctly, then the (opened) file
 *           buffer will be closed.
 * 
 *  \return  Anything other than '0' is an error.
 * 
 *  \reference https://igscb.jpl.nasa.gov/igscb/station/general/antex14.txt
 */
int antex::read_header()
{
    char line[MAX_HEADER_CHARS];

    // The stream should be open by now!
    assert(this->_istream.is_open());

    // Go to the top of the file.
    _istream.seekg(0);

    // Read the first line. Get version and system.
    // ----------------------------------------------------
    _istream.getline(line, MAX_HEADER_CHARS);
    // strtod will keep on reading untill a non-valid
    // char is read. Fuck this, lets split the string
    // so that it only reads the valid chars (for version).
    *(line+15) = '\0';
    float fvers = std::strtod(line,nullptr);
    if (std::abs(fvers - 1.4) < .001) {
        this->_version = antex::ATX_VERSION::v14;
    } else if (std::abs(fvers - 1.3) < .001) {
        this->_version = antex::ATX_VERSION::v13;
    } else {
#ifdef DEBUG
        throw std::runtime_error
        ("antex::read_header -> Invalid Antex version.");
#endif
        return 1;
    }
    // Resolve the satellite system.
    try {
        this->_satsys = ngpt::char_to_satsys(line[20]);
    } catch (std::runtime_error& e) {
#ifdef DEBUG
        throw std::runtime_error
        ("antex::read_header -> Invalid Satellite System.");
#endif
        return 1;
    }

    // Read the second line. Get PCV TYPE / REFANT.
    // ----------------------------------------------------
    _istream.getline(line, MAX_HEADER_CHARS);
    if (*line == 'A') {
        this->_type = antex::PCV_TYPE::Absolute;
    } else if (*line == 'R') {
        this->_type = antex::PCV_TYPE::Relative;
    } else {
#ifdef DEBUG
        throw std::runtime_error
        ("antex::read_header -> Invalid PCV type.");
#endif
        return 1;
    }
    // In case of a relative antenna, we also have the
    // REFANT field.
    if (this->_type == antex::PCV_TYPE::Relative) {
        this->_refant = (line+20);
    }

    // Keep on readling lines until 'END OF HEADER'.
    // ----------------------------------------------------
    int dummy_it = 0;
    _istream.getline(line, MAX_HEADER_CHARS);
    while (dummy_it < MAX_HEADER_LINES && 
            strncmp(line+60, "END OF HEADER", eoh_size) ) {
        _istream.getline(line, MAX_HEADER_CHARS);
        dummy_it++;
    }
    if (dummy_it >= MAX_HEADER_LINES) {
#ifdef DEBUG
        throw std::runtime_error
        ("antex::read_header -> Could not find 'END OF HEADER'.");
#endif
        return 1;
    }

    this->_end_of_head = _istream.tellg();

    // All done !
    return 0;
}

/**
 *  \details This function is used to read an antenna calibration info block out
 *           of an Antex instance. After it is handed an input
 *           file stream (i.e. that of an Antex instance), positioned at the
 *           begining of a 'METH / BY / # / DATE' field, it will continue
 *           reading the antenna block untill the 'END OF ANTENNA' field.
 * 
 *  \return  Everything other than 0 denots a failure.
 * 
 *  \todo    Need to also read 'FREQ RMS'
 */
ngpt::antenna_pcv<ngpt::pcv_type> 
ngpt::antex::read_pattern()
{
    using ngpt::pcv_type;

    char line[MAX_HEADER_CHARS];

    // next field is 'METH / BY / # / DATE'
    char clbr[20];
    if (_istream.getline(line, MAX_HEADER_CHARS)
        && !strncmp(line+60, "METH / BY / # / DATE", 20))
    {
        std::memcpy(clbr, line, 20);
    }

    // next field is 'DAZI'
    pcv_type dazi {-1000};
    if (_istream.getline(line, MAX_HEADER_CHARS)
        && !strncmp(line+60, "DAZI", 4)) 
    {
        dazi = std::stof(line+2, nullptr);
    }

    // next field is 'ZEN1 / ZEN2 / DZEN'
    pcv_type zen1,  zen2,  dzen;
    zen1 = zen2 = dzen = -1000;
    if (_istream.getline(line, MAX_HEADER_CHARS)
        && !strncmp(line+60, "ZEN1 / ZEN2 / DZEN", 18))
    {
        zen1 = std::stof(line+2, nullptr);
        zen2 = std::stof(line+8, nullptr);
        dzen = std::stof(line+14, nullptr);
        // see the decleration of MAX_GRID_CHARS for why this is needed.
        assert( 8*(std::size_t)((zen2-zen1)/dzen) < MAX_GRID_CHARS-10  );
    }

    // next field is '# OF FREQUENCIES'
    int num_of_freqs {0};
    if (_istream.getline(line, MAX_HEADER_CHARS)
        && !strncmp(line+60, "# OF FREQUENCIES", 16))
    {
        num_of_freqs = std::stoi(line, nullptr);
    }
  
    // Construct an AntennaPattern
    if (  dazi < -900
       || zen1 < -900
       || zen2 < -900
       || dzen < -900 ) {
        throw std::runtime_error
        ("antex::read_pattern() -> Failed to resolve antenna grid information.");
    }
    
    // Nice, we have all we need to construct an antenna pcv pattern.
    ngpt::antenna_pcv<ngpt::pcv_type> antpat 
                                    {zen1, zen2, dzen, num_of_freqs, dazi};
    // We're gonna use a pointer to each frequency_pcv in the antpat to assign
    // the values for each frequency
    ngpt::frequency_pcv<ngpt::pcv_type>* freq_pcv_ptr;

    // From here up untill the block 'START OF FREQUENCY' there can be a number
    // of optional fields.
    while ( _istream.getline(line, MAX_HEADER_CHARS)
          && strncmp(line+60, "START OF FREQUENCY", 18) )
    {
#ifdef DEBUG
        std::cout << "\n[DEBUG] Optional line skiped: " << line;
#endif
    }
    // make sure we're not at EOF or anything funny happened ...
    if ( !_istream.good() ) {
        throw std::runtime_error
        ("antex::read_pattern -> Could not find 'START OF FREQUENCY'.");
    }

    // blah ... blah ... blah ... comments and shit read. now we should read
    // the pattern for each frequency
    char tmp[11];
    char g_line[MAX_GRID_CHARS];
    std::size_t vals_to_read { static_cast<std::size_t>((zen2 - zen1) / dzen) + 1 };
    int num_of_azi_lines     { static_cast<int>(( antenna_pcv_details::azi2 
                               - antenna_pcv_details::azi1 ) / dazi) + 1 };
    
    for (int i=0; i<num_of_freqs; i++) {

        // make sure we are at the START OF FREQUENCY line
        if ( strncmp(line+60, "START OF FREQUENCY", 18) ) {
#ifdef DEBUG
            std::cerr << "\n[ERROR] Expected 'START OF FREQUENCY' but found:"
                      << line;
#endif
            throw  std::runtime_error
            ("antex::read_pattern -> Error reading frequency pcv.");
        }

        freq_pcv_ptr = &antpat.freq_pcv_pattern( i );

        // resolve and set the frquency/sat-sys
        ngpt::observation_type ot { antex2obstype(line+3) };
        freq_pcv_ptr->type() = ot;

        // next field is 'NORTH / EAST / UP'
        if (_istream.getline(line, MAX_HEADER_CHARS) 
            && !strncmp(line+60, "NORTH / EAST / UP", 17))
        {
            tmp[10] = '\0';
            std::memcpy(tmp, line, 10);
            freq_pcv_ptr->north() = std::stod(tmp, nullptr);
            std::memcpy(tmp, line+10, 10);
            freq_pcv_ptr->east()  = std::stod(tmp, nullptr);
            std::memcpy(tmp, line+20, 10);
            freq_pcv_ptr->up()    = std::stod(tmp, nullptr);
        }

        // read 'NOAZI' grid values
        std::vector<ngpt::pcv_type>* nav = &freq_pcv_ptr->no_azi_vector();
        if (_istream.getline(g_line, MAX_GRID_CHARS)
            && !strncmp(g_line, "   NOAZI", 8)) {
            char* lptr  = g_line + 8;
            char ntc    = '\0';
            //auto nav_it = nav->begin(); /* iterator to std::vector<ngpt::pcv_type> */
            for (std::size_t j=0; j<vals_to_read; ++j) {
                // make sure we only transform 8 chars to float !
                std::swap(lptr[8], ntc);
                nav->emplace_back( std::stod(lptr, nullptr) );
                std::swap(ntc, lptr[8]);
                lptr += 8;
            }
        } else {
            throw std::runtime_error
            ("antex::read_pattern -> Could not find 'NOAZI' grid.");
        }

        // read azimouth-dependent grid values
        std::vector<ngpt::pcv_type>* av = &freq_pcv_ptr->azi_vector();
        // initialize all values in the az-pcv vector to 0
        av->assign(antpat.azi_grid_pts(), .0);
        if ( dazi != 0 ) {
            for (int j=0; j<num_of_azi_lines; ++j) {
                if ( !_istream.getline(g_line, MAX_GRID_CHARS) ) {
                    throw std::runtime_error
                    ("antex::read_pattern -> Failed to read 'AZI' grid (1).");
                }
                pcv_type this_azi ( std::stod(g_line, nullptr) );
                if ( std::abs( this_azi - j*dazi + antenna_pcv_details::azi1 ) > .001 ) {
                    throw std::runtime_error
                    ("antex::read_pattern -> Failed to read 'AZI' grid (2).");
                }
                // find the right index for this azimouth in the azi_grid
                std::size_t index = antpat.azi_grid_pts() 
                                  - ((this_azi - antenna_pcv_details::azi1)/dazi)
                                  * vals_to_read - vals_to_read;
                auto av_it = av->begin() + index;
                char* lptr = g_line + 8;
                char ntc   = '\0';
                for (std::size_t k=0; k<vals_to_read; ++k) {
                    // make sure we only transform 8 chars to float !
                    std::swap(lptr[8], ntc);
                    *av_it = std::stod(lptr, nullptr);
                    std::swap(ntc, lptr[8]);
                    lptr += 8;
                    ++av_it;
#ifdef DEBUG
                    // this check is only for debuging. Do not use in production
                    // mode.
                    if ( (std::size_t)std::distance(av->begin(), av_it) 
                            > antpat.azi_grid_pts() ) {
                        int num = std::distance(av->begin(), av_it);
                        std::string msg ( "(reached" 
                                          + std::to_string(num) 
                                          + "/"
                                          + std::to_string(antpat.azi_grid_pts())
                                          + ")" );
                        throw std::runtime_error
                        ("WTF? Reading more azi-grid values than expected " + msg);
                    }
#endif
                }
            }
        }

        // should now see the 'END OF FREQUENCY' marker
        // since we're here, also read the next line ...
        if (   !_istream.getline(line, MAX_HEADER_CHARS)
            || strncmp(line+60, "END OF FREQUENCY", 16) 
            || !_istream.getline(line, MAX_HEADER_CHARS) ) {
            throw  std::runtime_error
            ("antex::read_pattern -> Error reading frequency pcv.");
        }
    }

    // all done
#ifdef DEBUG
    std::cout << "\n[DEBUG] Anntenna Pattern read ok!";
    //std::cout << "\n " << north << east << up << val;
#endif
    return antpat;
}

/**
 *  \details This is only a utility function. After it is handed an input
 *           file stream (i.e. that of an Antex instance), positioned at the
 *           begining of a 'METH / BY / # / DATE' field, it will continue
 *           reading the antenna block untill the 'END OF ANTENNA' field.
 * 
 *  \return  Everything other than 0 denots a failure.
 * 
 *  \todo    Need to also read 'FREQ RMS'
 */ 
int __skip_rest_of_antenna__(std::ifstream& fin)
{
    static char line[MAX_HEADER_CHARS];
    static char grid_line[MAX_GRID_CHARS];

    // next field is 'METH / BY / # / DATE'
    if (!fin.getline(line, MAX_HEADER_CHARS)
        || strncmp(line+60, "METH / BY / # / DATE", 20))
    {
        return 1;
    }

    // next field is 'DAZI'
    if (!fin.getline(line, MAX_HEADER_CHARS)
        || strncmp(line+60, "DAZI", 4)) 
    {
        return 2;
    }
    // if DAZI > 0 we will need to read the azimouth-dependent grid.
    float dazi { std::stof(line+2, nullptr) };

    // next field is 'ZEN1 / ZEN2 / DZEN'
    if (!fin.getline(line, MAX_HEADER_CHARS)
        || strncmp(line+60, "ZEN1 / ZEN2 / DZEN", 18))
    {
        return 3;
    }
    float zen1 { std::stof(line+2, nullptr) };
    float zen2 { std::stof(line+8, nullptr) };
    float dzen { std::stof(line+14, nullptr) };
    // see the decleration of MAX_GRID_CHARS for why this is needed.
    assert( 8*(std::size_t)((zen2-zen1)/dzen) < MAX_GRID_CHARS-10  );

    // next field is '# OF FREQUENCIES'
    if (!fin.getline(line, MAX_HEADER_CHARS)
        || strncmp(line+60, "# OF FREQUENCIES", 16))
    {
        return 4;
    }
    int num_of_freqs { std::stoi(line, nullptr) };

    // loop for all frequencies ...
    while (num_of_freqs-- > 0) {

        // next required field is 'START OF FREQUENCY' but there can be several
        // other optional fields inbetween.
        fin.getline(line, MAX_HEADER_CHARS);
        while ( strncmp(line+60, "START OF FREQUENCY", 18) ) {
            if (!fin.getline(line, MAX_HEADER_CHARS)) {
                return 5;
            }
        }

        // next field is 'NORTH / EAST / UP'
        if (!fin.getline(line, MAX_HEADER_CHARS)
            || strncmp(line+60, "NORTH / EAST / UP", 17)) 
        {
            return 6;
        }

        // reading the NOAZI values ...
        if (!fin.getline(grid_line, MAX_GRID_CHARS)
            || strncmp(grid_line+3, "NOAZI", 5)) 
        {
            return 7;
        }

        // reading azimuth-dependent values ...
        float azi { .0 };
        if (dazi > .0e0) {
            while (azi <= 360.0 && fin.getline(grid_line, MAX_GRID_CHARS)) {
                azi += dazi;
            }
        }

        // next field is 'END OF FREQUENCY'
        if (!fin.getline(line, MAX_HEADER_CHARS)
            || strncmp(line+60, "END OF FREQUENCY", 16))
        {
            return 9;
        }

    } // end reading all frequencies.

    // Fuck shit !! There could be pcv rms values hidding here ....
    fin.getline(line, MAX_HEADER_CHARS);
    while ( !strncmp(line+60, "START OF FREQ RMS", 17) ) {
        // next field is 'NORTH / EAST / UP'
        if (!fin.getline(line, MAX_HEADER_CHARS)
            || strncmp(line+60, "NORTH / EAST / UP", 17)) 
        {
            return 56;
        }

        // reading the NOAZI values ...
        if (!fin.getline(grid_line, MAX_GRID_CHARS)
            || strncmp(grid_line+3, "NOAZI", 5)) 
        {
            return 57;
        }

        // reading azimuth-dependent values ...
        float azi { .0 };
        if (dazi > .0e0) {
            while (azi <= 360.0 && fin.getline(grid_line, MAX_GRID_CHARS)) {
                azi += dazi;
            }
        }

        // next field is 'END OF FREQUENCY'
        if (!fin.getline(line, MAX_HEADER_CHARS)
            || strncmp(line+60, "END OF FREQ RMS", 15))
        {
            return 59;
        }

        // read next line
        if ( !fin.getline(line, MAX_HEADER_CHARS) ) {
            return 50;
        }
    }

    // all done! next field is 'END OF ANTENNA'
    if ( strncmp(line+60, "END OF ANTENNA", 14) )
    {
#ifdef DEBUG
        std::cerr << "\n(details) Expected \"END OF ANTENNA\", but found:\n"
                  << line;
#endif
        return 10;
    }

    return 0;
}

/// Check if a given string is empty, i.e. only holds whitespaces
inline
bool __string_is_empty__(const char* c, std::size_t max_chars)
noexcept
{
    for (std::size_t i=0; i<max_chars; i++)
        if ( *(c+i) != ' ' )
            return false;
    return true;
}

/**
 *  Search through the ANTEX file (i.e. this instance's buffer) to find
 *  a specific antenna. If the antenna is indeed found, the file buffer is
 *  set at the record line "TYPE / SERIAL NO" of the requested antenna.
 *  Note that the function will try to match the antenna based on the model,
 *  radome and serial number. The model+radome must be found exactly as
 *  recorded in the antenna instance. For the serial (if not matched exactly),
 *  the function will return a 'generic' antenna, i.e. with a serial of
 *  20 whitespaces. The ANTEX format specifications state that a blank serial 
 *  number match all representatives.
 * 
 *  \note  This function expects that the antenna patterns are sorted as
 *         documented by IGS, i.e.
 \verbatim

RECEIVER ANTENNAS:
--> sorted by antenna code
    --> sorted by radome code (NONE -> other radome types)
            --> sorted by 'SERIAL NO'

 \endverbatim
 *
 *  \return An integer denoting the exit status.
 *  Integer | Status
 *  --------|----------------------------------------------
 *       -1 | ANTEX format error; something went wrong while reading.
 *        0 | Success; antenna found.
 *        1 | Antenna could not be found.
 * 
 *  \warning 
 *  \todo consider serial numbers.
 *
 */
int antex::find_antenna(const antenna& ant)
{
    using ngpt::antenna;

    char line[MAX_HEADER_CHARS];

    /// See the definition of eoh_size for why the following is needed.
#ifdef __clang__
    const     std::size_t soa_size { std::strlen("START OF ANTENNA") };
#else
    constexpr std::size_t soa_size { std::strlen("START OF ANTENNA") };
#endif

    std::size_t antennas_read { 0 };
    antenna     t_ant;
    int         status;
    pos_type    best_match { 0 }; /* if more than one types match, this marks the
                                     best match depending on serial nr */

    // The stream should be open by now!
    assert(this->_istream.is_open());

    // Go to the end of header.
    _istream.seekg(_end_of_head, std::ios_base::beg);

    // First line after end of header, should be the start of a new antenna.
    if (!_istream.getline(line, MAX_HEADER_CHARS)
        || strncmp(line+60, "START OF ANTENNA", soa_size)) 
    {
        return -1;
    }

    // Read & check antenna type
    if (!_istream.getline(line, MAX_HEADER_CHARS)
        || strncmp(line+60, "TYPE / SERIAL NO    ", 20))
    {
        return -1;
    }

    // Set the antenna type of the current antenna.
    t_ant     = (line);
    
    // note that antenna::operator= only compares type+radome !
    while (t_ant != ant && _istream.good())
    {
        // skip the antenna details ...
        if ( (status = __skip_rest_of_antenna__(_istream)) ) {
#ifdef DEBUG
            std::cerr << "\n\t[DEBUG] ERROR in __skip_rest_of_antenna__; "
                      << "status="<<status;
            std::cerr << "\n\tError in line:\n"
                      << line;
#endif
            return -1;
        }
        antennas_read++;

        // read new antenna ..
        if (!_istream.getline(line, MAX_HEADER_CHARS)
            || strncmp(line+60, "START OF ANTENNA", soa_size))
        {
#ifdef DEBUG
            std::cerr << "\n[DEBUG] ERROR while reading antex file. "
                      << "'START OF ANTENNA' expected";
            std::cerr << "\n\tError in line:\n"
                      << line;
#endif
            return -1;
        }
        // read & check antenna type
        if (!_istream.getline(line, MAX_HEADER_CHARS)
            || strncmp(line+60, "TYPE / SERIAL NO", 16))
        {
#ifdef DEBUG
            std::cerr << "\n[DEBUG] ERROR while reading antex file. "
                      << "'TYPE / SERIAL NO' expected";
            std::cerr << "\n\tError in line:\n"
                      << line;
#endif
            return -1;
        }
        // Set the antenna type.
        t_ant = (line);
    }

    // .. we' ve stoped; either eof encountered ...
    if (_istream.eof()) {
        _istream.clear();
        return 1;
    }

    // .. or we found the antenna !! Cool, but we 're not done matherfucker.
    // We need to see if there is a better match, based on the serial.
    char* serial_ptr = line + 20;
    while ( t_ant == ant && _istream ) {
        if ( ant.compare_serial(serial_ptr)  ) {
            best_match = _istream.tellg();
            break;
        } else if (__string_is_empty__(serial_ptr, ANTEX_SERIAL_CHARS)) {
            best_match = _istream.tellg();
        }
#ifdef DEBUG
        std::cout << "\nSearching for a better match than:"
                  << line;
#endif
        // skip the antenna details ...
        if ( (status = __skip_rest_of_antenna__(_istream)) ) {
#ifdef DEBUG
            std::cerr << "\n\t[DEBUG] ERROR in __skip_rest_of_antenna__; "
                      << "status="<<status;
            std::cerr << "\n\tError in line:\n"
                      << line;
#endif
            return -1;
        }
        antennas_read++;

        // read new antenna ..
        if (!_istream.getline(line, MAX_HEADER_CHARS)
            || strncmp(line+60, "START OF ANTENNA", soa_size))
        {
            // Oh shit, wait a minute! we could have just read the last
            // antenna (line) in the file, so the lat op ended in eof state.
            if ( _istream.eof() ) {
                _istream.clear();
                break;
            } else {
#ifdef DEBUG
                std::cerr << "\n[DEBUG] ERROR while reading antex file (searching for a perfect match)"
                          << "'START OF ANTENNA' expected";
                std::cerr << "\n\tError in line:\n"
                          << line;
#endif
                return -1;
            }
        }
        // read & check antenna type
        if (!_istream.getline(line, MAX_HEADER_CHARS)
            || strncmp(line+60, "TYPE / SERIAL NO", 16))
        {
#ifdef DEBUG
            std::cerr << "\n[DEBUG] ERROR while reading antex file. "
                      << "'TYPE / SERIAL NO' expected";
            std::cerr << "\n\tError in line:\n"
                      << line;
#endif
            return -1;
        }
        // Set the antenna type.
        t_ant = ( line );
        serial_ptr = line + 20;
    }

    // position the buffer at the best match
    if ( !best_match ) {
        return 1;
    } else {
        _istream.seekg( best_match );
    }
    
    printf ("\nANTENNA FOUND AFTER READING %zu ANTENNAS",antennas_read);
    return 0;
}

/// Given a reference natenna \c ref and a vector of antennas (\c ants), search
/// for the best match. The \c integer match_type holds the type of matching, i.e.
/// 0 means no match,
/// 1 means match generic type (i.e. type + radome)
/// 2 means match type, radome and serial number
///
std::vector<ngpt::antenna>::const_iterator
__match_antenna__(const std::vector<ngpt::antenna>& ants, 
                  const ngpt::antenna& ref,
                  int&  match_type)
noexcept
{
    match_type = 0;
    auto it_end   = std::end( ants );
    auto it_match = std::end( ants );

    for ( auto it = std::begin(ants); it != it_end; ++it ) {
        if ( it->is_same( ref ) ) {
            match_type = 2;
            return it;
        }
        if ( *it == ref ) {
            match_type = 1;
            it_match   = it;
        }
    }

    return it_match;
}

int antex::find_antenna(const std::vector<antenna>& ants)
{
    using ngpt::antenna;

    char line[MAX_HEADER_CHARS];

    /// See the definition of eoh_size for why the following is needed.
#ifdef __clang__
    const     std::size_t soa_size { std::strlen("START OF ANTENNA") };
#else
    constexpr std::size_t soa_size { std::strlen("START OF ANTENNA") };
#endif

    std::size_t antennas_read { 0 };
    antenna     t_ant;
    int         status;
    pos_type    best_match { 0 }; // if more than one types match, this marks the
                                  //   best match depending on serial nr

    // this vector will hold pairs of antenna/pos_type, i.e. for each antenna
    // found, the **actual** name of the antenna matched and where it is found
    // in the antex file.
    typedef std::pair<antenna, pos_type> ant_pst;
    typedef std::pair<antenna, ant_pst>  ant_match;
    std::vector<ant_match>               found_antennas;

    // The stream should be open by now!
    assert(this->_istream.is_open());

    // Go to the end of header.
    _istream.seekg(_end_of_head, std::ios_base::beg);

    // First line after end of header, should be the start of a new antenna.
    if (!_istream.getline(line, MAX_HEADER_CHARS)
        || strncmp(line+60, "START OF ANTENNA", soa_size)) 
    {
        return -1;
    }

    // Read & check antenna type
    if (!_istream.getline(line, MAX_HEADER_CHARS)
        || strncmp(line+60, "TYPE / SERIAL NO    ", 20))
    {
        return -1;
    }

    // Set the antenna type of the current antenna.
    t_ant     = (line);
    
    auto        ant_vec_end = std::end( ants );
    auto        it          = ant_vec_end;
    int         match_type;
    std::size_t ants_size   = ants.size();
    std::size_t fnd_size    = found_antennas.size();

    // loop until all antennas found, or the stream goes bad
    while ( fnd_size < ants_size && _istream.good() ) {

        // check if the current antenna matches
        it = __match_antenna__(ants, t_ant, match_type);
        if ( it != ant_vec_end ) { // we have a match ! we should mark this place
// TODO that is a pickle !
        } else { // no match; skip the antenna
            // skip the antenna details ...
            if ( (status = __skip_rest_of_antenna__(_istream)) ) {
#ifdef DEBUG
                std::cerr << "\n\t[DEBUG] ERROR in __skip_rest_of_antenna__; "
                          << "status="<<status;
                std::cerr << "\n\tError in line:\n"
                          << line;
#endif
                return -1;
            }
            antennas_read++;
            // read new antenna ..
            if (!_istream.getline(line, MAX_HEADER_CHARS)
                || strncmp(line+60, "START OF ANTENNA", soa_size))
            {
#ifdef DEBUG
                std::cerr << "\n[DEBUG] ERROR while reading antex file. "
                          << "'START OF ANTENNA' expected";
                std::cerr << "\n\tError in line:\n"
                          << line;
#endif
                return -1;
            }
            // read & check antenna type
            if (!_istream.getline(line, MAX_HEADER_CHARS)
                || strncmp(line+60, "TYPE / SERIAL NO", 16))
            {
#ifdef DEBUG
                std::cerr << "\n[DEBUG] ERROR while reading antex file. "
                          << "'TYPE / SERIAL NO' expected";
                std::cerr << "\n\tError in line:\n"
                          << line;
#endif
                return -1;
            }
            // Set the antenna type.
            t_ant = (line);
        }
    }

    // .. or we found the antenna !! Cool, but we 're not done matherfucker.
    // We need to see if there is a better match, based on the serial.
    char* serial_ptr = line + 20;
    while ( t_ant == ant && _istream ) {
        if ( ant.compare_serial(serial_ptr)  ) {
            best_match = _istream.tellg();
            break;
        } else if (__string_is_empty__(serial_ptr, ANTEX_SERIAL_CHARS)) {
            best_match = _istream.tellg();
        }
#ifdef DEBUG
        std::cout << "\nSearching for a better match than:"
                  << line;
#endif
        // skip the antenna details ...
        if ( (status = __skip_rest_of_antenna__(_istream)) ) {
#ifdef DEBUG
            std::cerr << "\n\t[DEBUG] ERROR in __skip_rest_of_antenna__; "
                      << "status="<<status;
            std::cerr << "\n\tError in line:\n"
                      << line;
#endif
            return -1;
        }
        antennas_read++;

        // read new antenna ..
        if (!_istream.getline(line, MAX_HEADER_CHARS)
            || strncmp(line+60, "START OF ANTENNA", soa_size))
        {
            // Oh shit, wait a minute! we could have just read the last
            // antenna (line) in the file, so the lat op ended in eof state.
            if ( _istream.eof() ) {
                _istream.clear();
                break;
            } else {
#ifdef DEBUG
                std::cerr << "\n[DEBUG] ERROR while reading antex file (searching for a perfect match)"
                          << "'START OF ANTENNA' expected";
                std::cerr << "\n\tError in line:\n"
                          << line;
#endif
                return -1;
            }
        }
        // read & check antenna type
        if (!_istream.getline(line, MAX_HEADER_CHARS)
            || strncmp(line+60, "TYPE / SERIAL NO", 16))
        {
#ifdef DEBUG
            std::cerr << "\n[DEBUG] ERROR while reading antex file. "
                      << "'TYPE / SERIAL NO' expected";
            std::cerr << "\n\tError in line:\n"
                      << line;
#endif
            return -1;
        }
        // Set the antenna type.
        t_ant = ( line );
        serial_ptr = line + 20;
    }

    // position the buffer at the best match
    if ( !best_match ) {
        return 1;
    } else {
        _istream.seekg( best_match );
    }
    
    printf ("\nANTENNA FOUND AFTER READING %zu ANTENNAS",antennas_read);
    return 0;
}
