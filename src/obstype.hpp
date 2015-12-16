#ifndef _GNSSOBS_HPP_
#define _GNSSOBS_HPP_

#include <vector>
#include <algorithm>
#include <iterator>
#include <type_traits>
#include "satsys.hpp"

/*
 * \file
 *
 * \version
 *
 * \author xanthos@mail.ntua.gr <br>
 *         danast@mail.ntua.gr
 *
 * \date
 *
 * \brief
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

/// Enumeration for known Obsrvation Types.
/// This is extracted from \cite rnx303 .
enum class observation_type : char 
{
  pseudorange,
  carrier_phase,
  doppler,
  signal_strength,
  ionosphere_phase_delay, /// \cite rnx303 , sec. 5.12
  receiver_channel_number /// \cite rnx303 , sec. 5.13
};

/// Struct to hold observable attributes. See \cite rnx303
struct Attribute
{
    explicit Attribute(char c) noexcept 
        : _c(c) 
    {}
    bool operator==(const Attribute& a) const noexcept 
    {
        return (_c == a._c);
    }
    char _c;
};

/// Class to hold a raw, rinex observable.
/// This is a helper class, do not use in real code.
class _RawObs_
{

private:
    satellite_system _satsys;     ///< satellite system.
    observation_type _obstype;    ///< observation type.
    short int        _nfrequency; ///< Nominal Frequency band.
    Attribute        _attribute;  ///< Attribute.

public:

    /// Constructor from a full set.
    explicit 
    _RawObs_(satellite_system s, observation_type o, short int f, Attribute a)
    noexcept
        :_satsys{s}, _obstype{o}, _nfrequency{f}, _attribute{a}
    {}

    /// Constructor from a full set where the attribute is a 'char'.
    explicit 
    _RawObs_(satellite_system s, observation_type o, short int f, char c) 
    noexcept
        :_satsys{s}, _obstype{o}, _nfrequency{f}, _attribute(Attribute{c})
    {}
    
    satellite_system&
    satsys()    noexcept { return _satsys; }

    observation_type&
    obstype()   noexcept { return _obstype; }
    
    short int&
    band()      noexcept { return _nfrequency; }
    
    Attribute&
    attribute() noexcept { return _attribute; }
    
    satellite_system
    satsys()    const noexcept { return _satsys; }

    observation_type
    obstype()   const noexcept { return _obstype; }
    
    short int
    band()      const noexcept { return _nfrequency; }
    
    Attribute
    attribute() const noexcept { return _attribute; }

    /// Equality operator / comparisson.
    bool operator==(const _RawObs_& rhs) const noexcept
    {
        return (       _satsys == rhs._satsys 
                &&    _obstype == rhs._obstype 
                && _nfrequency == rhs._nfrequency 
                &&  _attribute == rhs._attribute );
    }

    /// Default copy constructor.
    _RawObs_(const _RawObs_&) noexcept = default;

    /// Default destructor.
    ~_RawObs_() noexcept = default;

    /// Default move constructor.
    _RawObs_(_RawObs_&&) noexcept = default;

    /// Default assignment operator.
    _RawObs_& operator=(const _RawObs_&) noexcept = default;

    /// Default move assignment operator.
    _RawObs_& operator=(_RawObs_&&) noexcept = default;

    /// \brief Return the nominal frequency for this instance.
    ///
    /// \note  This may throw in case the instance is not well formed (i.e. the
    ///        frequency band is not valid for this satellite system).
    ///        To see the valid 'pairs' of Satellite System / Frequency Bands,
    ///        see the function ngpt::nominal_frequency() and/or
    ///        ngpt::SatelliteSystemTraits<satellite_system>.
    ///
    inline double nominal_frequency() const
    {
        return ngpt::nominal_frequency(_nfrequency, _satsys);
    }
};

/// \brief This class can hold any GNSS observable, including linear 
///        combinations of observables.
///        All instances are analyzed to pairs of _RawObs_ and corresponding
///        coefficients. Note that it is possible to make any linear
///        combination, including between different satellite systems.
///
class ObservationType
{

typedef std::pair<double, _RawObs_> CoefObsPair;
typedef std::vector<CoefObsPair>    covec;

private:
    covec _cov; ///< Vector containing pairs of coeficients and _RawObs_.

public:

    ObservationType() noexcept {}

    /// Constructor from a full set.
    explicit
    ObservationType(satellite_system s, observation_type o, short int f,
                    Attribute a, double coef = 1.0e0)
    noexcept
        : _cov{ {coef, _RawObs_(s, o, f, a)} }
    {}

    /// Constructor from a full set where the attribute is a 'char'.
    explicit
    ObservationType(satellite_system s, observation_type o, short int f, char c,
                    double coef = 1.0e0)
    noexcept
        : _cov{ {coef, _RawObs_(s, o, f, Attribute{c})} }
    {}

    /// Default copy constructor.
    /// \todo The following decleration :
    ///       ObservationType(const ObservationType&) noexcept = default;
    ///       does not work with gcc 4.8.3. This probably means that the
    ///       vector<T> copy constructor is not noexcept. To fix this, use:
    ObservationType(const ObservationType&)
    noexcept( std::is_nothrow_copy_constructible<covec>::value ) = default;

    /// Default destructor.
    ~ObservationType() noexcept = default;

    /// Default move constructor.
    ObservationType(ObservationType&&) noexcept = default;

    /// Default assignment operator.
    /// \note See ObservationType(const ObservationType&) for the reason
    ///       of the noexcept expression.
    ObservationType& operator=(const ObservationType&) 
    noexcept( std::is_nothrow_copy_assignable<covec>::value ) = default;

    /// Default move assignment operator.
    /// \note See ObservationType(const ObservationType&) for the reason
    ///       of the noexcept expression.
    ObservationType& operator=(ObservationType&&)
    noexcept( std::is_nothrow_move_assignable<covec>::value ) = default;

    /// \brief Add an observation type. If the type already exists, then 
    ///        just alter the coefficient (to be previous + this one).
    ///
    std::size_t add_type(const _RawObs_& t, double coef = 1.0e0)
    noexcept
    {
        // Check if it (i.e. _RawObs_) already exists.
        auto it = std::find_if(std::begin(_cov), std::end(_cov),
                [&t](const /*auto C++14*/CoefObsPair& i) ->bool { 
                return i.second == t; 
                });
        if (it == std::end(_cov)) {
            _cov.emplace_back(coef, t);
        } else {
            it->first += coef;
        }

        return _cov.size();
    }

    /// \brief Add an observation type. If the type already exists, then just 
    ///        alter the coefficient.
    ///
    std::size_t add_type(satellite_system s, observation_type o, 
                         short int f, char c, double coef = 1.0e0)
    noexcept
    {
        return this->add_type(_RawObs_ (s,o,f,Attribute{c}), coef);
    }

    /// \brief Add an observation type. If the type already exists, then just 
    ///        alter the coefficient.
    ///
    std::size_t add_type(const ObservationType& t) noexcept
    {
        for (const auto& i : t._cov)
            this->add_type(i.second, i.first);
        return _cov.size();
    }

    /// \brief Return the frequency of the observable.
    ///
    /// \note  This may throw in case the instance is not well formed (i.e. the
    ///        frequency band is not valid for this satellite system).
    ///
    double frequency() const
    {
        double freq { .0e0 };
        for (auto& i : _cov)
            freq += ( i.second.nominal_frequency() * i.first );
        return freq;
    }

    /// \bried Return the number of raw (fundamental) observables
    std::size_t raw_obs_num()
    const noexcept
    {
        return _cov.size();
    }

    /// \brief   Access the underlying _RawObs_ instances via an index.
    ///
    /// \warning Be extra careful with this! No range check is performed
    ///          to validate the index
    ///
    ngpt::_RawObs_&
    raw_obs(std::size_t i)
    {
        //return std::get<1>( _cov[i] );
        return _cov[i].second;
    }
    ngpt::_RawObs_
    raw_obs(std::size_t i)
    const
    {
        return std::get<1>( _cov[i] );
    }
    
}; // end ObservationType

} // end ngpt

#endif
