#ifndef _T_GRID_HPP_
#define _T_GRID_HPP_

#include <assert.h>
#include <stdexcept>
#include <utility>

#ifdef DEBUG
  #include <iostream>
#endif

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

/// \brief  This class represents a tick axis!. Yes, a tick-axis like in
///         any plot. It has a (left) startpoint (i.e. from_), an ending
///         (rightmost) tick (at to_) and a step size (i.e. step_). The
///         axis can be ascending (i.e. to_ > from_ and step_ > 0) or
///         descending (i.e. to_ < from_ and step_ < 0). The template
///         parameter 'T', designates the type of the ticks (e.g. int, float, 
///         etc...).
///
/// \note   The class has a virtual destructor because it is meant to be used
///         as a base class for more elaborate classes (e.g. grids).
///
template<typename T>
class TickAxis {
private:
  T from_;          ///< Leftmost tick value.
  T to_;            ///< Rightmost tick value.
  T step_;          ///< Axis step size.
  std::size_t pts_; ///< Number of ticks.

public:
  /// Default constructor.
  explicit TickAxis(T f, T t, T s) noexcept
  :from_{ f }, to_{ t }, step_{ s }, 
  pts_{ static_cast<std::size_t>((t - f) / s) + 1 }
  {
    assert( step_ != 0 );
    assert( ((to_ - from_) > 0 && step_ > 0) || 
        ((to_ - from_) < 0 && step_ < 0) );
  }

  /// Destructor.
  virtual ~TickAxis() noexcept = default;

  /// Copy Constructor.
  TickAxis(const TickAxis&) noexcept = default;

  /// Move Constructor.
  TickAxis(TickAxis&&) noexcept = default;

  /// Assignment operator.
  TickAxis& operator=(const TickAxis&) noexcept = default;

  /// Move assignment operator.
  TickAxis& operator=(TickAxis&&) noexcept = default;

  /// Return the leftmost point.
  inline T start() const noexcept { return from_; }

  /// Return the rightmost point.
  inline T stop() const noexcept { return to_; }

  /// Return the step size.
  inline T step() const noexcept { return step_; }

  /// Return the number of grid points.
  inline std::size_t num_pts() const noexcept { return pts_; }

  /// Is the grid arranged in ascending order ?
  inline bool is_ascending() const noexcept
  {
    return (to_ - from_) > static_cast<T>(0);
  }
  
  /// Return the tick value at some given index.
  inline T tick_at(std::size_t idx) const noexcept
  {
    assert(idx < pts_);
    return from_ + step_ * idx;
  }

  /// \brief   Given a value 'x', return the tick on the axis nearest to the
  ///          input value.
  ///
  /// \param[in]  x    The input value.
  /// \param[out] indx The index of the axis-tick nearest to the given value.
  ///
  /// \return  The value of the axis-tick, nearest to the input value.
  ///
  inline T nearest_tick(const T x, std::size_t& indx) const noexcept
  {
    indx = static_cast<std::size_t>( (x + step_/2 - from_) / step_ );
    if (indx >= pts_)
      indx = pts_ - 1;
    return indx * step_ + from_;
  }

  /// \brief  Find (both index and value) the node in the axis (grid) for
  ///         imidiatelly left from the given point (i.e. x). 'Left' here,
  ///         denotes left from an axis starting from from_ and ending at to_
  ///         regardless the order of from_ and to_.
  ///         In an ascending grid, this means that the function will return 
  ///         the largest node for which: node_value <= x
  ///         In a descending grid, the function will return the smallest node,
  ///         for which: node_value > x
  ///
  /// \param[in]  x    Input value for which we want to (sort of) interpolate.
  /// \param[out] indx At output, the index of the node on the left of 'x'
  ///                  value (the value of the node is returned). Node with
  ///                  index '0' is from_.
  /// \return          The value of the node on the left of 'x'.
  ///
  /// \warning         The function will throw if the input value 'x' is out of
  ///                  bounds.
  inline T left_tick(const T x, std::size_t& indx) const
  {
    T i { (x - from_) / step_ };
    if (i < 0 || i > pts_) {
      throw std::out_of_range (
      "ERROR. TickAxis<T>::left_tick() -> out_of_range !!");
    }
    indx = static_cast<std::size_t>(i);
    return indx * step_ + from_;
  }

};

template<typename T, typename S>
struct GridPoint {
  T x, y;
  S val;
  void print() { std::cout << "("<<x<<", "<<y<<") = " << val; }
}; 

template<typename T, typename S>
struct OneDimGridImpl : TickAxis<T>
{
  T *ptr_;
  
  explicit OneDimGridImpl(T _start, T _stop, T _step) noexcept
  :TickAxis<T>{_start, _stop, _step}, ptr_{nullptr}
  {};
  
  virtual ~OneDimGridImpl() 
  { 
    ptr_ = nullptr; 
  }

  inline S nearest_neighbor(T x, T& tick_value) noexcept
  {
    assert( ptr_ );
    std::size_t idx;
    tick_value = TickAxis<T>::nearest_tick(x, idx);
    return *(ptr_ + idx);
  }
};

/// A grid can be one- or two- dimensional.
enum class Grid_Dimension : char { One, Two };

/// Generic Grid Class; to be specialized.
template<typename T, 
  typename S, 
  std::size_t N,
  Grid_Dimension D, 
  std::size_t M = 0>
class Gridi {};

/// \brief Grid class specialization: One-dimensional, static-sized grid
template<typename T, typename S, std::size_t N>
class Gridi<T,S,N,Grid_Dimension::One,0>
: OneDimGridImpl<T,S>
{
private:
  S data_[N];
public:
  constexpr explicit Gridi(T f, T t, T s) noexcept
  : OneDimGridImpl<T,S>(f, t, s)
  {
    //assert(num_pts() == N);
  }
  ~Gridi(){};
};

/// \brief Grid class specialization: One-dimensional, static-sized grid
template<typename T, typename S>
class Gridi<T,S,0,Grid_Dimension::One>
: OneDimGridImpl<T,S>
{
private:
  S *data_;
public:
  explicit Gridi(T f, T t, T s) noexcept
  : OneDimGridImpl<T,S>(f, t, s), data_{ nullptr }
  {
    try {
      data_ = new S [OneDimGridImpl<T,S>::num_pts()];
    } catch (std::bad_alloc&) {
      data_ = nullptr;
    }
    assert( data_ );
  }

  ~Gridi()
  {
    if (data_)
      delete [] data_;
  }
};
/*
template<typename T, typename S>
class Grid<T,S,Grid_Dimension::Two>
{
private:
  Axis<T, Grid_Range_Check::Yes> xaxis_;
  Axis<T, Grid_Range_Check::Yes> yaxis_;
  S* data_;
  
  inline void _free() noexcept
  {
    if (data_)
      delete [] data_;
  }

public:
  
  class visual_iterator {
  private:
    S* ptr_;
    std::size_t rows_, cur_row_;
    std::size_t cols_, cur_col_;
  public:
    
    visual_iterator() noexcept 
    : ptr_{ nullptr }, rows_{0}, cols_{0}, cur_row_{0}, cur_col_{0} 
    {};
    
    visual_iterator(Grid& g) noexcept 
    : ptr_{ g.data_ }, 
    rows_{ g.yaxis_.num_pts() }, 
    cur_row_{ 0 },
    cols_{ g.yaxis_.num_pts() },
    cur_col_{ 0 },
    {
      ptr_ += (rows_ - 1 ) * cols_;
      cur_row_ += rows_ - 1;
    };
    
    void advance()
    {
      if (!(cur_row_ + cur_col_)) {
        ptr_ = nullptr;
      }
      
      std::size_t last_index_in_row = (cur_row_ - 1 ) * cols_;
      <last_index_in_row
    }
    
    void begin() noexcept
    {
      ptr_ +=  (rows_ - 1 ) * cols_;
    }
  };
  
  explicit Grid(T xfrom, T xto, T xstep, T yfrom, T yto, T ystep) noexcept
  : xaxis_{ xfrom, xto, xstep }, 
  yaxis_{ yfrom, yto, ystep }, 
  data_{ nullptr }
  {
    try {
      data_ = new S [xaxis_.num_pts() * yaxis_.num_pts()];
    } catch (std::bad_alloc&) {
      data_ = nullptr;
    }
    assert( data_ );
  }
  
  ~Grid() noexcept
  {
    this->_free();
  }
  
  inline S& at_index(std::size_t i)
  {
    return data_[i];
  }
  
  inline S& at_index(std::size_t idx, std::size_t idy)
  {
    return data_[idy*xaxis_.num_pts()+idx];
  }
  
  std::vector<GridPoint<T,S>> cell(T x, T y)
  {
    std::size_t x0_index, y0_index;
    std::size_t x1_index, y1_index;
    std::size_t x2_index, y2_index;
    std::size_t x3_index, y3_index;
    
    T x0 { xaxis_.left_node_impl(x, x0_index) };
    T y0 { yaxis_.left_node_impl(y, y0_index) };
    
    x1_index = x0_index;
    y1_index = y0_index + 1;
    T x1 { xaxis_.grid_value_at(x1_index) };
    T y1 { yaxis_.grid_value_at(y1_index) };
    
    x2_index = x0_index + 1;
    y2_index = y0_index + 1;
    T x2 { xaxis_.grid_value_at(x2_index) };
    T y2 { yaxis_.grid_value_at(y2_index) };
    
    x3_index = x0_index + 1;
    y3_index = y0_index;
    T x3 { xaxis_.grid_value_at(x3_index) };
    T y3 { yaxis_.grid_value_at(y3_index) };
    
    S val0 = this->at_index(x0_index, y0_index);
    S val1 = this->at_index(x1_index, y1_index);
    S val2 = this->at_index(x2_index, y2_index);
    S val3 = this->at_index(x3_index, y3_index);
    
    return std::vector<GridPoint<T,S>> { {x0,y0,val0}, {x1,y1,val1}, {x2,y2,val2}, {x3,y3,val3} };
  }
};

template<typename T, typename S>
class Grid<T,S,Grid_Dimension::One>
{
private:
  Axis<T, Grid_Range_Check::Yes> xaxis_;
  S* data_;

  inline void _free() noexcept
  {
    if (data_)
      delete [] data_;
  }

public:
  explicit Grid(T from, T to, T step) noexcept
  : xaxis_{ from, to, step }, data_{ nullptr }
  {
    try {
      data_ = new S [xaxis_.num_pts()];
    } catch (std::bad_alloc&) {
      data_ = nullptr;
    }
    assert( data_ );
  }

  ~Grid() noexcept
  {
    this->_free();
  }
  
  inline S& at_index(std::size_t idx)
  {
    return data_[idx];
  }
  
  S interpolate(T x) const
  {
    std::size_t left_node_index;
    std::size_t right_node_index;
    T tmp;

    tmp = xaxis_.left_node_impl(x, left_node_index);
    right_node_index = left_node_index + 1;

    if ( !xaxis_.is_ascending ) {
      std::swap(left_node_index, right_node_index);
    }

    S y0 { data_[left_node_index] };
    S y1 { data_[right_node_index] };
    S x0 { static_cast<S>(xaxis_.grid_value_at_impl(left_node_index)) };
    S x1 { static_cast<S>(xaxis_.grid_value_at_impl(right_node_index)) };

    return y0 + (y1-y0) * ( (x - x0) / (x1 - x0) );
  }
  
  inline S nearest_neighbor(T x) const
  {
    std::size_t idx;
    xaxis_.nearest_node_impl(x, idx);
    return data_[idx];
  }
};
*/
} // end ngpt

#endif
