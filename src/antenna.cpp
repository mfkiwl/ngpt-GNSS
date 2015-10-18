#include <string>
#include <cstring>
#include <algorithm>
#include <regex>
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
Antenna::Antenna::operator=(const std::string& s) noexcept
{
  this->copy_from_str(s);
  return *this;
}

/// Equality operator.
///
/// \warning This function will NOT check the antenna serial number.
///
bool Antenna::operator==(const Antenna& rhs) noexcept
{
  return ( !std::strncmp(name_ ,rhs.name_ , 
        antenna_model_max_chars+1+antenna_radome_max_chars+1) );
}

/// This function compares two Antenna instances and return true if and only
/// if the Antenna is the same, i.e. they share the same model, radome and
/// serial number.
///
bool Antenna::is_same(const Antenna& rhs) noexcept
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

/// Set all chars in \c name_ to \c '\0'. 
inline 
void Antenna::nullify noexcept
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

/// Set radome to 'NONE'
inline
void Antenna::set_none_radome() noexcept
{
  std::memcpy(name_+antenna_model_max_chars+1, none_radome, 
      antenna_radome_max_chars * sizeof char);
}

/// Copy from an std::string (to \c name_).
///
/// \param[in] s An std::string representing a valid receiver model name.
///
/// \note        The characters copied from the input std::string \c s are 
///              <tt>min( s.size(), receiver_details::receiver_max_chars )</tt>.
///
/// \warning     This function asserts that 
///              <tt>receiver_details::receiver_max_chars > 0</tt> ; else the 
///              \c nothrow specification is not valid.
///
void Antenna::copy_from_str(const std::string& s) noexcept
{
  // set antenna model+radome to '\0'
  this->nullify_antenna();
  
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

/// Set an antenna/radome pair from a c-string. The input antenna/radome pair, 
/// should follow the conventions in https://igscb.jpl.nasa.gov/igscb/station/general/rcvr_ant.tab
/// Note that the function will leave the antenna serial number as is.
///
/// \param[in] c A \c cstring representing a valid antenna model name plus 
///              radome.
///
/// \note        The function's behaviour depends on the size of the provided
///              input c-string. So, if
///              # the input c-string has length <= antenna_model_max_chars then
///                it is assumed that the c-string only describes the antenna
///                model. The radome type is set to 'NONE'.
///              # the input c-string has length > antenna_model_max_chars then
///                it is assumed that the c-string is a full pair of antenna
///                model/radome (as described in \cite rcvr_ant ).
///
void Antenna::copy_from_cstr(const char* c) noexcept
{
  // set antenna model+radome to null.
  this->nullify_antenna();

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

/// Validate the receiver model; a receiver model, according to the IGS
/// conventions, may be a sequence of any of the characters:
///  -# 'A-Z'
///  -# '0-9' 
///  -# space 
///  -# '-_+'
/// 
/// \return true if the model name matches the specifications; false
///         otherwise.
///
/// \note   To perform the check, the receiver model name is compared against
///         the regular expression: '[A-Z,0-9, ,\\+,-,_]+$'
///
/// \bug    This seems to also match the \c '/' character though it shouldn't !
///
bool Antenna::validate() const
{
  std::regex valid ( "[A-Z -_\\+]+$" );
  return std::regex_match(name_, valid);
}
