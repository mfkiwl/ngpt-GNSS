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

} // end ngpt

#endif
