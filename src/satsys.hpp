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
/// This is extracted from RINEX v3.02
enum class SATELLITE_SYSTEM : char
{
  GPS,
  GLONASS,
  SBAS,
  GALILEO,
  BDS,
  QZSS,
  MIXED
};

/// Traits for Satellite Systems
template<SATELLITE_SYSTEM S>
  struct SatelliteSystemTraits
  { };

/// Specialize traits for Satellite System Gps
template<>
  struct SatelliteSystemTraits<SATELLITE_SYSTEM::GPS>
{
  /// Identifier
  static constexpr char identifier { 'G' };

  /// Dictionary holding pairs of <frequency band, freq. value>.
  static const std::map<int,double> frequency_map;
};

/// Specialize traits for Satellite System Glonass
template<>
  struct SatelliteSystemTraits<SATELLITE_SYSTEM::GLONASS>
{
  /// Identifier
  static constexpr char identifier { 'R' };

  /// Dictionary holding pairs of <frequency band, freq. value>.
  static const std::map<int,double> frequency_map;
};

/// Specialize traits for Satellite System Galileo
template<>
  struct SatelliteSystemTraits<SATELLITE_SYSTEM::GALILEO>
{
  /// Identifier
  static constexpr char identifier { 'E' };

  /// Dictionary holding pairs of <frequency band, freq. value>.
  static const std::map<int,double> frequency_map;
};

/// Specialize traits for Satellite System SBAS
template<>
  struct SatelliteSystemTraits<SATELLITE_SYSTEM::SBAS>
{
  /// Identifier
  static constexpr char identifier { 'S' };

  /// Dictionary holding pairs of <frequency band, freq. value>.
  static const std::map<int, double> frequency_map;
};

/// Specialize traits for Satellite System QZSS
template<>
  struct SatelliteSystemTraits<SATELLITE_SYSTEM::QZSS>
{
  /// Identifier
  static constexpr char identifier { 'J' };

  /// Dictionary holding pairs of <frequency band, freq. value>.
  static const std::map<int, double> frequency_map;
};

/// Specialize traits for Satellite System BDS
template<>
  struct SatelliteSystemTraits<SATELLITE_SYSTEM::BDS>
{
  /// Identifier
  static constexpr char identifier { 'B' };

  /// Dictionary holding pairs of <frequency band, freq. value>.
  static const std::map<int, double> frequency_map;
};

/// Specialize traits for Satellite System MIXED
template<>
  struct SatelliteSystemTraits<SATELLITE_SYSTEM::MIXED>
{
  /// Identifier
  static constexpr char identifier { 'M' };
  
  /// Number of frequency bands.
  static const std::size_t num_of_bands { 0 };
};

/// Given a satellite system, return its identifier
char SatSysIdentifier( SATELLITE_SYSTEM );

/// Given a char (i.e. identifier), return the corresponding satellite system.
SATELLITE_SYSTEM charToSatSys( char );

/// Given a frequency band (index) and a satellite system, 
/// return the nominal frequency value.
double nominal_frequency(int band, SATELLITE_SYSTEM s);

} // end namespace

#endif