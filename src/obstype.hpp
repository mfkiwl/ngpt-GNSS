#ifndef _GNSSOBS_HPP_
#define _GNSSOBS_HPP_

#include <vector>
#include <algorithm>
#include <iterator>
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
/// This is extracted from RINEX v3.02.
enum class OBSERVATION_TYPE : char 
{
  PSEUDORANGE,
  CARRIER_PHASE,
  DOPPLER,
  SIGNAL_STRENGTH,
  IONOSPHERE_PHASE_DELAY, /// see RINEX v3.02, sec. 5.12
  RECEIVER_CHANNEL_NUMBER /// see RINEX v3.02, sec. 5.13
};

/// Struct to hold observable attributes. See RINEX v3.02
/// A missing attribute (e.g. RINEX v2.xx is always denoted as '?').
struct Attribute
{
  explicit Attribute(char c) noexcept : _c(c) {};
  bool operator==(const Attribute& a) const noexcept {return (_c == a._c);}
  char _c;
};

/// Class to hold a raw, rinex observable.
/// This is a helper class, do not use in real code.
class _RawObs_ 
{
private:
  SATELLITE_SYSTEM _satsys;     ///< Satellite system.
  OBSERVATION_TYPE _obstype;    ///< Observation type.
  short int        _nfrequency; ///< Nominal Frequency band.
  Attribute        _attribute;  ///< Attribute.

public:
  /// Constructor from a full set.
  explicit 
  _RawObs_(SATELLITE_SYSTEM s,OBSERVATION_TYPE o,short int f,Attribute a) 
  noexcept
  :_satsys{s}, _obstype{o}, _nfrequency{f}, _attribute{a}
  {};

  /// Constructor from a full set where the attribute is a 'char'.
  explicit 
  _RawObs_(SATELLITE_SYSTEM s,OBSERVATION_TYPE o,short int f,char c) 
  noexcept
  :_satsys{s}, _obstype{o}, _nfrequency{f}, _attribute(Attribute{c})
  { };
  
  /// Equality operator / comparisson.
  bool operator==(const _RawObs_& rhs) const noexcept
  {
    return ( _satsys == rhs._satsys 
      && _obstype == rhs._obstype 
      && _nfrequency == rhs._nfrequency 
      && _attribute == rhs._attribute );
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
  ///        ngpt::SatelliteSystemTraits<SATELLITE_SYSTEM>.
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
typedef std::vector<CoefObsPair> COVec;

private:
  COVec _cov; ///< Vector containing pairs of coeficients and _RawObs_.

public:
  
  /// Constructor from a full set.
  explicit
  ObservationType(SATELLITE_SYSTEM s,OBSERVATION_TYPE o,short int f,
    Attribute a,double coef = 1.0e0)
  noexcept
  : _cov{ {coef, _RawObs_(s,o,f,a)} }
  {};
  
  /// Constructor from a full set where the attribute is a 'char'.
  explicit
  ObservationType(SATELLITE_SYSTEM s,OBSERVATION_TYPE o,short int f,char c,
    double coef = 1.0e0)
  noexcept
  : _cov{ {coef, _RawObs_(s,o,f,Attribute{c})} }
  {};
  
  /// Default copy constructor.
  /// \warning Dont know whay this is necesary !
#ifdef __clang__
  /// The noexcept does not work with clang 
  ObservationType(const ObservationType&) = default;
#else
  ObservationType(const ObservationType&) noexcept = default;
#endif
  
  /// Default destructor.
  ~ObservationType() noexcept = default;
  
  /// Default move constructor.
  /// \warning Dont know whay this is necesary !
#ifdef __clang__
  /// The noexcept does not work with clang 
  ObservationType(ObservationType&&) = default;
#else
  ObservationType(ObservationType&&) noexcept = default;
#endif
  
  /// Default assignment operator.
#ifdef __clang__
  ObservationType& operator=(const ObservationType&) = default;
#else
  ObservationType& operator=(const ObservationType&) noexcept = default;
#endif
  
  /// Default move assignment operator.
  ObservationType& operator=(ObservationType&&) noexcept = default;
  
  /// \brief Add an observation type. If the type already exists, then 
  ///        just alter the coefficient (to be previous + this one).
  ///
  std::size_t add_type(const _RawObs_& t,double coef = 1.0e0) noexcept
  {
    // Check if it (i.e. _RawObs_) already exists.
    auto it = std::find_if(std::begin(_cov), std::end(_cov),
      [&t](const /*auto C++14*/CoefObsPair& i) ->bool { return i.second == t; }
      );
    if (it == std::end(_cov))
      _cov.emplace_back(coef, t);
    else
      it->first += coef;

    return _cov.size();
  }

  /// \brief Add an observation type. If the type already exists, then just 
  ///        alter the coefficient.
  ///
  inline std::size_t add_type(SATELLITE_SYSTEM s,OBSERVATION_TYPE o,short int f,
    char c,double coef = 1.0e0) noexcept
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
}; // end ObservationType

} // end ngpt

#endif