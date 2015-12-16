#ifndef _SATELLITE_HPP_
#define _SATELLITE_HPP_

/**
 * \file
 *
 * \version
 *
 * \author    xanthos@mail.ntua.gr <br>
 *            danast@mail.ntua.gr
 *
 * \date
 *
 * \brief
 *
 * \details
 *
 * \note
 *
 * \todo
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

#include <cstddef>
#include <stdexcept>
#include <map> 

namespace ngpt
{

/// Enumeration for known Satellite systems.
/// This is extracted from \cite rnx303
enum class satellite_system : char
{
    gps,
    glonass,
    sbas,
    galileo,
    beidou,
    qzss,
    irnss,
    mixed
};

/// Traits for Satellite Systems
template<satellite_system S>
  struct SatelliteSystemTraits
{ };

/// Specialize traits for Satellite System Gps
template<>
struct SatelliteSystemTraits<satellite_system::gps>
{
    /// Identifier
    static constexpr char identifier { 'G' };

    /// Dictionary holding pairs of <frequency band, freq. value>.
    static const std::map<int,double> frequency_map;
};

/// Specialize traits for Satellite System Glonass
template<>
struct SatelliteSystemTraits<satellite_system::glonass>
{
    /// Identifier
    static constexpr char identifier { 'R' };

    /// Dictionary holding pairs of <frequency band, freq. value>.
    static const std::map<int,double> frequency_map;
};

/// Specialize traits for Satellite System Galileo
template<>
struct SatelliteSystemTraits<satellite_system::galileo>
{
    /// Identifier
    static constexpr char identifier { 'E' };

    /// Dictionary holding pairs of <frequency band, freq. value>.
    static const std::map<int,double> frequency_map;
};

/// Specialize traits for Satellite System SBAS
template<>
struct SatelliteSystemTraits<satellite_system::sbas>
{
    /// Identifier
    static constexpr char identifier { 'S' };

    /// Dictionary holding pairs of <frequency band, freq. value>.
    static const std::map<int, double> frequency_map;
};

/// Specialize traits for Satellite System QZSS
template<>
struct SatelliteSystemTraits<satellite_system::qzss>
{
    /// Identifier
    static constexpr char identifier { 'J' };

    /// Dictionary holding pairs of <frequency band, freq. value>.
    static const std::map<int, double> frequency_map;
};

/// Specialize traits for Satellite System BDS
template<>
struct SatelliteSystemTraits<satellite_system::beidou>
{
    /// Identifier
    static constexpr char identifier { 'C' };

    /// Dictionary holding pairs of <frequency band, freq. value>.
    static const std::map<int, double> frequency_map;
};

/// Specialize traits for Satellite System IRNSS
template<>
struct SatelliteSystemTraits<satellite_system::irnss>
{
    /// Identifier
    static constexpr char identifier { 'I' };

    /// Dictionary holding pairs of <frequency band, freq. value>.
    static const std::map<int, double> frequency_map;
};

/// Specialize traits for Satellite System MIXED
template<>
struct SatelliteSystemTraits<satellite_system::mixed>
{
    /// Identifier
    static constexpr char identifier { 'M' };

    /// Number of frequency bands.
    static const std::size_t num_of_bands { 0 };
};

/// Given a satellite system, return its identifier
char satsys_identifier( satellite_system );

/// Given a char (i.e. identifier), return the corresponding satellite system.
satellite_system char_to_satsys( char );

/// Given a frequency band (index) and a satellite system, 
/// return the nominal frequency value.
double nominal_frequency(int band, satellite_system s);

} // end namespace

#endif
