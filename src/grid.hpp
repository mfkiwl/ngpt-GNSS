#ifndef _T_GRID_HPP_
#define _T_GRID_HPP_

#include <assert.h>
#include <new>

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

/// An enumeration type to be used as template parameter for the class
/// Grid_Range_Check.
enum class Grid_Range_Check : char { Yes, No };

/// \brief A template class to represent an equidistant, one-dimensional grid.
///        The template parameter <T>, should be substituted with the type
///        of the grid values (i.e. int, float, double, etc...). Note that this
///        class does NOT hold any data, only the grid.
///
template<typename T/*, Grid_Range_Check RC = Grid_Range_Check::No*/>
class Equidistant_Grid {
private:
  T from_;          ///< Leftmost grid value (can be max or min).
  T to_;            ///< Rightmost grid value (can be max or min).
  T step_;          ///< Grid step size.
  std::size_t pts_; ///< Number of grid points.

public:  
  /// Default constructor.
  explicit Equidistant_Grid(T f, T t, T s) noexcept
  :from_{ f }, to_{ t }, step_{ s }, 
  pts_{ static_cast<std::size_t>((t - f) / s) + 1 }
  {
    assert( step_ != 0 );
    assert( ((to_ - from_) > 0 && step_ > 0) || 
        ((to_ - from_) < 0 && step_ < 0) );
  }

  /// Destructor.
  virtual ~Equidistant_Grid() noexcept = default;

  /// Copy Constructor.
  Equidistant_Grid(const Equidistant_Grid&) noexcept = default;

  /// Move Constructor.
  Equidistant_Grid(Equidistant_Grid&&) noexcept = default;

  /// Assignment operator.
  Equidistant_Grid& operator=(const Equidistant_Grid&) noexcept = default;

  /// Move assignment operator.
  Equidistant_Grid& operator=(Equidistant_Grid&&) noexcept = default;

  /// Return the leftmost point.
  inline T start() const noexcept { return from_; }

  /// Return the rightmost point.
  inline T stop() const noexcept { return to_; }

  /// Return the step size.
  inline T step() const noexcept { return step_; }

  /// Return the number of grid points.
  inline std::size_t num_pts() const noexcept { return pts_; }

  /// Return the size, i.e. number of grid points.
  inline std::size_t size() const noexcept
  {
    return pts_;
  }

  /// Is the grid arranged in ascending order ?
  inline bool is_ascending() const noexcept
  {
    return (to_ - from_) > 0;
  }
  
  /// Return the grid value at some given index.
  inline T grid_value_at_impl(std::size_t idx) const noexcept
  {
    return from_ + step_ * idx;
  }

  /// \brief   Given a value 'x', return the point on the grid nearest to the
  ///          input value.
  ///
  /// \param[in]  x    The input value.
  /// \param[out] indx The index of the grid point nearest to the given value
  ///
  /// \return  The value of the point on the grid, nearest to the input value.
  ///
  inline T nearest_grid_value_impl(const T x, std::size_t& indx) 
    const noexcept
  {
    return (indx = static_cast<std::size_t>( (x + (step_/2) - from_) / step_ ))
      * step_;
  }
  
  /// \brief   Given a value 'x', return the point on the grid nearest to the
  ///          input value, but not greater than the (input) value.
  ///
  /// \param[in]  x    The input value.
  /// \param[out] indx The index of the grid point nearest to the given value
  ///                  but not greater than the input value.
  ///
  /// \return  The value of the point on the grid, nearest to the input value
  ///          but not greater than the input value.
  ///
  /// \note    This function will always return the grid point exactly left of
  ///          the input value.
  /// 
  inline T nearest_grid_value_ngt_impl(const T x, std::size_t& indx) 
    const noexcept
  {
    return (indx = static_cast<std::size_t>( (x - from_) / step_ ))
      * step_;
  }

};

template<typename T,
    Grid_Range_Check RC = Grid_Range_Check::No>
  class Axis : public Equidistant_Grid<T>
{};


template<typename T>
class Axis<T, Grid_Range_Check::No> : public Equidistant_Grid<T>
{
public:
  
  /// Default constructor.
  explicit Axis(T f, T t, T s) noexcept
  : Equidistant_Grid<T>{f,t,s}
  {};

  /// Return the grid value at some given index.
  inline T grid_value_at(std::size_t i) const noexcept
  {
    return Equidistant_Grid<T>::grid_value_at_impl(i);
  }
};

template<typename T>
class Axis<T, Grid_Range_Check::Yes> : public Equidistant_Grid<T>
{
public:
  
  /// Default constructor.
  explicit Axis(T f, T t, T s) noexcept
  : Equidistant_Grid<T>{f,t,s}
  {};
  
  /// Return the grid value at some given index.
  inline T grid_value_at(std::size_t i) const noexcept
  {
    assert( i >=0 && i < Equidistant_Grid<T>::num_pts() );
    return Equidistant_Grid<T>::grid_value_at_impl(i);
  }
};

/*
/// Return the grid value at some given index, no range check.
template<typename T>
T 
Equidistant_Grid<T, Grid_Range_Check::No>::grid_value_at(std::size_t i)
{
  return from_ + step_ * i;
} 

/// Return the grid value at some given index, with range check.
template<typename T>
T 
Equidistant_Grid<T, Grid_Range_Check::Yes>::grid_value_at(std::size_t i)
{
  assert( i > 0 && i < pts_ );
  return from_ + step_ * i;
} 
*/
/*
template<typename T, typename S>
class Grid
{
public:

  /// Constructor given the starting, ending and step values for the grids.
  ///
  /// \note In-case the required memory cannot be allocated, the constructor 
  ///       will cause the program to terminate (failed assert).
  ///
  explicit 
  Grid(T xfrom, T xto, T dx, T yfrom = 0, T yto = 0, T dy = 1) 
  noexcept
  : x_from_{ xfrom }, x_to_{ xto }, dx_{ dx },
  y_from_{ yfrom }, y_to_{ yto }, dy_{ dy },
  data_{ nullptr }
  {
    std::size_t sz = static_cast<int>( (x_to_ - x_from_) / dx_ ) + 1;
    sz += static_cast<int>( (y_to_ - y_from_) / dy_ ) + 1;

    assert( this->_allocate(sz)  );
  }

  /// Destructor.
  ~Grid() noexcept
  {
    this->_free();
  }

  ///
  S nearest_value(S x, S y = 0)
  {
    std::size_t row { static_cast<std::size_t>((val + (dx_/2.0) - x_from_) / dx_)
    }
  }

private:

  /// Size of the x-grid.
  inline std::size_t _size_x() const noexcept
  {
    return static_cast<int>( (x_to_ - x_from_) / dx_ ) + 1;
  }

  /// Size of the y-grid.
  inline std::size_t _size_y() const noexcept
  {
    return static_cast<int>( (y_to_ - y_from_) / dy_ ) + 1;
  }

  /// De-allocate memmory.
  inline void _free() noexcept
  {
    if (data_) {
      delete [] data_;
    }
  }

  /// Allocate memmory for the data values.
  /// 
  /// \param[in] sz Number of data points (of type S).
  /// \throw        Does not throw!
  /// \return       True in case of success, else False.
  ///
  inline bool _allocate(std::size_t sz) noexcept
  {
    try {
      data_ = new S [sz];
    } catch (std::bad_alloc&) {
      return false;
    }
    return true;
  }

  /// Given a value, find the largest index of the grid less than the value.
  inline std::size_t _less_than_idx(S val) const
  {
    return static_cast<std::size_t>((val - x_from_) / dx_) 
      + (x_from_ > x_to_);
  }
  
  /// Given a value, find the nearest index of the grid.
  inline std::size_t _nearest_idx(S val) const
  {
    return static_cast<std::size_t>((val + (dx_/2.0) - x_from_) / dx_)
      + (x_from_ > x_to_);
  }

  T x_from_;  ///< Leftmost x grid value (can be max or min).
  T x_to_;    ///< Rightmost x grid value (can be max or min).
  T dx_;      ///< x grid step.
  T y_from_;  ///< Leftmost y grid value (can be max or min).
  T y_to_;    ///< Rightmost x grid value (can be max or min).
  T dy_;      ///< y grid step.
  S *data_;   ///< Ptr to start of data array.

}; // end Grid
*/
} // end ngpt

#endif
