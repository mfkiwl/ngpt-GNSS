#ifndef __GRID_HPP__
#define __GRID_HPP__

#include <tuple>
#include <cstring>

/**
 * \file
 *
 * \version
 *
 * \author    xanthos@mail.ntua.gr <br>
 *            danast@mail.ntua.gr
 *
 * \date
 *
 * \brief
 *
 * \details
 *
 * \note
 *
 * \todo
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

template<bool Do>
class RangeChecker
{};

template<T>
class RangeChecker<false>
{
  static constexpr bool validate_index() const noexcept { return true; }
};
template<T>
class RangeChecker<true>
{
  static constexpr bool
  validate_index(std::size_t pts,  val)
  const noexcept { return val >=0 ; }
};

/// \class   TickAxisImpl
/// \details This class is used to represent a tick axis, starting from tick
///          with value start_, ending to tick stop_ with a step size of 
///          step_. Note that stop_ does not have to be greater than start_,
///          i.e. the axis can be in either ascending or descending order.
///
template<typename T>
class TickAxisImpl
{
private:
  T           start_;
  T           stop_;
  T           step_;
  std::size_t npts;

public:
  explicit TickAxisImpl(T s,  T e, T d) noexcept
  : start_{s}, stop_{e}, step_{d},
  npts_{static_cast<std::size_t>((e-s)/d)+1}
  {};

  virtual ~TickAxisImpl() {};

  TickAxisImpl(const TickAxisImpl&) noexcept = default;

  TickAxisImpl& operator=(const TickAxisImpl&) noexcept = default;
  
  TickAxisImpl(TickAxisImpl&&) noexcept = default;
  
  TickAxisImpl& operator=(TickAxisImpl&&) noexcept = default;
  
  T min() const noexcept { return start_; }
  
  T max() const noexcept { return stop_; }

  T step() const noexcept { return step_; }

  bool is_ascending() const noexcept { return stop_-start_ > 0; }

  std::size_t size() const noexcept { return npts_; }
  
  virtual auto neighbor_nodes(T x)
  {
    // find tick on the left.
    T tmp { (x - start_) / step_ };
    if (tmp < static_cast<T>(0) || tmp >= npts_) {
      throw std::out_of_range (
        "ERROR. PcvGrid_Noazi::neighboring_cells() -> out_of_range !!");
     }
    std::size_t l_idx { static_cast<std::size_t>(tmp) };

    // assuming ascending grid
    std::size_t r_idx { l_idx + 1 };
    if (r_idx >= npts_) {
      throw std::out_of_range (
        "ERROR. PcvGrid_Noazi::neighboring_cells() -> out_of_range !!");
    }

    return std::make_tuple{
      l_idx, l_idx*step_ + start_, r_idx, r_idx*step_ + start_,
    }
  }/*
  S linear_interpolation(T x)
  {
    auto t { this->neighboring_cells(x) };
    return std::get<1>(t) + (std::get<3>(t) -std::get<1>(t)) *
      static_cast<S>((x-std::get<0>(t))/(std::get<2>(t)-std::get<0>(t)));
  }*/
  auto nearest_neighbor(T x) noexcept
  {
    std::size_t idx { static_cast<std::size_t>((x+(step_/(T)2)-start_)/step_) };
    return idx < npts_ ?
      std::make_tuple{idx, idx*step_+start_}:
      std::make_tuple{npts_-1, stop_};
  }

};

template<typename T, bool RangeCheck>
class TickAxis : TickAxisImpl<T>
{
public:
  explicit TickAxis(T start, T stop, T step) noexcept
  :TickAxisImpl<T>(start,stop,step){};
};

template<typename T, true>
class TickAxis : TickAxisImpl<T>
{
public:
  explicit TickAxis(T start, T stop, T step) noexcept
  :TickAxisImpl<T>(start,stop,step){};
  
  auto neighbor_nodes(T x)
  {
    // find tick on the left.
    T tmp { (x - start_) / step_ };
    if (tmp < static_cast<T>(0) || tmp >= npts_) {
      throw std::out_of_range (
        "ERROR. PcvGrid_Noazi::neighboring_cells() -> out_of_range !!");
     }
    std::size_t l_idx { static_cast<std::size_t>(tmp) };

    // assuming ascending grid
    std::size_t r_idx { l_idx + 1 };
    if (r_idx >= npts_) {
      throw std::out_of_range (
        "ERROR. PcvGrid_Noazi::neighboring_cells() -> out_of_range !!");
    }

    return std::make_tuple{
      l_idx, l_idx*step_ + start_, r_idx, r_idx*step_ + start_,
    }
  }/*
}

template<typename T, false>
class TickAxis : TickAxisImpl<T>
{
public:
  explicit TickAxis(T start, T stop, T step) noexcept
  :TickAxisImpl<T>(start,stop,step){};
}

// --------------------------------------------- //
class AziGridImpl<typename T, typename S>
{
public:
  AziGridImpl(T s1,  T e1, T d1, T s2,  T e2, T d2) noexcept
 : xaxis_{s1, e1, d1}, yaxis_{s2, e2, d2}{};
  ~AziGridImpl() { data_ = nullptr; }
  AziGridImpl(const AziGridImpl&) noexcept = default;
  AziGridImpl& operator=(const AziGridImpl&) noexcept = default;
  AziGridImpl(AziGridImpl&&) noexcept = default;
  AziGridImpl& operator=(AziGridImpl&&) noexcept = default;
  auto neighbor_nodes(T x, T y)                              // x0,  y0,  x1,  y1
  {
    auto t1 { xaxis_.neighbor_nodes(x) };
    auto t2 { yaxis_.neighbor_nodes(y) };
    return std::tuple_cat(t1, t2);
  }
  /*S linear_interpolation(T x)
  {
    auto t { this->neighboring_cells(x) };
    return std::get<1>(t) + (std::get<3>(t) -std::get<1>(t)) *
    static_cast<S>((x-std::get<0>(t))/(std::get<2>(t)-std::get<0>(t)));
  }*/
  auto nearest_neighbor(T x, T y) noexcept
  {
    auto tx { xaxis_.nearest_neighbor(x) };
    auto ty { yaxis_.nearest_neighbor(y) };
    ///  HERE ------------------------------------ //
  }
private:
  NoAziGridImpl<T, S> xaxis_;
  NoAziGridImpl<T, S> yaxis_;
};

class AntennaPattern
{
private:
  typedef float  grid_t; /// Type of data describing the grid nodes.
  typedef double data_t; /// Type of data describing the grid values.
private:
  NoAziGridImpl<grid_t, data_t> noazi_pcv_grid_; ///< NOAZI pattern; one for each observation type.
  std::vector<ngpt::ObservationType> otypes_;   ///< The observations types for wich we have pcv grids.
  std::vector<double> neu_;                     ///< North, east and up pc corrections.
  char calibration_method_[20];                             ///< Calibration method.

public:
  explicit AntennaPattern(grid_t zen1, grid_t zen2, grid_t dzen,
    grid_t dazi = 0, char* c = nullptr, int freqs = 2)
  noexcept
 :noazi_pcv_grid_(zen1, zen2, dzen),otypes_{}, neu_{}
  {
    otypes_.reserve(freqs);
    neu_.reserve(freqs);
    std::memset(c, ' ', 20)
    if (c) {
      std::size_t sz { std::strlen(c) };
      if (sz > 20) {
        sz = 20;
      }
      std::memcpy(calibration_method_, c, sz);
    }
  }
};

#endif
