#ifndef __IONEX_NGPT_
#define __IONEX_NGPT_

#include <fstream>
#include <vector>
#include <tuple>
#include "datetime_v2.hpp"

/**
 * \file
 *
 * \version
 *
 * \author    xanthos@mail.ntua.gr <br>
 *            danast@mail.ntua.gr
 *
 * \date      Sat 16 Jan 2016 01:08:46 PM EET 
 *
 * \copyright Copyright © 2015 Dionysos Satellite Observatory, <br>
 *            National Technical University of Athens. <br>
 *            This work is free. You can redistribute it and/or modify it under
 *            the terms of the Do What The Fuck You Want To Public License,
 *            Version 2, as published by Sam Hocevar. See http://www.wtfpl.net/
 *            for more details.
 *
 * <b><center><hr>
 * National Technical University of Athens <br>
 *      Dionysos Satellite Observatory     <br>
 *        Higher Geodesy Laboratory        <br>
 *      http://dionysos.survey.ntua.gr
 * <hr></center></b>
 *
 */

namespace ngpt
{

// The type we store ionex TEC values in.
// TODO remove this shit. Always int
// using ionex_tec_type = int;

/// The type we store ionex grid values in.
using ionex_grd_type = float;

/*
 * \class ionex
 *
 * \details This class is an interface to the igs IONEX files.
 *
 * \note    IONEX files hold date/time records in UT, i.e. UT1. From \cite ionex1
 *          sec. 3.5, :<br/>
 *          **Solar Time: UT1 and UTC**<br/>
 *          UT1 (or plain UT) is the modern equivalent of mean solar time, and 
 *          is really an angle rather than time in the physics sense. Originally
 *          defined in terms of a point in the sky called "the fictitious
 *          mean Sun", UT1 is now defined through its relationship with Earth 
 *          rotation angle (formerly through sidereal time). Because the Earth’s
 *          rotation rate is slightly irregular—for geophysical reasons - and is
 *          gradually decreasing, the UT1 second is not precisely matched to 
 *          the SI second. This makes UT1 itself unsuitable for use as a time 
 *          scale in physics applications. However, some applications do 
 *          require UT1, such as pointing a telescope or antenna at a celestial
 *          target, delay calculations in interferometers, and diurnal aberration,
 *          parallax and Doppler corrections.
 *
 * \warning -# The class only knows how te **read** IONEX files, i.e. they are
 *          always considered as input file streams. You cannot write to/a
 *          IONEX file.
 *          -# Record lines in IONEX files do not exceed 80 chars.
 *          -# The map grid points (lat, lon, hgt) are recorded within the
 *          IONEX files with a precision of 1e-1 degrees. Various functions use
 *          this fact to turn the (float/double) grid values to (int/long).
 *          Should this change, we're fucked!
 */
class ionex
{
    /// Let's not write this more than once.
    typedef std::ifstream::pos_type pos_type;

public:
    /// This is the datetime resolution for ionex dates
    typedef ngpt::datev2<ngpt::milliseconds> datetime_ms;

    /// Valid IONEX versions.
    enum class ionex_version : char { v10 };

    /// Constructor from filename.
    ionex(const char*);

    /// Destructor (closing the file is not mandatory, but nevertheless)
    ~ionex() noexcept { if ( _istream.is_open() ) _istream.close(); }

    /// Copy not allowed !
    ionex(const ionex&) = delete;

    /// Assignment not allowed !
    ionex& operator=(const ionex&) = delete;

    /// Move Constructor.
    ionex(ionex&& a)
      /*noexcept(std::is_nothrow_move_constructible<std::ifstream>::value)*/ = default;

    /// Move assignment operator.
    ionex& operator=(ionex&& a) //TODO glang++ does not accept this!
    /*noexcept(std::is_nothrow_move_assignable<std::ifstream>::value)*/ = default;

    std::string filename() const noexcept { return this->_filename; }

    datetime_ms first_epoch() const noexcept { return this->_first_epoch; }
    
    datetime_ms last_epoch() const noexcept { return this->_last_epoch; }

    std::tuple<ionex_grd_type, ionex_grd_type, ionex_grd_type> latitude_grid()
    const noexcept
    { return std::make_tuple(_lat1, _lat2, _dlat); }
    
    std::tuple<ionex_grd_type, ionex_grd_type, ionex_grd_type> longtitude_grid()
    const noexcept
    { return std::make_tuple(_lon1, _lon2, _dlon); }
  
    std::vector<std::vector<double>>
    interpolate(
        const std::vector<std::pair<ionex_grd_type,ionex_grd_type>>& points,
        std::vector<datetime_ms>& epochs,
        datetime_ms* ifrom = nullptr,
        datetime_ms* ito = nullptr,
        int interval = 0
    );

private:
    /// Read the instance header, and assign (most of) the fields.
    int read_header();

    /// Read all TEC maps and (spatial) interpolate for a given vector of
    /// points
    int get_tec_at(
            const std::vector<std::pair<ionex_grd_type,ionex_grd_type>>&,
            std::vector<datetime_ms>&,
            std::vector<std::vector<int>>&,
            datetime_ms *from = nullptr,
            datetime_ms *to = nullptr
    );

    /// This is a help function for parsing datetime arguments to the
    /// interpolation function.
    int parse_epoch_arguments(std::vector<datetime_ms>&,
                              datetime_ms*, datetime_ms*, int&);

    // Read a TEC map for a constant epoch
    int read_tec_map(std::vector<int>&);
    int skip_tec_map();

    // Read an individual map
    int read_latitude_map(std::size_t, std::vector<int>&, std::size_t&);

    // Compute how many (const) latitude maps there exist for each height.
    std::size_t latitude_maps() const noexcept;

    // Compute how many longtitude lienes there exist for a single const-
    // latitude map.
    std::size_t longtitude_lines() const noexcept;

    std::string      _filename;      ///< The name of the antex file.
    std::ifstream    _istream;       ///< The infput (file) stream.
    ionex_version    _version;       ///< Ionex  version (1.0).
    pos_type         _end_of_head;   ///< Mark the 'END OF HEADER' field.
    //ngpt::time_scale _time_scale;    ///< The timescale
    datetime_ms      _first_epoch;   ///< Epoch of first TEC map (UT)
    datetime_ms      _last_epoch;    ///< Epoch of first TEC map (UT)
    int              _interval;      ///< Time interval between maps in integer
    ///< seconds. If 0, interval may vary.
    std::size_t      _maps_in_file;  ///< Total number of TEC/RMS/HGT maps
    float            _min_elevation; ///< Minimum elevation angle (degrees)
    float            _base_radius;   ///< Mean earth radious (km)
    int              _map_dimension; ///< 2 or 3 (dimensions)
    ionex_grd_type   _hgt1, _hgt2, _dhgt; ///< the height grid; from _hgt1 to
    ///< _hgt2 with increment _dhgt. For 2d maps, _hgt1 = _hgt2 and _dhgt=0
    ionex_grd_type   _lat1, _lat2, _dlat; ///< the latitude grid; from _lat1 to
    ///< _lat2 with increment _dlat
    ionex_grd_type   _lon1, _lon2, _dlon; ///< the longtitude grid; from _lon1 to
    ///< _lon2 with increment _dlon
    int              _exp;         ///< the exponent; default = -1

}; // end ionex

} // end ngpt

#endif
