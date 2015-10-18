#ifndef _GNSS_ANTENNA_
#define _GNSS_ANTENNA_

/**
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

  /// Namespace to hide antenna specific details.
  namespace antenna_details
  {
    /// Maximum number of characters describing a GNSS antenna model 
    /// (no radome).
    constexpr std::size_t antenna_model_max_chars { 15 };

    /// Maximum number of characters describing a GNSS antenna radome type.
    constexpr std::size_t antenna_radome_max_chars { 4 };

    /// Maximum number of characters describing a GNSS antenna serial number.
    constexpr std::size_t antenna_serial_max_chars { 20 };

    /// Mximum size to represent all fields, including whitespace and null-
    /// terminating chars.
    /// \warning No null-terminating character.
    constexpr std::size_t antenna_full_max_chars
    {  antenna_model_max_chars  + 1 /* whitespace */
     + antenna_radome_max_chars + 1 /* whitespace */
     + antenna_serial_max_chars
    };
  }

/** \class    Antenna
 *
 *  \details  This class holds a GNSS Antenna + Radome type and a Serial Number.
 *
 * Reference: <a href="https://igscb.jpl.nasa.gov/igscb/station/general/rcvr_ant.tab">
 *            IGS rcvr_ant.tab</a>,
 *            <a href="https://igscb.jpl.nasa.gov/igscb/station/general/blank.log">
 *            IGS log file</a>.
 *
 * \note      The c-string array allocated for each instance, looks like:
 * \verbatim
  N = antenna_model_max_chars
  M = antenna_radome_max_chars
  K = antenna_serial_max_chars
  [0,     N)       antenna model name
  [N+1,   N+M+1)   antenna radome name
  [N+M+2, N+M+K+2) antenna serial number

  | model name      |   | radome      |     | serial number     |       |
  v                 v   v             v     v                   v       v
  +---+---+-...-+---+---+---+-...-+---+-----+-----+-...-+-------+-------+
  | 0 | 1 |     |N-1| N |N+1|     |N+M|N+M+1|N+M+2|     |N+M+K+1|N+M+K+2|
  +---+---+-...-+---+---+---+-...-+---+-----+-----+-...-+-------+-------+
                      ^                  ^                          ^
                      |                  |                          |
            whitespace character         |                          |
                                   whitespace character             |
                                                                   '\0'
  \endverbatim
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

  /// \brief   Equality operator (checks both antenna type and radome).
  ///
  /// \warning Antenna serial numbers are not checked! To compare antenna
  ///          instances using also the serial numbers, see 
  ///          Antenna::is_same(const Antenna&) const
  bool operator==(const Antenna& rhs) const noexcept
  {
    return ( !std::strncmp(name_,rhs.name_,_MAX_ANTENNA_SIZE_BYTES_) );
  }

  /// Comparisson function, using also the serial number.
  bool is_same(const Antenna& rhs) const noexcept
  {
    return (*this == rhs) &&
      (!std::strncmp(name_+_SN_OFFSET_,rhs.name_+_SN_OFFSET_,_ANTENNA_SN_SIZE_));
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

  /// Clear the name, radome and serial number (all set to whitespace).
  inline void clear() noexcept
  {
    std::memset(name_, ' ', _MAX_ANTENNA_SIZE_);
  }

  /// Set the Radome type.
  void set_radome(const char*) noexcept;

  /// Set the Radome type.
  void set_radome(const std::string&) noexcept;

  /// Set the Antenna type (+ Radome).
  void set_antenna(const char*) noexcept;

private:

  ///< Combined antenna, radome and serian num.
  char name_[antenna_details::antenna_full_max_chars]; 

}; // end Antenna

} // end ngpt

#endif
