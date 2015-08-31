#ifndef _ANTEX_HPP_
#define _ANTEX_HPP_

#include <fstream>
#include "satsys.hpp"
#include "antenna.hpp"

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

///
/// ======================================================================
/// 3. DEFINITION OF THE ANTENNA CORRECTIONS
/// ======================================================================
///
/// In order to clarify the sign convention, the following shortened formulae are
/// given:
///
/// RECEIVER ANTENNA:
/// ----------------------------------------------------------------------
///
/// mean phase
/// center position   = ARP (antenna reference point) position +
/// phase center offset vector (given in a topocentric
/// left-handed system: north, east and up component)
///
/// observed distance = geometric distance to the mean phase center +
/// phase center variations (zenith, azimuth) +
/// other corrections
/// (Here the azimuth counts clockwise from the North toward the East.)
///
/// SATELLITE ANTENNA:
/// ----------------------------------------------------------------------
///
/// mean phase
/// center position   = CM (center of mass) position +
/// phase center offset vector
/// (The phase center offset is given in a satellite-fixed
/// right-handed coordinate system:
/// - z-axis points toward the geocenter
/// - y-axis (rotation axis of the solar panels) corresponds
/// to the cross product of the z-axis with the vector from
/// the satellite to the Sun
/// - x-axis completes the right-handed system
/// (x cross y = z))
///
/// observed distance = geometric distance to the mean phase center +
/// phase center variations (nadir, azimuth) +
/// other corrections
/// (Here the azimuth counts clockwise from the y-axis toward the x-axis when
/// looking in the direction of the negative z-axis or toward deep space,
/// respectively. For the estimation of the azimuth angle under which a station
/// is seen from the satellite, the yaw attitude (rotation about the z-axis) 
/// has to be considered. Furthermore, deviations from the nominal attitude 
/// occur during eclipse seasons when the Sun sensors fail.)
///
/// Reference -> ANTEX: The Antenna Exchange Format, Version 1.4,
///              https://igscb.jpl.nasa.gov/igscb/station/general/antex14.txt
///

namespace ngpt
{

class Antex
{
  /// Let's not write this more than once.
  typedef std::ifstream::pos_type pos_type;

  /// Valid atx versions.
  enum class ATX_VERSION : char
  {
    v14
  };

  /// Pcv Type (absolute or relative).
  enum class PCV_TYPE : char
  {
    Absolute,
    Relative
  };

public:
  /// Constructor from filename.
  Antex(const char*);
  
  /// Copy not allowed !
  Antex(const Antex&) = delete;
  
  /// Assignment not allowed !
  Antex& operator=(const Antex&) = delete;
  
  /// Move Constructor.
  Antex(Antex&& a) noexcept = default;

  /// Move assignment operator.
  Antex& operator=(Antex&& a) 
    noexcept(std::is_nothrow_move_assignable<std::ifstream>::value) = default;
  
  /// Read the instance header, and assign (most of) the fields.
  void read_header();

  /// Find a specific antenna in the instance.
  int find_antenna(const Antenna&);

private:
  std::string            _filename; ///< The name of the antex file.
  std::ifstream          _istream;  ///< The infput (file) stream.
  ngpt::SATELLITE_SYSTEM _satsys;   ///< Satellite System.
  ATX_VERSION            _version;  ///< Atx version (1.4).
  PCV_TYPE               _type;     ///< Pcv type (absolute or relative).
  ngpt::Antenna          _refant;   ///< Reference antenna (only relative pcv).
  pos_type               _end_of_head; ///< Mark the 'END OF HEADER' field.

}; // end Antex

} // end ngpt

#endif
