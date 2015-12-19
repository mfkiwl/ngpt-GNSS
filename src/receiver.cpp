#include <string>
#include <cstring>
#include <algorithm>
/* #include <regex> */
#include "receiver.hpp"

using ngpt::Receiver;

/// Empty (default) constructor; all characters in \c name_ are set to \c '\0'.
///
Receiver::Receiver() noexcept 
{
    this->nullify();
}

/// Constructor from receiver type.
///
Receiver::Receiver(const char* c) noexcept
{
    this->copy_from_cstr(c);
}

/// Constructor from receiver type.
///
Receiver::Receiver(const std::string& s) noexcept
{
    this->copy_from_str(s);
}

/// Copy constructor.
///
Receiver::Receiver(const Receiver& rhs) noexcept
{
    std::memcpy(name_, rhs.name_, receiver_details::receiver_max_bytes);
}

/// Assignment operator.
///
Receiver& Receiver::operator=(const Receiver& rhs) noexcept
{
    if (this!=&rhs)
    {
        std::memcpy(name_, rhs.name_, receiver_details::receiver_max_bytes);
    }
    return *this;
}

/// Assignment operator (from c-string).
///
Receiver& Receiver::operator=(const char* c) noexcept
{
    this->copy_from_cstr(c);
    return *this;
}

/// Assignment operator (from std::string).
///
Receiver& Receiver::operator=(const std::string& s) noexcept
{
    this->copy_from_str(s);
    return *this;
}

/// Equality operator.
///
bool Receiver::operator==(const Receiver& rhs) noexcept
{
    return ( !std::strncmp(name_ ,rhs.name_ , 
             receiver_details::receiver_max_chars) );
}

/// Pointer to receiver name.
///
inline 
const char* Receiver::name() const noexcept
{
    return name_;
}

/// Receiver name as string.
///
std::string Receiver::toString() const noexcept
{
    return std::string(name_, receiver_details::receiver_max_chars+1);
}

/// Set all chars in \c name_ to '\0'.
///
inline 
void Receiver::nullify() noexcept
{
    std::memset( name_, '\0', 
               (receiver_details::receiver_max_chars+1) * sizeof(char) );
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
inline 
void Receiver::copy_from_str(const std::string& s) noexcept
{
    this->nullify();
    // std::basic_string::copy can only throw if max_chars <= 0, i.e. NEVER !
    static_assert(receiver_details::receiver_max_chars > 0,
                 "receiver_details::receiver_max_chars <= 0");
    s.copy(name_, receiver_details::receiver_max_chars, 0);
}

/// Copy from a c-string.
///
/// \param[in] c A \c cstring representing a valid receiver model name.
///
/// \note        The characters copied from the input \s cstring \c c are 
///              <tt>min( strlen(c), receiver_details::receiver_max_chars )</tt>.
///
inline
void Receiver::copy_from_cstr(const char* c) noexcept
{
    this->nullify();
    std::memcpy(name_, c, std::min(std::strlen(c) * sizeof(char),
                receiver_details::receiver_max_bytes) );
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
/*
bool Receiver::validate() const
{
    std::regex valid ( "[A-Z -_\\+]+$" );
    return std::regex_match(name_, valid);
}
*/
