#ifndef _GNSS_RECEIVER_
#define _GNSS_RECEIVER_

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
 * \details   This file declarea a Receiver Class for GNSS. Reference is
 *            the IGS rcvr_ant.tab (see \cite rcvr_ant ).
 *
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

  /// Namespace to hide receiver specific details.
  namespace receiver_details
  {
    /// Maximum number of characters describing a GNSS receiver.
    /// \warning The character '\0' is not included.
    ///
    constexpr std::size_t receiver_max_chars { 20 };

    /// Maximum size of a char array, holding any receiver (in bytes).
    /// \warning The character '\0' is not included.
    ///
    constexpr std::size_t receiver_max_bytes
    { receiver_max_chars * sizeof(char) };

    /// Better be safe than sorry ...
    static_assert(receiver_max_bytes >= receiver_max_chars, 
                  "Receiver size in bytes < Receiver size ?!?");
  }

/** \details  This class holds a GNSS receiver. Any receiver is represented
 *            by an array of chars of maximum receiver_details::receiver_max_chars 
 *            elements. One space between manufacturer name and model name.
 *
 *            The class has a (single) member holding a \c cstring, representing
 *            the model name. The model name can be of maximum 
 *            receiver_details::receiver_max_chars characters. One more char is
 *            reserved (the last one) to add the '\0' character. So the size of the
 *            member char-array is actually receiver_details::receiver_max_chars + 1.
 *
 *            Never mess with the last character! You can't if you use the
 *            interface.
 *
 *            Allowed in model name: 
 *            -# 'A-Z'
 *            -# '0-9' 
 *            -# space and 
 *            -# '-_+'.
 *            Example: 'ASHTECH 3DF-XXIV', 'TPS ODYSSEY_E'.
 *
 * \todo      -# Sould the move constructors be enabled ?
 *
 * \bug       -# The function validate does not work as expected 
 *               \see Receiver::validate().
 *               For example the following: 
 *               \code{.cpp}
 *               Receiver rec {" GEODETIC III L1/L2   "};
 *               bool ok = rec.validate();
 *               \endcode
 *               Will set \c ok to \c true , even though the character \c '/'
 *               is not allowed.
 *
 * \cite         rcvr_ant
 */
class Receiver
{
public:

  /// Default constructor.
  Receiver() noexcept;

  /// Constructor from receiver type.
  explicit Receiver(const char*) noexcept;

  /// Constructor from receiver type.
  explicit Receiver(const std::string&) noexcept;

  /// Copy constructor.
  Receiver(const Receiver&) noexcept;

  /// Move constructor.
  Receiver(Receiver&&) noexcept = default;

  /// Assignment operator.
  Receiver& operator=(const Receiver&) noexcept;

  /// Assignment operator (from c-string).
  Receiver& operator=(const char*) noexcept;

  /// Assignment operator (from std::string).
  Receiver& operator=(const std::string&) noexcept;

  /// Move assignment operator.
  Receiver& operator=(Receiver&&) noexcept = default;

  /// Equality operator.
  bool operator==(const Receiver&) noexcept;

  /// Destructor.
  ~Receiver() noexcept = default;

  /// Pointer to receiver name.
  inline const char* name() const noexcept;

  /// Receiver name as string.
  std::string toString() const noexcept;

  /// Validate the receiver model (according to IGS).
  bool validate() const;

private:

  /// Set all chars to '\0'.
  inline void nullify() noexcept;

  /// Copy from an std::string (to name).
  inline void copy_from_str(const std::string&) noexcept;

  /// Copy from a c-string.
  inline  void copy_from_cstr(const char*) noexcept;

  /// This array holds the receiver name; last char always '\0' !
  char name_[receiver_details::receiver_max_chars + 1];

}; // end Receiver

} // end ngpt

#endif
