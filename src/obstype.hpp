#ifndef _GNSSOBS_HPP_
#define _GNSSOBS_HPP_

#include <vector>
#include <type_traits>
#include <tuple>
#ifdef DEBUG
    #include <iostream>
    #include <string>
#endif
#include "satsys.hpp"

/*
 * \file
 *
 * \version
 *
 * \author  xanthos@mail.ntua.gr <br>
 *          danast@mail.ntua.gr
 *
 * \date    Sat 19 Dec 2015 03:57:29 PM EET 
 *
 * \brief   Define GNSS observation types.
 *
 * \details
 *
 * \note
 *
 * \todo Need to use compiler-dependent versions of defaulted copy/move 
 *       constructors for the class ObservationType. WTF?? At least get
 *       the directives right!
 *
 * \copyright Copyright © 2015 Dionysos Satellite Observatory, <br>
 * National Technical University of Athens. <br>
 * This work is free. You can redistribute it and/or modify it under
 * the terms of the Do What The Fuck You Want To Public License,
 * Version 2, as published by Sam Hocevar. See http://www.wtfpl.net/
 * for more details.
 *
 * <b><center><hr>
 * National Technical University of Athens <br>
 * Dionysos Satellite Observatory <br>
 * Higher Geodesy Laboratory <br>
 * http://dionysos.survey.ntua.gr
 * <hr></center></b>
 *
 */

namespace ngpt
{

/// Enumeration for known Obsrvation Types. This is extracted from 
/// \cite rnx303 .
enum class observable_type : char 
{
    pseudorange,
    carrier_phase,
    doppler,
    signal_strength,
    ionosphere_phase_delay, ///< \cite rnx303 , sec. 5.12
    receiver_channel_number ///< \cite rnx303 , sec. 5.13
};

/// Return the identifier of the input observable_type.
char
obstype_to_char(observable_type);

/// Return the observable_type represented by the given char.
observable_type
char_to_obstype(char);

/// Struct to hold observable attributes. See \cite rnx303 .
///
/// \note There is a special attribute with value '?', which describes an
///       unknown attribute.
///
class obs_attribute
{
public:
    explicit 
    obs_attribute(char c = '?')
    noexcept 
    : c_(c) 
    {}
    
    /// Assignment op; ust compare the underlying char.
    bool
    operator==(const obs_attribute& a)
    const noexcept 
    { return (c_ == a.c_); }

    /// A char as defined in \cite rnx303
    char c_;
};

namespace gnss_obs_details {

/**
 *  Class to hold a raw, rinex observable. This is a helper class, do not 
 *  use in real code.
 * 
 */ 
class _rawobs_
{

public:
#if __cplusplus > 201103L
    /// see the match template function for this.
    /// TODO should the template parameters of std::tuple be references ?
    std::tuple<satellite_system, observable_type, short int, obs_attribute> 
    tie()
    const noexcept
    { return std::tie(satsys_, obstype_, nfrequency_, attribute_); }
#else
    /// see the match template function for this.
    template<typename>
    friend
    struct expose_guts;
#endif

private:
    satellite_system satsys_;     ///< satellite system.
    observable_type  obstype_;    ///< observation type.
    short int        nfrequency_; ///< Nominal Frequency band.
    obs_attribute    attribute_;  ///< Attribute.

public:

    /// Constructor from a full set.
    explicit 
    _rawobs_(satellite_system s,
             observable_type o,
             short int f,
             obs_attribute a)
    noexcept
    : satsys_    { s }, 
      obstype_   { o }, 
      nfrequency_{ f }, 
      attribute_ { a }
    {}

    /// Constructor from a full set where the attribute is a 'char'.
    explicit 
    _rawobs_(satellite_system s,
             observable_type o,
             short int f,
             char c)
    noexcept
    : satsys_    { s }, 
      obstype_   { o }, 
      nfrequency_{ f }, 
      attribute_ ( obs_attribute{c} )
    {}

    /// Constructor from a RINEX string
    explicit
    _rawobs_(const std::string& str, satellite_system* s = nullptr)
    { this->set_from_str(str, s); }
    
    explicit
    _rawobs_(const std::string& str, char* s = nullptr)
    { this->set_from_str(str, s); }
    
    /// Get/Set the satellite system.
    satellite_system&
    satsys() noexcept 
    { return satsys_; }

    /// Get/Set the observation type.
    observable_type&
    obstype() noexcept 
    { return obstype_; }
    
    /// Get/Set the frequency band.
    short int&
    band() noexcept 
    { return nfrequency_; }
    
    /// Get/Set the attribute
    obs_attribute&
    attribute() noexcept 
    { return attribute_; }
    
    /// Get the satellite system
    satellite_system
    satsys() 
    const noexcept 
    { return satsys_; }

    /// Get the observation type
    observable_type
    obstype() 
    const noexcept 
    { return obstype_; }
    
    /// Get the frequency band
    short int
    band() 
    const noexcept 
    { return nfrequency_; }
    
    /// Get the attribute
    obs_attribute
    attribute() const noexcept 
    { return attribute_; }

    /// Equality operator / comparisson. This is **strict** comparisson; most
    /// of the time not very helpful.
    bool 
    operator==(const _rawobs_& rhs)
    const noexcept
    {
        return (       satsys_ == rhs.satsys_
                &&    obstype_ == rhs.obstype_
                && nfrequency_ == rhs.nfrequency_
                &&  attribute_ == rhs.attribute_ );
    }

    /// Default copy constructor.
    _rawobs_(const _rawobs_&) noexcept = default;

    /// Default destructor.
    ~_rawobs_() noexcept = default;

    /// Default move constructor.
    _rawobs_(_rawobs_&&) noexcept = default;

    /// Default assignment operator.
    _rawobs_& operator=(const _rawobs_&) noexcept = default;

    /// Default move assignment operator.
    _rawobs_& operator=(_rawobs_&&) noexcept = default;

    /// Set from RINEX identifying string
    void
    set_from_str(const std::string&, ngpt::satellite_system* s = nullptr);
    
    /// Set from RINEX identifying string (providing a char as 
    /// satsys identifier)
    void
    set_from_str(const std::string&, char*);

    /**
     *  \brief Return the nominal frequency for this instance.
     * 
     *  \note  This may throw in case the instance is not well formed (i.e. the
     *         frequency band is not valid for this satellite system).
     *         To see the valid 'pairs' of Satellite System / Frequency Bands,
     *         see the function ngpt::nominal_frequency() and/or
     *         ngpt::SatelliteSystemTraits<satellite_system>.
     */ 
    double 
    nominal_frequency()
    const
    { return ngpt::nominal_frequency(nfrequency_, satsys_); }

#ifdef DEBUG
    friend
    std::ostream&
    operator<<(std::ostream& os, const _rawobs_& obs)
    {
        char str[5];
        *str     = ngpt::satsys_identifier( obs.satsys_ );
        *(str+1) = ngpt::obstype_to_char( obs.obstype_ );
        *(str+2) = std::to_string( obs.nfrequency_ ).at(0);
        *(str+3) = obs.attribute_.c_;
        *(str+4) = '\0';
        os << std::string( str );
        return os;
    }
#endif

}; // _rawobs_

#if __cplusplus > 201103L
    /// C++14 
    template<typename... Args>
    bool
    match(const _rawobs_& x1, const _rawobs_& x2)
    noexcept
    {
        return std::make_tuple(std::get<Args>(x1.tie())...) ==
               std::make_tuple(std::get<Args>(x2.tie())...);
    }
#else
    /// C++11
    template<typename>
        struct expose_guts;

    template<>
        struct expose_guts<satellite_system>
    { 
        static const satellite_system&
        get(const _rawobs_& x)
        noexcept
        { return x.satsys_; } 
    };

    template<>
        struct expose_guts<observable_type>
    {
        static const observable_type&
        get(const _rawobs_& x)
        noexcept
        { return x.obstype_; }
    };

    template<>
        struct expose_guts<short int>
    {
        static const short int&
        get(const _rawobs_& x)
        noexcept
        { return x.nfrequency_; }
    };

    template<>
        struct expose_guts<obs_attribute>
    { 
        static const obs_attribute&
        get(const _rawobs_& x)
        noexcept
        { return x.attribute_; }
    };

    template<typename ...Args>
    bool
    match(const _rawobs_& x1, const _rawobs_& x2)
    {
        return std::tie(expose_guts<Args>::get(x1)...) ==
               std::tie(expose_guts<Args>::get(x2)...);
    }
#endif

} // end gnss_obs_details

/**
 *  \brief This class can hold any GNSS observable, including linear 
 *         combinations of observables.
 *         All instances are analyzed to pairs of _rawobs_ and corresponding
 *         coefficients. Note that it is possible to make any linear
 *         combination, including between different satellite systems.
 *
 *  \example test_obstype.cpp
 */ 
class observation_type
{

typedef std::pair<double, gnss_obs_details::_rawobs_> CoefObsPair;
typedef std::vector<CoefObsPair>                      covec;

private:
    covec cov_; ///< Vector containing pairs of coeficients and 
                ///  gnss_obs_details::_rawobs_.
    
    /// \brief Add an observation type. If the type already exists, then 
    ///        just alter the coefficient (to be previous + this one).
    ///
    std::size_t
    add_type(const gnss_obs_details::_rawobs_& t, double coef = 1.0e0)
    noexcept;

public:

    // TODO Provide a validate function using satellite_system_traits

    observation_type() 
    noexcept
    {}

    /// Constructor from a full set.
    explicit
    observation_type(satellite_system s, 
                     observable_type o, 
                     short int f,
                     obs_attribute a, 
                     double coef = 1.0e0)
    noexcept
    : cov_( 1, std::make_pair(coef, gnss_obs_details::_rawobs_(s, o, f, a)) )
    {}

    /// Constructor from a full set where the attribute is a 'char'.
    /// \todo is this correct initialization ?
    explicit
    observation_type(satellite_system s, 
                     observable_type o,
                     short int f,
                     char c,
                     double coef = 1.0e0)
    noexcept
    : cov_( 1, std::make_pair
                (coef, gnss_obs_details::_rawobs_(s, o, f, obs_attribute{c})) )
    {}

    /// Constructor from a list of RINEX strings
    explicit
    observation_type(const std::string& str, satellite_system* s = nullptr)
    : cov_( 1, std::make_pair(1.0e0, gnss_obs_details::_rawobs_(str, s)) )
    {}

    /// Default copy constructor.
    observation_type(const observation_type&)
    noexcept(std::is_nothrow_copy_constructible<covec>::value) = default;

    /// Default destructor.
    ~observation_type() noexcept = default;

    /// Default move constructor.
    observation_type(observation_type&&) 
    noexcept(std::is_nothrow_move_constructible<covec>::value) = default;

    /// Default assignment operator.
    observation_type&
    operator=(const observation_type&) 
    noexcept(std::is_nothrow_copy_assignable<covec>::value) = default;

    /// Default move assignment operator.
    observation_type&
    operator=(observation_type&&)
    noexcept(std::is_nothrow_move_assignable<covec>::value) = default;

    /// \brief Add an observation type. If the type already exists, then just 
    ///        alter the coefficient.
    ///
    std::size_t 
    add_type(satellite_system s, 
             observable_type o, 
             short int f, 
             char c, 
             double coef = 1.0e0)
    noexcept
    { return this->add_type(gnss_obs_details::_rawobs_
                                (s, o, f, obs_attribute{c}), coef); }

    /// \brief Add an observation type. If the type already exists, then just 
    ///        alter the coefficient.
    ///
    std::size_t
    add_type(const observation_type& t)
    noexcept
    {
        for (const auto& i : t.cov_) {
            this->add_type(i.second, i.first);
        }
        return cov_.size();
    }

    /// \brief Return the frequency of the observable.
    ///
    /// \note  This may throw in case the instance is not well formed (i.e. the
    ///        frequency band is not valid for this satellite system).
    ///
    double
    frequency()
    const
    {
        double freq { .0e0 };
        for (auto& i : cov_) {
            freq += ( i.second.nominal_frequency() * i.first );
        }
        return freq;
    }

    /// \brief Return the number of raw (fundamental) observables
    std::size_t
    raw_obs_num()
    const noexcept
    { return cov_.size(); }

    /// \brief   Access the underlying gnss_obs_details::_rawobs_ instances via an index.
    ///
    /// \warning Be extra careful with this! No range check is performed
    ///          to validate the index
    ///
    ngpt::gnss_obs_details::_rawobs_&
    raw_obs(std::size_t i)
    { return cov_[i].second; }

    ngpt::gnss_obs_details::_rawobs_
    raw_obs(std::size_t i)
    const
    { return std::get<1>( cov_[i] ); }
    
#ifdef DEBUG
    friend
    std::ostream&
    operator<<(std::ostream& os, const observation_type& obs)
    {
        os << "[" ;
        for ( auto i : obs.cov_ ) {
            os <<  std::showpos << i.first << "*" << i.second ;
        }
        os << "]" ;
        return os;
    }
#endif

}; // end ObservationType

} // end ngpt

#endif
