#include <cassert>
#include <stdlib.h>
#include <cmath>
#include <stdexcept>
#include <string.h>
#include "ionex.hpp"

#ifdef DEBUG
    #include <iostream>
#endif

using ngpt::ionex;

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
    const     std::size_t stm_size { std::strlen("START OF TEC MAP") };
#else
    constexpr std::size_t stm_size { std::strlen("START OF TEC MAP") };
#endif

/// Max header lines.
constexpr int MAX_HEADER_LINES { 1000 };

/* Read an ionex header.
 *
 */
int
ionex::read_header()
{
    char line    [MAX_HEADER_CHARS];
    char sysmodel[4];
    char mapfun  [5];
    char* start, end;

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
    float fvers = std::strtod(line, nullptr);
    if (std::abs(fvers - 1.0) < .001) {
        this->_version = antex::ionex_version::v10;
    } else {
#ifdef DEBUG
        throw std::runtime_error
        ("[DEBUG] ionex::read_header -> Invalid IONEX version.");
#endif
        return 1;
    }
    if ( line[10] != 'I' ) {
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
            _first_epoch = //TODO
        }
        else if ( !strncmp(line+60, "EPOCH OF LAST MAP", 17) )
        {
            _last_epoch = //TODO
        }
        else if ( !strncmp(line+60, "INTERVAL", 8) )
        {
            *(line+6) = '\0';
            _interval = std::stoi(line, nullptr);
        }
        else if ( !strncmp(line+60, "# OF MAPS IN FILE", 17) )
        {
            *(line+6) = '\0';
            _maps_in_file = std::stoi(line, nullptr);
        }
        else if ( !strncmp(line+60, "MAPPING FUNCTION", 16) )
        {
            std::memcpy(mapfun, line+2, 4);
        }
        else if ( !strncmp(line+60, "ELEVATION CUTOFF", 16) )
        {
            *(line+10) = '\0';
            _min_elevation = std::stof(line, nullptr);
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
            _base_radius = std::stod(line, nullptr);
        }
        else if ( !strncmp(line+60, "MAP DIMENSION", 13) )
        {
            *(line+6) = '\0';
            _map_dimension = std::stoi(line, nullptr);
        }
        else if ( !strncmp(line+60, "HGT1 / HGT2 / DHGT", 18) )
        {
            start = line+2;
            _hgt1 = std::strtod(start, &end);
            start = end;
            _hgt2 = std::strtod(start, &end);
            start = end;
            _dhgt = std::strtod(start, &end);
            if ( errno == ERANGE )
            {
#ifdef DEBUG
                throw std::runtime_error
                ("[DEBUG] ionex::read_header -> Invalid IONEX height grid info");
#endif
                return 1;
            }
        }
        else if ( !strncmp(line+60, "LAT1 / LAT2 / DLAT", 18) )
        {
            start = line+2;
            _lat1 = std::strtod(start, &end);
            start = end;
            _lat2 = std::strtod(start, &end);
            start = end;
            _dlat = std::strtod(start, &end);
            if ( errno == ERANGE )
            {
#ifdef DEBUG
                throw std::runtime_error
                ("[DEBUG] ionex::read_header -> Invalid IONEX latitude grid info");
#endif
                return 1;
            }
        }
        else if ( !strncmp(line+60, "LON1 / LON2 / DLON", 18) )
        {
            start = line+2;
            _lon1 = std::strtod(start, &end);
            start = end;
            _lon2 = std::strtod(start, &end);
            start = end;
            _dlon = std::strtod(start, &end);
            if ( errno == ERANGE )
            {
#ifdef DEBUG
                throw std::runtime_error
                ("[DEBUG] ionex::read_header -> Invalid IONEX longtitude grid info");
#endif
                return 1;
            }
        }
        else if ( !strncmp(line+60, "EXPONENT", 8) )
        {
            *(line+10) = '\0';
            _exp = std::stoi(line, nullptr);
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
