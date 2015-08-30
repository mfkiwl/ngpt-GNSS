#ifndef _GNSS_ANTENNA_
#define _GNSS_ANTENNA_

#include <string>
#include <cstring>

/*
 * \file      antenna.hpp
 *
 * \version   0.10
 *
 * \author    xanthos@mail.ntua.gr <br>
 *            danast@mail.ntua.gr
 *
 * \date      MAY 2015
 *
 * \brief     GNSS Antenna (plus Radome) Class. Used both for stations and
 *            satellites.
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

namespace ngpt
{

/// Maximum size of a char array, holding any antenna type
constexpr std::size_t _ANTENNA_TYPE_SIZE_ { 15 };

/// Maximum size of a char array, holding any radome type
constexpr std::size_t _ANTENNA_RADOME_SIZE_ { 4 };

/// Maximum size a char array, holding any antenna+radome pair
constexpr std::size_t _MAX_ANTENNA_SIZE_ 
{ _ANTENNA_TYPE_SIZE_ + 1 + _ANTENNA_RADOME_SIZE_ };

/// Maximum size a char array, holding any antenna+radome pair in bytes
constexpr std::size_t _MAX_ANTENNA_SIZE_BYTES_  
{ _MAX_ANTENNA_SIZE_ * sizeof(char) };

/// Offset from the start of a char array holding any antenna+radome pair,
/// to the radome type
constexpr std::size_t _RADOME_OFFSET_ { _ANTENNA_TYPE_SIZE_ + 1 };

/// Make sure nothing funny happens !
static_assert(_MAX_ANTENNA_SIZE_ > _ANTENNA_TYPE_SIZE_ + _ANTENNA_RADOME_SIZE_,
              "Antenna type and radome sizes do not match!");
static_assert(_RADOME_OFFSET_ < _MAX_ANTENNA_SIZE_,
              "Radome offset > Antenna size !");
static_assert(_MAX_ANTENNA_SIZE_BYTES_ >= _MAX_ANTENNA_SIZE_,
              "Antenna size in bytes < Antenna size ?!?");

/** \details  This class holds a GNSS Antenna + Radome type.
 *            For the antenna, we need _ANTENNA_TYPE_SIZE_ columns maximum.
 *            First three characters are manufacturer code (except satellites).
 *            Allowed in manufacturer code: '-' and 'A-Z' and '0-9'.
 *            Allowed in model name: '/-_.+' and 'A-Z' and '0-9'. Model name
 *            must start with 'A-Z' or '0-9'.
 *            Radomes: _ANTENNA_RADOME_SIZE_ columns; 'A-Z' and '0-9' allowed.
 *            Antenna+Radome: Combine them with the radome code in columns
 *            17-20. Fill with spaces between the end of the antenna and column
 *            17.
 *            Example: AOAD/M_T        SCIT
 *            To represent an antenna+radome, we use a _MAX_ANTENNA_SIZE_ char
 *            array, where
 *            [ 0 - _ANTENNA_TYPE_SIZE_ )              is the antenna name,
 *            [ _ANTENNA_TYPE_SIZE_ ]                  whitespace
 *            [ _RADOME_OFFSET_ - _MAX_ANTENNA_SIZE_ ) is the radome name (if
 *            any)
 *
 * Reference: <a href="https://igscb.jpl.nasa.gov/igscb/station/general/rcvr_ant.tab">
 *            IGS rcvr_ant.tab</a>
 */
class Antenna
{
public:

  /// Default constructor.
  Antenna() noexcept { };

  /// Constructor from Antenna type plus Radome (if any).
  explicit Antenna (const char*) noexcept;

  /// Constructor from Antenna type plus Radome (if any).
  explicit Antenna (const std::string&) noexcept;

  /// Copy constructor.
  Antenna(const Antenna& rhs) noexcept
  {
    std::memcpy(name_,rhs.name_,_MAX_ANTENNA_SIZE_BYTES_);
  }

  /// Move constructor.
  Antenna(Antenna&&) noexcept = default;

  /// Assignment operator.
  Antenna& operator=(const Antenna& rhs) noexcept
  {
    if (this!=&rhs)
    {
      std::memcpy(name_,rhs.name_,_MAX_ANTENNA_SIZE_BYTES_);
    }
    return *this;
  }

  /// Move assignment operator.
  Antenna& operator=(Antenna&&) noexcept = default;

  /// Equality operator (checks both antenna type and radome).
  bool operator==(const Antenna& rhs) const noexcept
  {
    return ( !std::strncmp(name_,rhs.name_,_MAX_ANTENNA_SIZE_BYTES_) );
  }
  
  /// Inequality operator (checks both antenna type and radome).
  bool operator!=(const Antenna& rhs) const noexcept
  {
    return ( !(*this == rhs) );
  }

  /// Destructor.
  ~Antenna() noexcept {};

  /// Pointer to antenna name.
  inline const char* name() const noexcept
  {
    return name_;
  }

  /// Pointer to (start of) radome.
  inline const char* radome() const noexcept
  {
    return name_+_RADOME_OFFSET_;
  }

  /// Get the radome type (as String).
  inline std::string radome_to_string() const noexcept
  {
    return std::string { name_+_RADOME_OFFSET_,_ANTENNA_RADOME_SIZE_ };
  }

  /// Antenna+Radome  as string.
  inline std::string to_string() const noexcept
  {
    return std::string(name_,_MAX_ANTENNA_SIZE_);
  }

  /// Set the Radome type.
  void set_radome(const char*) noexcept;

  /// Set the Radome type.
  void set_radome(const std::string&) noexcept;
  
  /// Set the Antenna type (+ Radome).
  void set_antenna(const char*) noexcept;

private:
  char name_[_MAX_ANTENNA_SIZE_]; ///< Combined antenna and radome name.
}; // end Antenna

} // end ngpt

#endif