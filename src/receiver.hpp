#ifndef _GNSS_RECEIVER_
#define _GNSS_RECEIVER_

#include <string>
#include <cstring>

/**
 * \file      receiver.hpp
 *
 * \version   0.10
 *
 * \author    xanthos@mail.ntua.gr <br>
 *            danast@mail.ntua.gr
 *
 * \date      MAY 2015
 *
 * \brief     Receiver Class for GNSS.
 *
 * \details
 *
 * \note
 *
 * \todo      If the static-assert block is not commented out (it shouldn't),
 *            doxygen fails to parse the rest of the file. It is temporarily
 *            commented out, but there should be a workaround.
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

/// Maximum size of a char array, holding any receiver type
constexpr std::size_t _RECEIVER_MAX_SIZE_ { 20 };

/// Maximum size a char array, holding any receiver in bytes
constexpr std::size_t _RECEIVER_MAX_SIZE_BYTES_
{ _RECEIVER_MAX_SIZE_ * sizeof(char) };

/* -----------------------------------------------------------------
 * STATIC ASSERT BLOCK.
 * ------------------------------------------------------------------
/// Better to be safe than sorry ...
static_assert(_RECEIVER_MAX_SIZE_BYTES_ >= _RECEIVER_MAX_SIZE_,
              "Receiver size in bytes < Receiver size ?!?");
 * ------------------------------------------------------------------
 */

/** \details  This class holds a GNSS receiver. Any receiver is represesnted
 *            by a string of max _RECEIVER_MAX_SIZE_ columns. One space between
 *            manufacturer name and model name. Allowed in model name: 'A-Z'
 *            and '0-9' and space and '-_+'.
 *            Example: 'ASHTECH 3DF-XXIV', 'TPS ODYSSEY_E'.
 *
 * Reference: <a href="https://igscb.jpl.nasa.gov/igscb/station/general/rcvr_ant.tab">
 *            IGS rcvr_ant.tab</a>
 */
class Receiver
{
public:

  /// Default constructor.
  Receiver() noexcept {};

  /// Constructor from receiver type.
  explicit Receiver (const char*) noexcept;

  /// Constructor from receiver type.
  explicit Receiver (const std::string&) noexcept;

  /// Copy constructor.
  Receiver(const Receiver& rhs) noexcept
  {
    std::memcpy(name_,rhs.name_,_RECEIVER_MAX_SIZE_BYTES_);
  }

  /// Move constructor.
  Receiver(Receiver&&) noexcept = default;

  /// Assignment operator.
  Receiver& operator=(const Receiver& rhs) noexcept
  {
    if (this!=&rhs)
    {
      std::memcpy(name_,rhs.name_,_RECEIVER_MAX_SIZE_BYTES_);
    }
    return *this;
  }

  /// Move assignment operator.
  Receiver& operator=(Receiver&&) noexcept = default;

  /// Equality operator.
  bool operator==(const Receiver& rhs) noexcept
  {
    return ( !std::strncmp(name_,rhs.name_,_RECEIVER_MAX_SIZE_) );
  }

  /// Destructor.
  ~Receiver() noexcept = default;

  /// Pointer to receiver name.
  inline const char* name() const noexcept
  {
    return name_;
  }

  /// Receiver name as string.
  inline std::string toString() const noexcept
  {
    return std::string(name_,_RECEIVER_MAX_SIZE_);
  }

private:
  char name_[_RECEIVER_MAX_SIZE_]; ///< Receiver type name.
}; // end Receiver

} // end ngpt

#endif
