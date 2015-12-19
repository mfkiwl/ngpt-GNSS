#ifndef _GNSS_ANTENNA_
#define _GNSS_ANTENNA_

/* #include <regex> */

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

/** \class    antenna
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
class antenna
{
public:

    /// Antenna Type: Receiver or Satellite.
    enum class Antenna_Type : char 
    { 
      Receiver_Antenna, 
      Satellite_Antenna 
    };

    /// Default constructor.
    antenna() noexcept;

    /// Constructor from Antenna type plus Radome (if any).
    explicit antenna (const char*) noexcept;

    /// Constructor from Antenna type plus Radome (if any).
    explicit antenna (const std::string&) noexcept;

    /// Copy constructor.
    antenna(const antenna&) noexcept;

    /// Move constructor.
    antenna(antenna&&) noexcept = default;

    /// Assignment operator.
    antenna& operator=(const antenna&) noexcept;

    /// Assignment operator from a c-string.
    antenna& operator=(const char*) noexcept;

    /// Assignment operator from an std::string.
    antenna& operator=(const std::string&) noexcept;

    /// Move assignment operator.
    antenna& operator=(antenna&&) noexcept = default;

    /// Equality operator (checks both antenna type and radome).
    bool operator==(const antenna&) const noexcept;
      
    /// In-Equality operator (checks both antenna type and radome).
    bool operator!=(const antenna&) const noexcept;
      
    /// Equality operator (checks antenna type, radome and serial nr).
    bool is_same(const antenna&) const noexcept;

    /// Compare antenna's serial number to a c-string
    bool compare_serial(const char*) const noexcept;

    /// Set antenna's serial number
    void set_serial_nr(const char*) noexcept;

    /// Destructor.
    ~antenna() noexcept {};

    /// Antenna model name as string.
    std::string model_str() const noexcept;

    /// Antenna radome name as string.
    std::string radome_str() const noexcept;

    /// Antenna model/radome to string.
    std::string to_string() const noexcept;

    /*
    /// Validate antenna model/radome
    template<Antenna_Type T>
      bool validate() const 
    {
    }
    */

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
/*
    /// Validate receiver antenna
    bool validate_receiver_antenna() const;
  
    /// Validate satellite antenna
    bool validate_satellite_antenna() const;
*/
    /// Combined antenna, radome and serian number.
    char name_[antenna_details::antenna_full_max_chars]; 

}; // end antenna

/* TODO: the methods validate_receiver_antenna and validate_satellite_antenna
 * do not compile
 *
template<>
  bool Antenna::validate<Antenna::Antenna_Type::Receiver_Antenna>() const
{
  return this->validate_receiver_antenna();
}
template<>
  bool Antenna::validate<Antenna::Antenna_Type::Satellite_Antenna>() const
{
  return this->validate_satellite_antenna();
}
*/

} // end ngpt

#endif
