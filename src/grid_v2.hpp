#ifndef __NGPT_GRID_HPP__V2__
#define __NGPT_GRID_HPP__V2__

#include <tuple>
#include <limits>
#include <cstring>
#include <cassert>
#include <vector>
#ifdef DEBUG
    #include <iostream>
#endif

#define GRID_VERSION 2

/**
 * \file      grid.hpp
 *
 * \version   1.0.0
 *
 * \author    xanthos@mail.ntua.gr <br>
 *            danast@mail.ntua.gr
 *
 * \date      Mon 28 Dec 2015 10:33:53 AM EET 
 *
 * \todo      Provide a factor_to_int_type (template?) parameter. E.g. when
 *            constucting a grid from 0.0 to 90.0 with step = 0.5 this could
 *            be done a grid from 0 to 900 with step = 5 and a factor of 10.
 *            Would that enhance efficiency ??
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

/** \class   tickaxisimpl
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
class tick_axis_impl
{
public:
    
    /// A class to represent a node
    class node {
    public:
        explicit constexpr
        node(std::size_t i = 0, const tick_axis_impl* t = nullptr)
        noexcept
            : index_ (i), axis_ (t) {}

        constexpr node(const node& n) noexcept
            : index_(n.index_), axis_(n.axis_) {}

        constexpr node(node&& n) noexcept
            : index_(std::move(n.index_)), axis_ (std::move(n.axis_)) {}

        constexpr node& operator=(const node& n) noexcept
        {
            if (this != &n) {
                index_ = n.index_;
                axis_  = n.axis_;
            }
            return *this;
        }

        constexpr node& operator=(node&& n) noexcept
        {
            index_ = std::move(n.index_);
            axis_  = std::move(n.axis_);
            return *this;
        }

        /// Move forward (preincrement), i.e. to the next node.
        constexpr node& operator++() noexcept
        {
            ++index_;
            return *this;
        }

        constexpr node next() const noexcept
        { return node(this->index_+1, this->axis_); }

        /// Move forward (postincrement), i.e. to the next node.
        constexpr node operator++(int) noexcept
        {
            node nd (*this);
            this->operator++();
            return nd;
        }

        /// Return the index.
        constexpr std::size_t index() const noexcept { return index_; }

        /// Return the underlying axis
        constexpr const tick_axis_impl* axis() const noexcept { return axis_; }

        /// Return the value of the node
        constexpr T value() const noexcept
        { return axis_->start_ + axis_->step_ * index_; }

#ifdef DEBUG
        void print_address() const { if ( axis_) {std::cout<<axis_;} else {std::cout<<"null";} }
#endif

    private:
        std::size_t           index_;
        const tick_axis_impl* axis_;
    };

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
     *  \param[in] x  The point for which we want the neighboring nodes/ticks.
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
    //constexpr
#endif
    auto
    neighbor_nodes_impl(T x, std::true_type)
    const noexcept( !do_range_check::value )
#if __cplusplus == 201103L
    -> std::tuple<node, node>
#endif
    {
        if ( this->out_of_range(x) ) {
            throw std::out_of_range (
            "tick_axis_impl<>::neighbor_nodes_impl -> out_of_range !!");
        }

        // find tick on the left.
        node left (static_cast<std::size_t>((x-start_)/step_), this);

        // find tick on the right.
        node right ( left.next() );
        if ( right.index() >= npts_ ) {
            throw std::out_of_range (
            "tick_axis_impl<>::neighbor_nodes_impl -> out_of_range !!");
        }

        return std::make_tuple( left, right );
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
    -> std::tuple<node, node>
#endif
    {
        node left (static_cast<std::size_t>((x - start_) / step_));
        node right = left + 1;
        return std::make_tuple(left, right);
    }

public:

    /// Constructor.
#if __cplusplus > 201103L
    constexpr
#endif
    explicit tick_axis_impl(T s,  T e, T d) noexcept
        : start_{s},
          stop_ {e},
          step_ {d},
          // in case (e-s)/d is negative, the cast will result in a huge (positive)
          // number; the assert (inside the con'tor body) will check for this.
          npts_{ d ? static_cast<std::size_t>((e-s)/d)+1 : 0 }
    {
        assert( npts_ < std::numeric_limits<int>::max() );
    }

    /// This is designed to be a base class.
    virtual ~tick_axis_impl() {}

    tick_axis_impl(const tick_axis_impl&) noexcept = default;

    tick_axis_impl&
    operator=(const tick_axis_impl&) noexcept = default;

    tick_axis_impl(tick_axis_impl&&) noexcept = default;

    tick_axis_impl&
    operator=(tick_axis_impl&&) noexcept = default;

    /// Return the left-most tick.
    constexpr T from() const noexcept { return start_; }

    /// Return the right-most tick.
    constexpr T to() const noexcept { return stop_; }

    /// Return the axis step size.
    constexpr T step() const noexcept { return step_; }

    /// Check if the axis is in ascending order.
    constexpr bool is_ascending() const noexcept 
    { return stop_ - start_ > 0; }

    /// Return the number of tick-points.
    constexpr std::size_t size() const noexcept { return npts_; }

    /// Check if the given input value \p x is left from start_ (i.e. if it is
    /// between the given limits on the left side).
    /// \return \p true if \p x in on the right of start_, false otherwise (i.e.
    ///         if false is returned, the input \p x value lays outside the axis
    ///         limits on the left).
    ///
    constexpr bool far_left(T x) const noexcept
    { return this->is_ascending() ? x < start_ : x > start_ ; }

    /// Check if the given input value \p x is right from stop_ (i.e. if it is
    /// between the given limits on the right side).
    /// \return \p true if \p x lays on the left of stop_, false otherwise (i.e.
    ///         if false is returned, the input \p x value lays outside the axis
    ///         limits on the right).
    ///
    constexpr bool far_right(T x) const noexcept
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
    int out_of_range(T x) const noexcept
    {
        if ( this->far_left(x) ) { return -1; }
        if ( this->far_right(x)) { return 1; }
        return 0;
    }

    ///  Return the neighbor nodes of the given input value \c x.
    ///  \throw  Will throw an std::out_of_range only if the instance
    ///          is contructed with a true RangeCheck parameter.
#if __cplusplus > 201103L
    constexpr
#endif
    auto neighbor_nodes(T x) const noexcept( !RangeCheck )
#if __cplusplus == 201103L
    -> std::tuple<node, node>
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
    auto nearest_neighbor(T x) const noexcept
#if __cplusplus == 201103L
    -> node
#endif
    {
        if ( this->far_left(x) ) {
            return node(0, this); 
        } else if ( this->far_right(x) ) {
            return node(npts_-1, this);
        }
        return node(static_cast<std::size_t>((x+(step_/(T)2)-start_)/step_),
                    this);
    }

};

enum class Grid_Dimension : char { OneDim, TwoDim };

/// A skeleton for a generic, two-dimensional grid.
template<typename       T,
         bool           RangeCheck,
         Grid_Dimension Dim>
class grid_skeleton {};

template<typename T, bool RangeCheck>
class grid_skeleton<T,
                    RangeCheck,
                    Grid_Dimension::OneDim>
: public tick_axis_impl<T, RangeCheck>
{
public:
#if __cplusplus > 201103L
    constexpr
#endif
    explicit grid_skeleton(T x1, T x2, T dx) noexcept
        : tick_axis_impl<T, RangeCheck>(x1, x2, dx) {}

    virtual ~grid_skeleton() {};
 
    template<typename S>
    constexpr S linear_interpolation(T x, const std::vector<S>& vec) const
    { return this->linear_interpolation(x, vec.data()); }
    
    template<typename S>
    constexpr S linear_interpolation(T x, const S* data) const
    {
        auto nodes ( this->neighbor_nodes(x) );
        S x0 ( static_cast<S>(std::get<0>(nodes).value()) );
        S x1 ( static_cast<S>(std::get<1>(nodes).value()) );
        S y0 ( data[std::get<0>(nodes).index()] );
        S y1 ( data[std::get<1>(nodes).index()] );
        return y0 + (y1-y0)*(x-x0)/(x1-x0);
    }
};

template<typename T, bool RangeCheck>
class grid_skeleton<T,
                    RangeCheck,
                    Grid_Dimension::TwoDim>
{
private:
    tick_axis_impl<T, RangeCheck> xaxis_;
    tick_axis_impl<T, RangeCheck> yaxis_;

public:

    class node {
    private:
        typename tick_axis_impl<T, RangeCheck>::node xnode_;
        typename tick_axis_impl<T, RangeCheck>::node ynode_;
    public:
        explicit constexpr node(std::size_t xindex, std::size_t yindex,
                                const grid_skeleton* grid = nullptr)
        noexcept
            : xnode_(xindex, grid ? &(grid->xaxis_) : nullptr),
              ynode_(yindex, grid ? &(grid->yaxis_) : nullptr)
        { if (xnode_.axis()==nullptr) std::cout<<"\nMan! WTF??"; }

        explicit constexpr node(typename tick_axis_impl<T, RangeCheck>::node& xnode,
                                typename tick_axis_impl<T, RangeCheck>::node& ynode)
        noexcept
            : xnode_(xnode), ynode_(ynode)
        { if (xnode_.axis()==nullptr) std::cout<<"\nMan! WTF??"; }
        
        constexpr node(typename tick_axis_impl<T, RangeCheck>::node&& xnode,
                       typename tick_axis_impl<T, RangeCheck>::node&& ynode)
        noexcept
            : xnode_(std::move(xnode)), ynode_(std::move(ynode))
        { if (xnode_.axis()==nullptr) std::cout<<"\nMan! WTF??"; }

        constexpr node(const node& n) noexcept
            : xnode_(n.xnode_), ynode_(n.ynode_)
        { if (xnode_.axis()==nullptr) std::cout<<"\nMan! WTF??"; }

        constexpr node(node&& n) noexcept
            : xnode_(std::move(n.xnode_)), ynode_(std::move(n.ynode_))
        { if (xnode_.axis()==nullptr) std::cout<<"\nMan! WTF??"; }

        constexpr node& operator=(const node& n) noexcept
        {
            if (this != &n) {
                xnode_ = n.xnode_;
                ynode_ = n.ynode_;
            }
        if (xnode_.axis()==nullptr) std::cout<<"\nMan! WTF??";
            return *this;
        }

        constexpr node& operator=(node&& n) noexcept
        {
            xnode_ = std::move(n.xnode_);
            ynode_ = std::move(n.ynode_);
        if (xnode_.axis()==nullptr) std::cout<<"\nMan! WTF??";
            return *this;
        }

        constexpr std::size_t data_index() const noexcept
        { return ynode_.index() * xnode_.axis()->size() + xnode_.index(); }
        
        constexpr std::size_t data_index_inv() const noexcept 
        { return (ynode_.axis()->size()-ynode_.index()-1)*
                 xnode_.axis()->size()+xnode_.index(); }

        /// Return the indexes
        constexpr std::tuple<std::size_t, std::size_t> node_index() const noexcept
        { return std::make_tuple( xnode_.index(), ynode_.index() ); }

        /// Return the x-index
        constexpr std::size_t index_x() const noexcept
        { return xnode_.node_index(); }
        
        /// Return the y-index
        constexpr std::size_t index_y() const noexcept
        { return ynode_.node_index(); }
        
        /// Return the values at the index(es).
        constexpr std::tuple<T, T> value() const noexcept
        { return std::make_tuple( xnode_.value(), ynode_.value() ); }
        
        /// Return the x-index value
        constexpr std::size_t x_value() const noexcept
        { return xnode_.value(); }
        
        /// Return the y-index value
        constexpr std::size_t y_value() const noexcept
        { return ynode_.value(); }
    };

    /// Default constructor.
#if __cplusplus > 201103L
    constexpr
#endif
    explicit grid_skeleton(T x1, T x2, T dx, T y1, T y2, T dy)
    noexcept
        : xaxis_(x1, x2, dx),
          yaxis_(y1, y2, dy)
    {}
  
    /// Destructor
    virtual ~grid_skeleton() {};

    /// Number of nodes.
    constexpr
    std::size_t size() const noexcept { return xaxis_.size() * yaxis_.size(); }
    
    /// Number of nodes on the x-axis.
    constexpr
    std::size_t x_axis_pts() const noexcept { return xaxis_.size(); }
    
    /// Number of nodes on the y-axis.
    constexpr
    std::size_t y_axis_pts() const noexcept { return yaxis_.size(); }

    /// Starting node on x-axis.
    constexpr
    T x_axis_from() const noexcept { return xaxis_.from(); }
    
    /// Ending node on x-axis.
    constexpr
    T x_axis_to() const noexcept { return xaxis_.to(); }
    
    /// Step of x-axis.
    constexpr
    T x_axis_step() const noexcept { return xaxis_.step(); }
    
    /// Starting node on y-axis.
    constexpr
    T y_axis_from() const noexcept { return yaxis_.from(); }
    
    /// Ending node on y-axis.
    constexpr
    T y_axis_to() const noexcept { return yaxis_.to(); }
    
    /// Step on y-axis.
    constexpr
    T y_axis_step() const noexcept { return yaxis_.step(); }

    constexpr auto nearest_neighbor(T x, T y) const noexcept -> node
    {
        auto t1 ( xaxis_.nearest_neighbor(x) );
        auto t2 ( yaxis_.nearest_neighbor(y) );
        return node( std::move(t1), std::move(t2) );
    }

    ///   [3]    [2]
    /// ...+-----+...  -> y_1
    ///    |     |
    ///    |   p |
    /// ...+-----+...  -> y_0
    ///   [0]    [1]
    ///    |     |
    ///    v     v
    ///   x_0   x_1
    constexpr auto neighbor_nodes(T x, T y) const noexcept( !RangeCheck )
    -> std::tuple<node, node, node, node>
    {
        auto t1 ( xaxis_.neighbor_nodes(x) );
        auto t2 ( yaxis_.neighbor_nodes(y) );
        return std::make_tuple( node(std::get<0>(t1), std::get<0>(t2)),
                                node(std::get<1>(t1), std::get<0>(t2)),
                                node(std::get<1>(t1), std::get<1>(t2)),
                                node(std::get<0>(t1), std::get<1>(t2))
                              );
    };
    
    ///
    template<typename S>
    constexpr S bilinear_interpolation(S x, S y,
                                    const std::vector<S>& vec)
    { return this->bilinear_interpolation(x, y, vec.data()); }

    template<typename S>
    constexpr S bilinear_interpolation(S x, S y, 
                                    std::tuple<node, node, node, node>& nodes,
                                    const S* vals)
    const noexcept( !RangeCheck )
    {
        S x0 (static_cast<S>(std::get<0>(nodes).x_value()));
        S x1 (static_cast<S>(std::get<1>(nodes).x_value()));
        S y0 (static_cast<S>(std::get<0>(nodes).y_value()));
        S y1 (static_cast<S>(std::get<2>(nodes).y_value()));
        S f00 ( vals[std::get<0>(nodes).data_index()] );
        S f10 ( vals[std::get<1>(nodes).data_index()] );
        S f11 ( vals[std::get<2>(nodes).data_index()] );
        S f01 ( vals[std::get<3>(nodes).data_index()] );
        S denom ( (x1-x0)*(y1-y0) );
        return ((x1-x)*(y1-y)/denom)*f00
              +((x-x0)*(y1-y)/denom)*f10
              +((x1-x)*(y-y0)/denom)*f01
              +((x-x0)*(y-y0)/denom)*f11;                                                                   
    }

    template<typename S>
    constexpr S bilinear_interpolation(S x, S y, const S* vals)
    const noexcept( !RangeCheck )
    {
        auto t1 ( xaxis_.neighbor_nodes(x) );
        auto t2 ( yaxis_.neighbor_nodes(y) );
        S x0 (static_cast<S>(std::get<0>(t1).value()));
        S x1 (static_cast<S>(std::get<1>(t1).value()));
        S y0 (static_cast<S>(std::get<0>(t2).value()));
        S y1 (static_cast<S>(std::get<1>(t2).value()));
        node n00 (std::get<0>(t1), std::get<0>(t2));
        node n10 (std::get<1>(t1), std::get<0>(t2));
        node n11 (std::get<1>(t1), std::get<1>(t2));
        node n01 (std::get<0>(t1), std::get<1>(t2));
        S f00 ( vals[n00.data_index()] );
        S f10 ( vals[n10.data_index()] );
        S f11 ( vals[n11.data_index()] );
        S f01 ( vals[n01.data_index()] );
        S denom ( (x1-x0)*(y1-y0) );
        return ((x1-x)*(y1-y)/denom)*f00
              +((x-x0)*(y1-y)/denom)*f10
              +((x1-x)*(y-y0)/denom)*f01
              +((x-x0)*(y-y0)/denom)*f11;                                                                   
    }

    // This function will convert a tuple of (x, y) indexes, to an index.
    // For example, if we have an instance of grid_skeleton assosicated with
    // a vector<...>, where: 
    // vector[0] -> (0, 0)
    // vector[1] -> (1, 0)
    // ...
    // and we want to find the index of the vector corresponding to the tuple
    // (x, y), then this function will compute it.
    //
    std::size_t node_to_index(const std::tuple<std::size_t,std::size_t>& t)
    const
    {
        auto x = std::get<0>( t );
        auto y = std::get<1>( t );
        return y * xaxis_.size() + x;
    }
    std::size_t node_to_index(const std::size_t x, const std::size_t y)
    const noexcept
    { return y * xaxis_.size() + x; }

    /* This function perfoms the same operation as node_to_index(), only it is
     * used for cases when the y-axis (in the corresponding vector) is filled
     * in reverse order, i.e.
    \verbatim
      A.                   B.

[3]   15--16--17--18--19   0---1---2---3---4
       |   |   |   |   |   |   |   |   |   |
[2]   10--11--12--13--14   5---6---7---8---9
       |   |   |   |   |   |   |   |   |   |
[1]    5---6---7---8---9  10--11--12--13--14
       |   |   |   |   |   |   |   |   |   |
[0]    0---1---2---3---4  15--16--17--18--19

     [0]  [1] [2] [3] [4]
     \endverbatim
     * Incase the vector is storead in the A. form, use node_to_index(), else
     * (i.e. for case B.) use node_to_index_inv().
     */
    std::size_t node_to_index_inv(const std::size_t x, const std::size_t y)
    const noexcept
    { return (yaxis_.size() - y -1) * xaxis_.size() + x; }

};

} // end namespace

#endif
