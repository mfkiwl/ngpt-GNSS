#ifndef __NGPT_GRID_HPP__
#define __NGPT_GRID_HPP__

#include <tuple>
#include <limits>
#include <cstring>
#include <cassert>

/**
 * \file
 *
 * \version
 *
 * \author    xanthos@mail.ntua.gr <br>
 *            danast@mail.ntua.gr
 *
 * \date      Mon 28 Dec 2015 10:33:53 AM EET 
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

/** \class   TickAxisImpl
 *
 *  \details This class is used to represent a tick axis, starting from tick
 *           with value start_, ending to tick stop_ with a step size of 
 *           step_. Note that stop_ does not have to be greater than start_,
 *           i.e. the axis can be in either ascending or descending order.
 *           The representation of the tick-axis is always from start_ to stop_
 *           like:
 \verbatim
  
      start_ + step_
            ^
            | 
   [index]  |
      [0]  [1]  [2]         npts_-1
   ..--+----+----+--...--+----+--..
       |                      |
       v                      v
     start_                 stop_
   [left]         -->            [right]
  
 \endverbatim
 * 
 *  Template Parameters:
 *           - T                : type of ticks (e.g. float)
 *           - RangeCheck (bool): Enable/Disable range check
 *
 */ 
template<typename T,          ///< type of axis e.g. float, double, ...
         bool     RangeCheck> ///< enable range-check
class TickAxisImpl
{
private:

    T           start_; ///< left-most tick point.
    T           stop_;  ///< right-most tick point.
    T           step_;  ///< step size.
    std::size_t npts_;  ///< number of tick points.

    /// do_range_check wraps the user's choice for RangeCheck.
    using do_range_check = std::integral_constant<bool, RangeCheck>;

    /** Given a point \p x lying on the TickAxis, compute the neighboring ticks
     *  (nodes), i.e. the left and right ticks inbetween of which lays point \p x.
     * 
     *  \parameter[in] x  The point for which we want the neighboring nodes/ticks.
     * 
     *  \return           A tuple containing 
     *  {left_tick_index, left_tick_value, right_tick_index, right_tick_value}
     *  
     *  \note This function will perform range checks to see if the point \p x
     *        does lay between the leftmost and rightmost ticks.
     * 
     \verbatim

       left-most              | x value
         tick                 v
         +-----+-----+-----+--x--+--...--+
                                       right-most
                                         tick
     \endverbatim
     *
     */ 
#if __cplusplus > 201103L
    constexpr
#endif
    auto
    neighbor_nodes_impl(T x, std::true_type)
    const noexcept( !do_range_check::value )
#if __cplusplus == 201103L
    -> std::tuple<std::size_t, T, std::size_t, T>
#endif
    {
        if ( this->out_of_range(x) ) {
            throw std::out_of_range (
            "ERROR. TickAxisImpl<>::neighbor_nodes_impl -> out_of_range !!");
        }

        // find tick on the left.
        std::size_t l_idx { static_cast<std::size_t>((x - start_) / step_) };

        // find tick on the right.
        std::size_t r_idx { l_idx + 1 };
        if (r_idx >= npts_) {
            throw std::out_of_range (
            "ERROR. TickAxisImpl<>::neighbor_nodes_impl -> out_of_range !!");
        }

        return std::make_tuple(
                l_idx, l_idx*step_ + start_, 
                r_idx, r_idx*step_ + start_);
    }

    /// Does exactly the same as neighbor_nodes_impl(T x, std::true_type) but
    /// with no range checks.
#if __cplusplus > 201103L
    constexpr
#endif
    auto
    neighbor_nodes_impl(T x, std::false_type)
    const noexcept( !do_range_check::value )
#if __cplusplus == 201103L
    -> std::tuple<std::size_t, T, std::size_t, T>
#endif
    {
        // find tick on the left.
        std::size_t l_idx { static_cast<std::size_t>((x - start_) / step_) };

        // find tick on the right.
        std::size_t r_idx { l_idx + 1 };

        return std::make_tuple(
                l_idx, l_idx*step_ + start_,
                r_idx, r_idx*step_ + start_);
    }

public:

    /// Constructor.
#if __cplusplus > 201103L
    constexpr
#endif
    explicit TickAxisImpl(T s,  T e, T d) noexcept
        : start_{s},
          stop_{e},
          step_{d},
          npts_{ d ? static_cast<std::size_t>((e-s)/d)+1 : 0 }
    {
        assert( npts_ < std::numeric_limits<int>::max() );
    }

    /// This is designed to be a base class.
    virtual ~TickAxisImpl()
    {}

    TickAxisImpl(const TickAxisImpl&)
    noexcept
    = default;

    TickAxisImpl&
    operator=(const TickAxisImpl&)
    noexcept
    = default;

    TickAxisImpl(TickAxisImpl&&)
    noexcept
    = default;

    TickAxisImpl&
    operator=(TickAxisImpl&&)
    noexcept
    = default;

    /// Return the left-most tick.
    constexpr T
    from()
    const noexcept
    { return start_; }

    /// Return the right-most tick.
    constexpr T 
    to()
    const noexcept 
    { return stop_; }

    /// Return the axis step size.
    constexpr T 
    step()
    const noexcept 
    { return step_; }

    /// Check if the axis is in ascending order.
    constexpr bool 
    is_ascending()
    const noexcept 
    { return stop_ - start_ > 0; }

    /// Return the number of tick-points.
    constexpr std::size_t
    size()
    const noexcept 
    { return npts_; }

    /// Check if the given input value \p x is left from start_ (i.e. if it is
    /// between the given limits on the left side).
    /// \return \p true if \p x in on the right of start_, false otherwise (i.e.
    ///         if false is returned, the input \p x value lays outside the axis
    ///         limits on the left).
    ///
    constexpr
    bool
    far_left(T x)
    const noexcept
    { return this->is_ascending() ? x < start_ : x > start_ ; }

    /// Check if the given input value \p x is right from stop_ (i.e. if it is
    /// between the given limits on the right side).
    /// \return \p true if \p x lays on the left of stop_, false otherwise (i.e.
    ///         if false is returned, the input \p x value lays outside the axis
    ///         limits on the right).
    ///
    constexpr
    bool
    far_right(T x)
    const noexcept
    { return this->is_ascending() ? x > stop_ : x < stop_; }

    /// Check if the given input value \p x lays between the limits of the tick
    /// axis.
    /// \return An integer is returned, which can be:
    ///         Return Value | Explanation
    ///         -------------|-------------------------------------------------
    ///                   -1 | \p x is left from the starting point (outside limits).
    ///                    0 | \p x is inside the axis limits.
    ///                    1 | \p x is right from the ending point (outside limits).
    ///
#if __cplusplus > 201103L
    constexpr
#endif
    int
    out_of_range(T x)
    const noexcept
    {
        if ( this->far_left(x) ) {
            return -1;
        }

        if ( this->far_right(x) ) {
            return 1;
        }

        return 0;
    }

    ///  Return the neighbor nodes of the given input value \c x.
    ///  \throw  Will throw an std::out_of_range only if the instance
    ///          is contructed with a true RangeCheck parameter.
#if __cplusplus > 201103L
    constexpr
#endif
    auto
    neighbor_nodes(T x) 
    const
    noexcept( !RangeCheck )
#if __cplusplus == 201103L
    -> std::tuple<std::size_t, T, std::size_t, T>
#endif
    { return neighbor_nodes_impl(x, do_range_check{}); }

    /// Given a value \p x on the (tick) axis, compute and return the nearest
    /// tick (on the axis), i.e. either the left or the right one.
    /// The function will not check wether the input \p x point lays between
    /// the leftmost-rightmost range, it doesn't care!
    ///
    /// \return A tuple containing {nearet_tick_index, nearest_tick_value}
    ///
#if __cplusplus > 201103L
    constexpr
#endif
    auto
    nearest_neighbor(T x)
    const noexcept
#if __cplusplus == 201103L
    -> std::tuple<std::size_t, T>
#endif
    {
        if ( this->far_left(x) ) {
            return std::make_tuple((std::size_t)0, start_);
        } else if ( this->far_right(x) ) {
            return std::make_tuple(npts_-1, stop_);
        }
        std::size_t idx { static_cast<std::size_t>
                          ((x+(step_/(T)2)-start_)/step_) };
        return std::make_tuple(idx, idx*step_+start_);
    }

};

enum class Grid_Dimension : char
{
    OneDim,
    TwoDim
};

/// A skeleton for a generic, two-dimensional grid.
template<typename       T,
         bool           RangeCheck,
         Grid_Dimension Dim>
    class GridSkeleton
{};

template<typename T, bool RangeCheck>
    class GridSkeleton<T,
                       RangeCheck,
                       Grid_Dimension::OneDim>
    : public TickAxisImpl<T, RangeCheck>
{
public:
#if __cplusplus > 201103L
    constexpr
#endif
    explicit
    GridSkeleton(T x1, T x2, T dx) noexcept
        : TickAxisImpl<T, RangeCheck>(x1, x2, dx)
    {}

    virtual ~GridSkeleton() {};
};

template<typename T, bool RangeCheck>
    class GridSkeleton<T,
                       RangeCheck,
                       Grid_Dimension::TwoDim>
{
private:
    TickAxisImpl<T, RangeCheck> xaxis_;
    TickAxisImpl<T, RangeCheck> yaxis_;

public:
    /// Default constructor.
#if __cplusplus > 201103L
    constexpr
#endif
    explicit
    GridSkeleton(T x1, T x2, T dx, T y1, T y2, T dy)
    noexcept
        : xaxis_(x1, x2, dx),
          yaxis_(y1, y2, dy)
    {}
  
    virtual ~GridSkeleton() {};

    std::size_t
    size()
    const noexcept 
    { return xaxis_.size() * yaxis_.size(); }

    T
    x_axis_from()
    const noexcept
    { return xaxis_.from(); }
    
    T
    x_axis_to()
    const noexcept
    { return xaxis_.to(); }
    
    T
    x_axis_step()
    const noexcept
    { return xaxis_.step(); }
    
    T
    y_axis_from()
    const noexcept
    { return yaxis_.from(); }
    
    T
    y_axis_to()
    const noexcept
    { return yaxis_.to(); }
    
    T
    y_axis_step()
    const noexcept
    { return yaxis_.step(); }

    auto
    nearest_neighbor(T x, T y)
    noexcept 
#if __cplusplus == 201103L
    -> std::tuple<std::size_t, T, std::size_t, T>
#endif
    {
        auto t1 ( xaxis_.nearest_neighbor(x) );
        auto t2 ( yaxis_.nearest_neighbor(y) );
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
#if __cplusplus == 201103L
    -> std::tuple<std::size_t, T, std::size_t, T, 
                  std::size_t, T, std::size_t, T>
#endif
    {
        auto t1 ( xaxis_.neighbor_nodes(x) );
        auto t2 ( yaxis_.neighbor_nodes(y) );
        // TODO: this is not right! place in right order
        return std::tuple_cat(t1, t2);
    };

};

} // end namespace

#endif
