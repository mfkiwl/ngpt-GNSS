#include <string>
#include <cstring>
#include <algorithm>
/* #include <regex> */
#include "receiver.hpp"

using ngpt::receiver;

/// Empty (default) constructor; all characters in \c name_ are set to \c '\0'.
///
receiver::receiver()
noexcept 
{
    this->nullify();
}

/// Constructor from receiver type.
///
receiver::receiver(const char* c)
noexcept
{
    this->copy_from_cstr(c);
}

/// Constructor from receiver type.
///
receiver::receiver(const std::string& s)
noexcept
{
    this->copy_from_str(s);
}

/// Copy constructor.
///
receiver::receiver(const receiver& rhs)
noexcept
{
    std::memcpy(name_, rhs.name_, receiver_details::receiver_max_bytes);
}

/// Assignment operator.
///
receiver& 
receiver::operator=(const receiver& rhs)
noexcept
{
    if (this!=&rhs)
    {
        std::memcpy(name_, rhs.name_, receiver_details::receiver_max_bytes);
    }
    return *this;
}

/// Assignment operator (from c-string).
///
receiver&
receiver::operator=(const char* c)
noexcept
{
    this->copy_from_cstr(c);
    return *this;
}

/// Assignment operator (from std::string).
///
receiver&
receiver::operator=(const std::string& s)
noexcept
{
    this->copy_from_str(s);
    return *this;
}

/// Equality operator.
///
bool
receiver::operator==(const receiver& rhs)
noexcept
{
    return ( !std::strncmp(name_ ,rhs.name_ , 
             receiver_details::receiver_max_chars) );
}

/// Pointer to receiver name.
///
inline 
const char*
receiver::name()
const noexcept
{
    return name_;
}

/// receiver name as string.
///
std::string
receiver::toString()
const noexcept
{
    return std::string(name_, receiver_details::receiver_max_chars+1);
}

/// Set all chars in \c name_ to '\0'.
///
inline void
receiver::nullify()
noexcept
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
void
receiver::copy_from_str(const std::string& s)
noexcept
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
void
receiver::copy_from_cstr(const char* c)
noexcept
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
bool receiver::validate() const
{
    std::regex valid ( "[A-Z -_\\+]+$" );
    return std::regex_match(name_, valid);
}
*/
