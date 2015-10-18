#include <string>
#include <cstring>
#include <algorithm>
#include "antenna.hpp"

using ngpt::Antenna;
using ngpt::antenna_details::antenna_model_max_chars;
using ngpt::antenna_details::antenna_radome_max_chars;
using ngpt::antenna_details::antenna_serial_max_chars;
using ngpt::antenna_details::antenna_full_max_chars;

/// The 'NONE' radome as a c-string.
constexpr char none_radome[] = "NONE";

/// Empty (default) constructor; all characters in \c name_ are set to \c '\0'.
///
Antenna::Antenna() noexcept 
{
  this->nullify();
}

/// Constructor from antenna type. If the size of the input string is smaller
/// than antenna_model_max_chars, then the radome is set to 'NONE' (i.e. it
/// is assumed that the input string only describes the antenna model name).
///
/// \param[in] c  A c-string representing a valid antenna name, or an antenna
///               name/radome pair.
///
/// \note         If the size of the input c-string is less than antenna_model_max_chars
///               the radome is automatically set to 'NONE'.
///
Antenna::Antenna(const char* c) noexcept
{
  this->copy_from_cstr(c);
}

/// Constructor from antenna type. If the size of the input string is smaller
/// than antenna_model_max_chars, then the radome is set to 'NONE' (i.e. it
/// is assumed that the input string only describes the antenna model name).
///
/// \param[in] c  An std::string representing a valid antenna name, or an 
///               antenna name/radome pair.
///
/// \note         If the size of the input std::string is less than antenna_model_max_chars
///               the radome is automatically set to 'NONE'.
///
Antenna::Antenna(const std::string& s) noexcept
{
  this->copy_from_str(s);
}

/// Copy constructor.
///
Antenna::Antenna(const Antenna& rhs) noexcept
{
  std::memcpy(name_, rhs.name_, antenna_full_max_chars * sizeof(char));
}

/// Assignment operator.
///
Antenna& Antenna::operator=(const Antenna& rhs) noexcept
{
  if (this!=&rhs)
  {
    std::memcpy(name_, rhs.name_, antenna_full_max_chars * sizeof(char));
  }
  return *this;
}

/// Assignment operator (from c-string).
///
/// \param[in] c  A c-string representing a valid antenna name, or an 
///               antenna name/radome pair.
///
/// \note         If the size of the input c-string is less than antenna_model_max_chars
///               the radome is automatically set to 'NONE'.
///
Antenna& Antenna::operator=(const char* c) noexcept
{
  this->copy_from_cstr(c);
  return *this;
}

/// Assignment operator (from std::string).
///
/// \param[in] c  An std::string representing a valid antenna name, or an 
///               antenna name/radome pair.
///
/// \note         If the size of the input std::string is less than antenna_model_max_chars
///               the radome is automatically set to 'NONE'.
///
Antenna& Antenna::operator=(const std::string& s) noexcept
{
  this->copy_from_str(s);
  return *this;
}

/// Equality operator.
///
/// \warning This function will NOT check the antenna serial number.
///
bool Antenna::operator==(const Antenna& rhs) const noexcept
{
  return ( !std::strncmp(name_ ,rhs.name_ , 
        antenna_model_max_chars+1+antenna_radome_max_chars+1) );
}

/// This function compares two Antenna instances and return true if and only
/// if the Antenna is the same, i.e. they share the same model, radome and
/// serial number.
///
bool Antenna::is_same(const Antenna& rhs) const noexcept
{
  return ( !std::strncmp(name_ ,rhs.name_ , antenna_full_max_chars) );
}

/// Antenna model name as string.
///
std::string Antenna::model_str() const noexcept
{
  return std::string(name_, antenna_model_max_chars);
}

/// Antenna radome name as string.
///
std::string Antenna::radome_str() const noexcept
{
  return std::string(name_+antenna_model_max_chars+1, antenna_radome_max_chars);
}

/// Antenna model/radome pair as string.
///
std::string Antenna::toString() const noexcept
{
  return std::string(name_, antenna_model_max_chars+1+antenna_radome_max_chars);
}
/// Set all chars in \c name_ to \c '\0'. 
inline 
void Antenna::nullify() noexcept
{
  std::memset( name_, '\0', antenna_full_max_chars * sizeof(char) );
}

/// Set all chars corresponding to the antenna name plus radome in \c name_,
/// to \c '\0'. Note that the serial number will be left as is.
///
inline 
void Antenna::nullify_antenna() noexcept
{
  std::size_t chars { antenna_model_max_chars + 1 
    + antenna_radome_max_chars + 1 };
  std::memset( name_, '\0', chars * sizeof(char) );
}

/// Set all chars corresponding to the antenna name plus radome in \c name_,
/// to \c ' ' (i.e. whitespace char). Note that the serial number will be left 
/// as is.
///
inline 
void Antenna::wspaceify_antenna() noexcept
{
  std::size_t chars { antenna_model_max_chars + 1 
    + antenna_radome_max_chars + 1 };
  std::memset( name_, ' ', chars * sizeof(char) );
}

/// Set radome to 'NONE'
inline
void Antenna::set_none_radome() noexcept
{
  std::memcpy(name_+antenna_model_max_chars+1, none_radome, 
      antenna_radome_max_chars * sizeof(char));
}

/// \details     Set an antenna/radome pair from an std::string. The input 
///              antenna/radome pair, should follow the conventions in 
///              \cite rcvr_ant . Note that the function will leave the antenna
///              serial number as is.
///
/// \param[in] c An std::string representing a valid antenna model name, or 
///              optionaly aan antenna/radome pair.
///
/// \note        The function's behaviour depends on the size of the provided
///              input std::string. So, if
///              - the input string has length <= antenna_model_max_chars then
///                it is assumed that the c-string only describes the antenna
///                model. The radome type is set to 'NONE'.
///              - the input string has length > antenna_model_max_chars then
///                it is assumed that the c-string is a full pair of antenna
///                model/radome (as described in \cite rcvr_ant ).
///
void Antenna::copy_from_str(const std::string& s) noexcept
{
  // set antenna model+radome to ' '
  this->wspaceify_antenna();
  
  // size of input string
  std::size_t str_size { s.size() };

  // assuming we have a model/radome pair; copy to name_ at maximum
  // (antenna_model_max_chars+1+antenna_radome_max_chars) characters
  if ( str_size > antenna_model_max_chars ) {
    std::size_t nrc { std::min(str_size, 
        antenna_model_max_chars+1+antenna_radome_max_chars) };
    std::copy(s.begin(), s.begin()+nrc, &name_[0]);
  } else {
  // assume we only have the model; copy it and set radome to 'NONE'
    std::copy(s.begin(), s.end(), &name_[0]);
    this->set_none_radome();
  }
}

/// \details     Set an antenna/radome pair from a c-string. The input 
///              antenna/radome pair, should follow the conventions in 
///              \cite rcvr_ant . Note that the function will leave the antenna
///              serial number as is.
///
/// \param[in] c A \c cstring representing a valid antenna model name, or 
///              optionaly aan antenna/radome pair.
///
/// \note        The function's behaviour depends on the size of the provided
///              input c-string. So, if
///              - the input c-string has length <= antenna_model_max_chars then
///                it is assumed that the c-string only describes the antenna
///                model. The radome type is set to 'NONE'.
///              - the input c-string has length > antenna_model_max_chars then
///                it is assumed that the c-string is a full pair of antenna
///                model/radome (as described in \cite rcvr_ant ).
///
void Antenna::copy_from_cstr(const char* c) noexcept
{
  // set antenna model+radome to null.
  this->wspaceify_antenna();

  // get the size of the input string.
  std::size_t ant_size { std::strlen(c) };

  // if size of input string > antenna_model_max_chars, then assume we are
  // copying model+radome,
  if ( ant_size > antenna_model_max_chars ) {
    std::memcpy(name_, c, sizeof(char) * std::min(ant_size, 
        (antenna_model_max_chars + 1 + antenna_radome_max_chars) ));
  } else {
  // else assume we are only copying the antenna model
    std::memcpy(name_, c, sizeof(char) * ant_size);
    this->set_none_radome();
  }
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
bool Antenna::validate_receiver_antenna() const
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
bool Antenna::validate_satellite_antenna() const
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
