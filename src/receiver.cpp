#include <string>
#include <cstring>
#include <algorithm>
#include "receiver.hpp"

using ngpt::Receiver;

Receiver::Receiver() noexcept 
{
  this->nullify();
}

/// Constructor from receiver type.
Receiver::Receiver(const char* c) noexcept
{
  this->copy_from_cstr(c);
}

/// Constructor from receiver type.
Receiver::Receiver(const std::string& s) noexcept
{
  this->copy_from_str(s);
}

/// Copy constructor.
Receiver::Receiver(const Receiver& rhs) noexcept
{
  std::memcpy(name_, rhs.name_, receiver_details::receiver_max_bytes);
}

/// Assignment operator.
Receiver& Receiver::operator=(const Receiver& rhs) noexcept
{
  if (this!=&rhs)
  {
    std::memcpy(name_, rhs.name_, receiver_details::receiver_max_bytes);
  }
  return *this;
}

/// Assignment operator (from c-string).
Receiver& Receiver::operator=(const char* c) noexcept
{
#ifdef DEBUG
  printf("\n[DEBUG MSG]:: Using Receiver& operator=(const char* c)\n");
#endif
  this->copy_from_cstr(c);
  return *this;
}

/// Assignment operator (from std::string).
Receiver& Receiver::operator=(const std::string& s) noexcept
{
#ifdef DEBUG
  printf("\n[DEBUG MSG]:: Using Receiver& operator=(const std::string)\n");
#endif
  this->copy_from_str(s);
  return *this;
}

/// Equality operator.
bool Receiver::operator==(const Receiver& rhs) noexcept
{
  return ( !std::strncmp(name_ ,rhs.name_ , 
        receiver_details::receiver_max_bytes) );
}

/// Pointer to receiver name.
/// \warning  This is not a c-string! no terminating '\0' char.
inline 
const char* Receiver::name() const noexcept
{
  return name_;
}

/// Receiver name as string.
std::string Receiver::toString() const noexcept
{
  return std::string(name_, receiver_details::receiver_max_chars);
}

/// set all chars to '\0'.
inline 
void Receiver::nullify() noexcept
{
  std::memset(name_, '\0', receiver_details::receiver_max_chars);
}

/// copy from an std::string (to name).
inline 
void Receiver::copy_from_str(const std::string& s) noexcept
{
  this->nullify();
  // std::basic_string::copy can only throw if max_chars >= 0, i.e. NEVER !
  s.copy(name_, receiver_details::receiver_max_chars, 0);
}

/// copy from a c-string.
inline
void Receiver::copy_from_cstr(const char* c) noexcept
{
  this->nullify();
  std::memcpy(name_, c, std::min(std::strlen(c), 
        receiver_details::receiver_max_chars)*sizeof(char));
}
