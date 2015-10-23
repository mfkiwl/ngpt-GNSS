#ifndef __NGPT_GRID_HPP__
#define __NGPT_GRID_HPP__

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

/// \class   TickAxisImpl
///
/// \details This class is used to represent a tick axis, starting from tick
///          with value start_, ending to tick stop_ with a step size of 
///          step_. Note that stop_ does not have to be greater than start_,
///          i.e. the axis can be in either ascending or descending order.
///
/// Template Parameters:
///          - T                : type of ticks (e.g. float)
///          - RangeCheck (bool): Enable/Disable range check
///
template<typename T, bool RangeCheck>
class TickAxisImpl
{
private:

  T           start_; ///< left-most tick point.
  T           stop_;  ///< right-most tick point.
  T           step_;  ///< step size.
  std::size_t npts_;  ///< number of tick points.

  /// do_range_check wraps the user's choice for RangeCheck.
  using do_range_check = std::integral_constant<bool, RangeCheck>;

  /// Given a point \p x lying on the TickAxis, compute the neighboring ticks
  /// (nodes), i.e. the left and right ticks inbetween of which lays point \p x.
  ///
  /// \parameter[in] x  The point for which we want the neighboring nodes/ticks.
  ///
  /// \return           A tuple containing 
  /// {left_tick_index, left_tick_value, right_tick_index, right_tick_value}
  /// 
  /// \note This function will perform range checks to see if the point \p x
  ///       does lay between the leftmost and rightmost ticks.
  ///
  /// \verbatim
  /// right-most             | x value
  ///   tick                 v
  ///   +-----+-----+-----+--x--+--...--+
  ///                                 left-most
  ///                                   tick
  /// \endverbatim
  ///
  auto
    neighbor_nodes_impl(T x, std::true_type)
    const noexcept( !do_range_check::value )
  {
    // find tick on the left.
    T tmp { (x - start_) / step_ };
    if (tmp < static_cast<T>(0) || tmp >= npts_) {
      throw std::out_of_range (
        "ERROR. TickAxisImpl<>::neighbor_nodes_impl -> out_of_range !!");
    }
    std::size_t l_idx { static_cast<std::size_t>(tmp) };

    // assuming ascending grid
    std::size_t r_idx { l_idx + 1 };
    if (r_idx >= npts_) {
      throw std::out_of_range (
        "ERROR. TickAxisImpl<>::neighbor_nodes_impl -> out_of_range !!");
    }

    return std::make_tuple(
        l_idx, l_idx*step_ + start_, r_idx, r_idx*step_ + start_ );
  }

  /// Does exactly the same as neighbor_nodes_impl(T x, std::true_type) but
  /// with no range checks.
  auto 
    neighbor_nodes_impl(T x, std::false_type)
    const noexcept( !do_range_check::value )
  {
    // find tick on the left.
    T tmp { (x - start_) / step_ };
    std::size_t l_idx { static_cast<std::size_t>(tmp) };

    // assuming ascending grid
    std::size_t r_idx { l_idx + 1 };

    return std::make_tuple(
      l_idx, l_idx*step_ + start_, r_idx, r_idx*step_ + start_ );
  }


public:
  explicit constexpr TickAxisImpl(T s,  T e, T d) noexcept
  : start_{s},
    stop_{e},
    step_{d},
    npts_{static_cast<std::size_t>((e-s)/d)+1}
  {};

  virtual ~TickAxisImpl() {};

  TickAxisImpl(const TickAxisImpl&) noexcept = default;

  TickAxisImpl& operator=(const TickAxisImpl&) noexcept = default;
  
  TickAxisImpl(TickAxisImpl&&) noexcept = default;
  
  TickAxisImpl& operator=(TickAxisImpl&&) noexcept = default;
  
  constexpr T 
    from() const noexcept
  { 
    return start_; 
  }
  
  constexpr T 
    to() const noexcept 
  { 
    return stop_;
  }

  constexpr T 
    step() const noexcept 
  { 
    return step_;
  }

  constexpr bool 
    is_ascending() const noexcept 
  { 
    return stop_-start_ > 0;
  }

  constexpr std::size_t 
    size() const noexcept 
  { 
    return npts_;
  }
  
  auto 
    neighbor_nodes(T x) 
    const noexcept( !RangeCheck )
  {
    return neighbor_nodes_impl(x, do_range_check{});
  }

  /// Given a value \p x on the (tick) axis, compute and return the nearest
  /// tick (on the axis), i.e. either the left or the right one.
  /// The function will not check wether the input \p x point lays between
  /// the leftmost-rightmost range, it doesn't care!
  ///
  /// \return A tuple containing {nearet_tick_index, nearest_tick_value}
  ///
  auto 
    nearest_neighbor(T x) noexcept
  {
    if ( x<= start_ ) return std::make_tuple((std::size_t)0, start_);
    if ( x>= stop_ ) return std::make_tuple(npts_-1, stop_);
    std::size_t idx { static_cast<std::size_t>((x+(step_/(T)2)-start_)/step_) };
    return std::make_tuple(idx, idx*step_+start_);
  }

};

enum class Grid_Dimension : char
{
  OneDim,
  TwoDim
};

/// A skeleton for a generic, two-dimensional grid.
template<typename T, bool RangeCheck, Grid_Dimension Dim>
class GridSkeleton
{};

template<typename T, bool RangeCheck>
class GridSkeleton<T, RangeCheck, Grid_Dimension::OneDim>
:TickAxisImpl<T, RangeCheck>
{
public:
  explicit constexpr
    GridSkeleton(T x1, T x2, T dx) noexcept
    : TickAxisImpl<T, RangeCheck>(x1, x2, dx)
  {};
};

template<typename T, bool RangeCheck>
class GridSkeleton<T, RangeCheck, Grid_Dimension::TwoDim>
{
private:
  TickAxisImpl<T, RangeCheck> xaxis_;
  TickAxisImpl<T, RangeCheck> yaxis_;

public:
  /// Default constructor.
  explicit constexpr
    GridSkeleton(T x1, T x2, T dx, T y1, T y2, T dy) noexcept
  : xaxis_(x1, x2, dx),
    yaxis_(y1, y2, dy)
  {}

  auto
    nearest_neighbor(T x, T y) noexcept
  {
    auto t1 { xaxis_.nearest_neighbor(x) };
    auto t2 { yaxis_.nearest_neighbor(y) };
    return std::tuple_cat(t1, t2);
  }

  ///   [1]    [2]
  /// ...+-----+...  -> y_1
  ///    |     |
  ///    |   p |
  /// ...+-----+...  -> y_0
  ///   [0]    [3]
  ///    |     |
  ///    v     v
  ///   x_0   x_1
  auto
    neighbor_nodes(T x, T y)
    const noexcept( !RangeCheck )
    {
      auto t1 { xaxis_.neighbor_nodes(x) };
      auto t2 { yaxis_.neighbor_nodes(y) };
      // TODO: this is not right! place in right order
      return std::tuple_cat(t1, t2); 
    };

};

/*
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
  //S linear_interpolation(T x)
  //{
  //  auto t { this->neighboring_cells(x) };
  //  return std::get<1>(t) + (std::get<3>(t) -std::get<1>(t)) *
  //  static_cast<S>((x-std::get<0>(t))/(std::get<2>(t)-std::get<0>(t)));
  //}
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
*/

/*
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
*/

} // end namespace

#endif
