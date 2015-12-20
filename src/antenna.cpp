#include <string>
#include <cstring>
#include <algorithm>
#include "antenna.hpp"

using ngpt::antenna;
using ngpt::antenna_details::antenna_model_max_chars;
using ngpt::antenna_details::antenna_radome_max_chars;
using ngpt::antenna_details::antenna_serial_max_chars;
using ngpt::antenna_details::antenna_full_max_chars;

/// The 'NONE' radome as a c-string.
constexpr char none_radome[] = "NONE";

/// Empty (default) constructor; all characters in \c name_ are set to \c '\0'.
///
antenna::antenna() noexcept 
{
    this->nullify();
}

/// Constructor from antenna type. At maximum antenna_full_max_char - 1 chars
/// are copied from the string.
antenna::antenna(const char* c)
noexcept
{
    this->copy_from_cstr(c);
}

/// Constructor from antenna type. At maximum antenna_full_max_char - 1 chars
/// are copied from the string.
antenna::antenna(const std::string& s)
noexcept
{
    this->copy_from_str(s);
}

/// Copy constructor.
///
antenna::antenna(const antenna& rhs)
noexcept
{
    std::memcpy(name_, rhs.name_, antenna_full_max_chars * sizeof(char));
}

/// Assignment operator.
///
antenna& antenna::operator=(const antenna& rhs)
noexcept
{
    if (this!=&rhs)
    {
        std::memcpy(name_, rhs.name_, antenna_full_max_chars * sizeof(char));
    }
    return *this;
}

/// Assignment operator (from c-string). At maximum antenna_full_max_char - 1
/// chars are copied from the string.
///
antenna&
antenna::operator=(const char* c)
noexcept
{
    this->copy_from_cstr(c);
    return *this;
}

/// Assignment operator (from std::string). At maximum antenna_full_max_char - 1
/// chars are copied from the string.
///
antenna&
antenna::operator=(const std::string& s)
noexcept
{
    this->copy_from_str(s);
    return *this;
}

/// Equality operator.
///
/// \warning This function will NOT check the antenna serial number.
///
bool
antenna::operator==(const antenna& rhs)
const noexcept
{
    return ( !std::strncmp(name_ ,rhs.name_ , 
        antenna_model_max_chars+1+antenna_radome_max_chars) );
}

/// In-equality operator.
///
/// \warning This function will NOT check the antenna serial number.
///
bool
antenna::operator!=(const antenna& rhs)
const noexcept
{ return ! (*this == rhs ); }

/// Compare the string lexicographically.
///
bool
antenna::operator<(const antenna& rhs)
const noexcept
{ return std::strcmp(name_, rhs.name_) < 0; }

/// This function compares two Antenna instances and return true if and only
/// if the Antenna is the same, i.e. they share the same model, radome and
/// serial number.
///
bool
antenna::is_same(const antenna& rhs)
const noexcept
{ return ( !std::strncmp(name_ ,rhs.name_ , antenna_full_max_chars) ); }

/// Antenna model name as string.
/// TODO
std::string antenna::model_str() const noexcept
{
    return std::string(name_, antenna_model_max_chars);
}

/// Antenna radome name as string.
/// TODO
std::string antenna::radome_str() const noexcept
{
    return std::string(name_+antenna_model_max_chars+1,
                       antenna_radome_max_chars);
}

/// Antenna model/radome pair as string.
/// TODO
std::string antenna::to_string() const noexcept
{
    return std::string(name_, 
                       antenna_model_max_chars+1+antenna_radome_max_chars);
}
/// Set all chars in \c name_ to \c '\0'. 
inline void
antenna::nullify()
noexcept
{ std::memset( name_, '\0', antenna_full_max_chars * sizeof(char) ); }

/// Set all chars corresponding to the antenna name plus radome in \c name_,
/// to \c '\0'. Note that the serial number will be left as is.
///
inline void
antenna::nullify_antenna()
noexcept
{
    std::size_t chars { antenna_model_max_chars + 1 
                      + antenna_radome_max_chars };
    std::memset( name_, '\0', chars * sizeof(char) );
}

/// Set all chars corresponding to the antenna name plus radome in \c name_,
/// to \c ' ' (i.e. whitespace char). Note that the serial number will be left 
/// as is.
///
inline void
antenna::wspaceify_antenna()
noexcept
{
    std::size_t chars { antenna_model_max_chars + 1 
                      + antenna_radome_max_chars };
    std::memset( name_, ' ', chars * sizeof(char) );
}

/// Set radome to 'NONE'
inline void
antenna::set_none_radome()
noexcept
{
    std::memcpy(name_+antenna_model_max_chars+1, none_radome, 
                antenna_radome_max_chars * sizeof(char));
}

/*
 *  \details     Set an antenna/radome pair from a c-string. The input 
 *               antenna/radome pair, should follow the conventions in 
 *               \cite rcvr_ant . 
 * 
 *  \param[in] c A \c cstring representing a valid antenna model name (optionaly
 *               including a serial number).
 * 
 *  \note        At most antenna_full_max_chars - 1 characters will be copied;
 *               any remaining characters will be ignored.
 */ 
void
antenna::copy_from_str(const std::string& s)
noexcept
{
    // set antenna model+radome+serial to '\0'
    this->nullify();
  
    // size of input string
    std::size_t str_size { s.size() };
    std::size_t nrc      { std::min(str_size, antenna_full_max_chars-1) };
    std::copy(s.begin(), s.begin()+nrc, &name_[0]);
}

/*
 *  \details     Set an antenna/radome pair from a c-string. The input 
 *               antenna/radome pair, should follow the conventions in 
 *               \cite rcvr_ant . 
 * 
 *  \param[in] c A \c cstring representing a valid antenna model name (optionaly
 *               including a serial number).
 * 
 *  \note        At most antenna_full_max_chars - 1 characters will be copied;
 *               any remaining characters will be ignored.
 */ 
void
antenna::copy_from_cstr(const char* c)
noexcept
{
    // set name_ to null.
    this->nullify();

    // get the size of the input string.
    std::size_t ant_size { std::strlen(c) };

    // copy at maximum antenna_full_max_chars - 1 characters
    std::memcpy(name_, c, sizeof(char) * 
                          std::min(ant_size, antenna_full_max_chars-1) );
}

/// Set the antenna's serial number
///
/// \todo Should i strip trailing wahitespaces ??
///
void
antenna::set_serial_nr(const char* c)
noexcept
{
    constexpr std::size_t start_idx { antenna_model_max_chars  + 1 /* whitespace */
                                    + antenna_radome_max_chars };

    std::memset(name_ + start_idx, '\0', antenna_serial_max_chars);

    std::memcpy(name_ + start_idx, c, sizeof(char) * 
                        std::min(std::strlen(c), antenna_serial_max_chars) );

    return;
}

/// Compare the antenna's serial number to the given c-string
bool
antenna::compare_serial(const char* c)
const noexcept
{
    return ! std::strncmp(name_ + antenna_model_max_chars 
                                + 1 + antenna_radome_max_chars,
                          c,
                          antenna_serial_max_chars);
}

/*
 * TODO The following methods do not compile !!

/// \details Specialization for Receiver Antennas. Validate model/radome based
///          on \cite rcvr_ant . Specifically:
///          - First three characters are manufacturer code
///          Allowed in manufacturer code: - and A-Z and 0-9
///          - Allowed in model name: /-_.+ and A-Z and 0-9
///          - Model name must start with A-Z or 0-9
///          - 4 columns; A-Z and 0-9 allowed
///
bool antenna::validate_receiver_antenna() const
{
    // validate whitespace between model name and radome
    if ( !(name_[antenna_model_max_chars] == ' ') ) {
      return false;
    }
  
    // First three characters are manufacturer code;
    // Allowed in manufacturer code: - and A-Z and 0-9
    std::string man { name_, 3 };
    std::regex  valid_man ( "[A-Z0-9-]+" );
    if ( !std::regex_match(man, valid_man) ) {
      return false;
    }

    // Allowed in model name: /-_.+ and A-Z and 0-9
    // Model name must start with A-Z or 0-9
    std::string mod { name_+2,  antenna_model_max_chars};
    std::regex  valid_mod ( "[A-Z0-9][A-Z0-9-_/ \\.\\+]+" );
    if ( !std::regex_match(mod, valid_mod) ) {
      return false;
    }

    // radome: 4 columns; A-Z and 0-9 allowed
    std::string rad { name_+antenna_model_max_chars+1,  antenna_radome_max_chars};
    std::regex  valid_rad ( "[A-Z0-9]+" );
    if ( !std::regex_match(rad, valid_rad) ) {
      return false;
    }
  
    return true;
}

/// \details Specialization for Satellite Antennas. Validate model/radome based
///          on \cite rcvr_ant . Specifically:
///          - Allowed in model name: /-_.+ and A-Z and 0-9
///          - Model name must start with A-Z or 0-9
///          - 4 columns; A-Z and 0-9 allowed
///
bool antenna::validate_satellite_antenna() const
{
    // validate whitespace between model name and radome
    if ( !(name_[antenna_model_max_chars] == ' ') ) {
      return false;
    }
  
    // Allowed in model name: /-_.+ and A-Z and 0-9
    // Model name must start with A-Z or 0-9
    std::string mod { name_,  antenna_model_max_chars};
    std::regex valid_mod ( "[A-Z0-9][A-Z0-9-_/ \\.\\+]+" );
    if ( !std::regex_match(mod, valid_mod) ) {
      return false;
    }

    // radome: 4 columns; A-Z and 0-9 allowed
    std::string rad { name_+antenna_model_max_chars+1,  antenna_radome_max_chars};
    std::regex  valid_rad ( "[A-Z0-9]+" );
    if ( !std::regex_match(rad, valid_rad) ) {
      return false;
    }
  
    return true;
}
*/
