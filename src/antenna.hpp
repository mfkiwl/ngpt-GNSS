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
 * \details  This class holds a GNSS Antenna either for a satellite or a 
 *           receiver. Every antenna is repreesnted by a specific Antenna model
 *           name, a Radome model and a Serial Number. These are all concateneted
 *           is a char array (but not exactly a c-string). See the note below
 *           for how this character array is formed.
 *
 * References \cite rcvr_ant
 *
 * \note     The c-string array allocated for each instance, looks like:
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
                                       '\0'                         |
                                                                   '\0'
  \endverbatim
 *
 * \example  test_antenna.cpp
 *           An example to show how to use the Antenna class.
 */
class Antenna
{
public:

  /// Default constructor.
  Antenna() noexcept;

  /// Constructor from Antenna type plus Radome (if any).
  explicit Antenna (const char*) noexcept;

  /// Constructor from Antenna type plus Radome (if any).
  explicit Antenna (const std::string&) noexcept;

  /// Copy constructor.
  Antenna(const Antenna&) noexcept;

  /// Move constructor.
  Antenna(Antenna&&) noexcept = default;

  /// Assignment operator.
  Antenna& operator=(const Antenna&) noexcept;

  /// Assignment operator from a c-string.
  Antenna& operator=(const char*) noexcept;

  /// Assignment operator from an std::string.
  Antenna& operator=(const std::string&) noexcept;

  /// Move assignment operator.
  Antenna& operator=(Antenna&&) noexcept = default;

  /// Equality operator (checks both antenna type and radome).
  bool operator==(const Antenna&) const noexcept;
  
  /// Equality operator (checks antenna type, radome and serial nr).
  bool is_same(const Antenna&) const noexcept;

  /// Destructor.
  ~Antenna() noexcept {};

  /// Antenna model name as string.
  std::string model_str() const noexcept;

  /// Antenna radome name as string.
  std::string radome_str() const noexcept;

  /// Antenna model/radome to string.
  std::string toString() const noexcept;

private:

  /// Set all chars in \c name_ to \c '\0'. 
  inline
  void nullify() noexcept;

  /// Set all chars corresponding to the antenna name plus radome in \c name_,
  /// to \c '\0'. 
  inline 
  void nullify_antenna() noexcept;

  /// Set all chars corresponding to the antenna name plus radome in \c name_,
  /// to \c ' '. 
  inline 
  void wspaceify_antenna() noexcept;

  /// Set radome to 'NONE'
  inline
  void set_none_radome() noexcept;

  /// Copy from an std::string (to \c name_).
  void copy_from_str(const std::string&) noexcept;

  /// Set an antenna/radome pair from a c-string. The input antenna/radome pair, 
  void copy_from_cstr(const char*) noexcept;

  /// Combined antenna, radome and serian number.
  char name_[antenna_details::antenna_full_max_chars]; 

}; // end Antenna

} // end ngpt

#endif
