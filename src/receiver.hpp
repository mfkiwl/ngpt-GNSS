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
 * \date      Sun 20 Dec 2015 01:49:52 AM EET 
 *
 * \brief     receiver Class for GNSS.
 *
 * \details   This file declares a receiver Class for GNSS. Reference is
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
                  "receiver size in bytes < receiver size ?!?");
    }

/** \class   receiver
 *
 * \details  This class holds a GNSS receiver. Any receiver is represented
 *           by an array of chars of maximum receiver_details::receiver_max_chars 
 *           elements. One space between manufacturer name and model name.
 *
 *           The class has a (single) member holding a \c cstring, representing
 *           the model name. The model name can be of maximum 
 *           receiver_details::receiver_max_chars characters. One more char is
 *           reserved (the last one) to add the '\0' character. So the size of the
 *           member char-array is actually receiver_details::receiver_max_chars + 1.
 *
 *           Never mess with the last character! You can't if you use the
 *           interface.
 *
 *           Allowed in model name: 
 *            -# 'A-Z'
 *            -# '0-9' 
 *            -# space and 
 *            -# '-_+'.
 *           
 *           Example: 'ASHTECH 3DF-XXIV', 'TPS ODYSSEY_E'.
 *
 * \todo     -# Sould the move constructors be enabled ?
 *
 * \bug      -# The function validate does not work as expected 
 *               \see receiver::validate().
 *               For example the following: 
 *               \code{.cpp}
 *               receiver rec {" GEODETIC III L1/L2   "};
 *               bool ok = rec.validate();
 *               \endcode
 *               Will set \c ok to \c true , even though the character \c '/'
 *               is not allowed.
 *
 * References \cite rcvr_ant
 *
 * \example   test_receiver.cpp
 */
class receiver
{
public:

    /// Default constructor.
    receiver() noexcept;

    /// Constructor from receiver type.
    explicit receiver(const char*) noexcept;

    /// Constructor from receiver type.
    explicit receiver(const std::string&) noexcept;

    /// Copy constructor.
    receiver(const receiver&) noexcept;

    /// Move constructor.
    receiver(receiver&&) noexcept = default;

    /// Assignment operator.
    receiver& operator=(const receiver&) noexcept;

    /// Assignment operator (from c-string).
    receiver& operator=(const char*) noexcept;

    /// Assignment operator (from std::string).
    receiver& operator=(const std::string&) noexcept;

    /// Move assignment operator.
    receiver& operator=(receiver&&) noexcept = default;

    /// Equality operator.
    bool operator==(const receiver&) noexcept;

    /// Destructor.
    ~receiver() noexcept = default;

    /// Pointer to receiver name.
    inline const char* name() const noexcept;

    /// receiver name as string.
    std::string toString() const noexcept;

    /// Validate the receiver model (according to IGS).
    /*
    bool validate() const;
    */

private:

    /// Set all chars to '\0'.
    inline void nullify() noexcept;

    /// Copy from an std::string (to name).
    inline void copy_from_str(const std::string&) noexcept;

    /// Copy from a c-string.
    inline  void copy_from_cstr(const char*) noexcept;

    /// This array holds the receiver name; last char always '\0' !
    char name_[receiver_details::receiver_max_chars + 1];

}; // end receiver

} // end ngpt

#endif
