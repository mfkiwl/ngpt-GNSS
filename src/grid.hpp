#ifndef __NGPT_GRID_HPP__V2__
#define __NGPT_GRID_HPP__V2__

#include <tuple>
#include <limits>
#include <cstring>
#include <cassert>
#include <vector>
#include <type_traits>
#ifdef GRID_DEBUG
    #include <string>
    #include <iostream>
#endif

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

namespace grid_details
{
    constexpr std::size_t invalid_index = -1;
}

/** \class   tick_axis
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
template<typename T>          ///< type of axis e.g. float, double, ...
class tick_axis
{
public:
    
    /// A class to represent a node
    class node {
        /// Invalid node (i.e. past-the-end)
        static constexpr std::size_t invalid_index = -1;
    public:
        /// Constructor
        explicit 
        node(std::size_t i, const tick_axis& t)
        noexcept
        : index_ {i}, axis_{t} {};
        /// Copy constructor.
        node(const node& n)
        noexcept
            : index_{n.index_}, axis_{n.axis_} {}
        /// Move constructor.
        node(node&& n)
        noexcept
            : index_{std::move(n.index_)}, axis_ {std::move(n.axis_)} {}
        /// Return the ending node (i.e. one past-the-end).
        static constexpr node end(const tick_axis& t)
        noexcept
        { return node {invalid_index, t}; }
        /// Return the begining node.
        static constexpr node begin(const tick_axis& t)
        noexcept
        { return node{0, t}; }
        /// Check if the node is valid.
        bool is_valid() const noexcept
        { return !(this->index_ == invalid_index); }
        /// Copy assignment.
        node& operator=(const node& n) noexcept
        {
            if (this != &n) {
                index_ = n.index_;
                axis_  = n.axis_;
            }
            return *this;
        }
        /// Move assignement.
        node& operator=(node&& n) noexcept
        {
            index_ = std::move(n.index_);
            axis_  = std::move(n.axis_);
            return *this;
        }
        /// Move to next node. If we hit the end, return the end node.
        node next() const noexcept
        {
            if ( index_+1 < axis_.npts_ ) {
                return node{index_+1, axis_};
            } else {
                return node{invalid_index, axis_};
            }
        }
        /// Move forward (preincrement), i.e. to the next node
        node& operator++() noexcept
        {
            if ( index_+1 < axis_.npts_ ) {
                ++index_;
            } else {
                index_ = invalid_index;
            }
            return *this;
        }
        /// Move forward (postincrement), i.e. to the next node
        node operator++(int) noexcept
        {
            node nd (*this);
            this->operator++();
            return nd;
        }
        /// Equality operator.
        /// \warning Only valid for nodes of the same tick_axis.
        bool operator==(const node& n) const noexcept
        { return this->index_ == n.index_; }
        /// Comparisson operator.
        /// \warning Only valid for nodes of the same tick_axis.
        bool operator!=(const node& n) const noexcept
        { return !(*this == n); }
        /// Return the index.
        std::size_t index() const noexcept
        { return index_; }
        /// Return the underlying axis
        const tick_axis& axis() const noexcept
        { return axis_; }
        /// Return the value of the node
        T value() const noexcept
        { return axis_.start_ + axis_.step_ * index_; }

    private:
        std::size_t      index_;
        const tick_axis& axis_;
    
    }; // node


    /// do_range_check wraps the user's choice for RangeCheck.
    /// using do_range_check = std::integral_constant<bool, RangeCheck>;


public:

    /// Constructor.
    explicit tick_axis(T s,  T e, T d) noexcept
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
    virtual ~tick_axis() {}
    /// Copy constructor.
    tick_axis(const tick_axis&) noexcept = default;
    /// Assignment operator
    tick_axis&
    operator=(const tick_axis&) noexcept = default;
    /// Move constructor.
    tick_axis(tick_axis&&) noexcept = default;
    /// Move assignment.
    tick_axis&
    operator=(tick_axis&&) noexcept = default;
    /// Return the left-most tick.
    T from() const noexcept { return start_; }
    /// Return the right-most tick.
    T to() const noexcept { return stop_; }
    /// Return the axis step size.
    T step() const noexcept { return step_; }
    /// Check if the axis is in ascending order.
    bool is_ascending() const noexcept 
    { return stop_ - start_ > 0; }
    /// Return the number of tick-points.
    std::size_t size() const noexcept { return npts_; }
    /// Return the start (left-most) node
    node begin() const noexcept
    { return node::begin(*this); }
    /// Return the (past-the-end) node
    node end() const noexcept
    { return node::end(*this); }
    /// Check if the given input value x is left from start_ (i.e. if it is
    /// between the given limits on the left side).
    /// \return true if x in on the right of start_, false otherwise (i.e.
    ///         if false is returned, the input x value lays outside the axis
    ///         limits on the left).
    ///
    bool far_left(T x) const noexcept
    { return this->is_ascending() ? x < start_ : x > start_ ; }
    /// Check if the given input value x is right from stop_ (i.e. if it is
    /// between the given limits on the right side).
    /// \return true if x lays on the left of stop_, false otherwise (i.e.
    ///         if false is returned, the input x value lays outside the axis
    ///         limits on the right).
    ///
    bool far_right(T x) const noexcept
    { return this->is_ascending() ? x > stop_ : x < stop_; }
    /// Check if the given input value x lays between the limits of the tick
    /// axis.
    /// \return An integer is returned, which can be:
    ///         Return Value | Explanation
    ///         -------------|-------------------------------------------------
    ///                   -1 | x is left from the starting point (outside limits).
    ///                    0 | x is inside the axis limits.
    ///                    1 | x is right from the ending point (outside limits).
    ///
    int out_of_range(T x) const noexcept
    {
        if ( this->far_left(x) ) { return -1; }
        if ( this->far_right(x)) { return 1; }
        return 0;
    }
    /** Given a point x lying on the tickaxis, compute the neighboring ticks
     *  (nodes), i.e. the left and right ticks inbetween of which lays point x.
     * 
     \verbatim

       left-most              | x value
         tick                 v
         +-----+-----+-----+--x--+--...--+
                                       right-most
                                         tick
     \endverbatim
     *
     *  What happens on and/or outside the borders?
     *  -------------------------------------------
     *  - If the input point x, lays outside the axis (to the left of start_ or
     *  to the right of stop_), then the returned nodes are both invalid.
     *  - If the input point equals the left-most node (i.e. start_), then both
     *  returned nodes are valid, and correspond to points 0 and 1.
     *  - If the input point equals the right-most node (i.e. stop_), then
     *  the first (of the two) returned nodes is valid and corresponds to the
     *  last element in the axis (i.e. npts_-1); the second node is invalid.
     */
    std::tuple<node, node>
    neighbor_nodes(T x) const noexcept
    {
        if ( this->out_of_range(x) ) {
            return std::make_tuple( node::end(*this), node::end(*this) );
        }
        // find tick on the left.
        node left {static_cast<std::size_t>((x-start_)/step_), *this};
        // find tick on the right.
        node right {left.next()};
        return std::make_tuple( left, right );
    }
    /// Return the index of the node to the left of the input argument. If x
    /// falls outside the interval [start, stop] then invalid_index is
    /// returned.
    std::size_t left_node_idx(T x) const noexcept
    {
        if ( !this->out_of_range(x) ) {
            return static_cast<std::size_t>((x-start_)/step_);
        } else {
            return grid_details::invalid_index;
        }
    }
    /// Given a value x on the (tick) axis, compute and return the nearest
    /// node (on the axis), i.e. either the left or the right one. In case x
    /// is outside limits, either the left-most or the right-most nodes are
    /// returned.
    node nearest_neighbor(T x) const noexcept
    {
        if ( far_left(x) ) {
            return node{0, *this}; 
        } else if ( far_right(x) ) {
            return node{npts_-1, *this};
        }
        return node{static_cast<std::size_t>((x+(step_/(T)2)-start_)/step_),
                    *this};
    }
    std::size_t nearest_neighbor_idx(T x) const noexcept
    {
        if (far_left(x)) return 0;
        if (far_right(x)) return npts_-1;
        return static_cast<std::size_t>( (x+(step_/(T)2)-start_)/step_ );
    }
    /// Perform linear interpolation to compute the value at point x. The
    /// data parameter should hold the (y) values corresponding to the nodes
    /// of the axis (i.e. data[0] is the value at the first node and data[npts_]
    /// is the value at node npts_-1).
    template<class S, bool bound>
    S interpolate(T x, const S* data) const noexcept( !bound )
    {
        using bound_type = std::integral_constant<bool, bound>;
        return linear_interpolation_impl_raw(x, data, bound_type{});
    }

private:

    T           start_; ///< left-most tick point.
    T           stop_;  ///< right-most tick point.
    T           step_;  ///< step size.
    std::size_t npts_;  ///< number of tick points.
    
    /// Perform linear interpolation to compute the value at point x. The
    /// data parameter should hold the (y) values corresponding to the nodes
    /// of the axis (i.e. data[0] is the value at the first node and data[npts_]
    /// is the value at node npts_-1). If the value of x is not in the interval
    /// [start_, stop_] an std::domain_error exception is thrown.
    template<typename S>
    S /*bounded_*/linear_interpolation_impl_raw(T x, const S* d, std::true_type)
    const
    {
        int status = this->out_of_range(x);
        if ( !status ) {
            std::size_t left {static_cast<std::size_t>((x-start_)/step_)};
            if (left <= npts_ - 2) {
                S x0 = start_ + (S)left * step_;
                S x1 = x0 + step_;
                S y0 = d[left];
                S y1 = d[left+1];
                return y0 + (y1-y0)*((x-x0)/(x1-x0));
            } else if (left == npts_ - 1) {
                return d[npts_-1];
            }
        }
        throw std::domain_error
            ("tick_axis_imp::bounded_linear_interpolation() -> argument out of range!");
    }
    /// Perform linear interpolation to compute the value at point x. The
    /// data parameter should hold the (y) values corresponding to the nodes
    /// of the axis (i.e. data[0] is the value at the first node and data[npts_-1]
    /// is the value at node npts_-1). If the value of x is not in the interval
    /// [start_, stop_] then the closest of the two limiting values is returned.
    template<typename S>
    S /*unbounded_*/linear_interpolation_impl_raw(T x, const S* d, std::false_type)
    const noexcept
    {
        if ( far_left(x) ) {
            return *d;
        }
        // if x < start this will be veeeeeery big
        std::size_t left {static_cast<std::size_t>((x-start_)/step_)};
        if (left <= npts_ - 2) {
            S x0 = start_ + (S)left * step_;
            S x1 = x0 + step_;
            S y0 = d[left];
            S y1 = d[left+1];
            return y0 + (y1-y0)*((x-x0)/(x1-x0));
        }   
        return d[npts_-1];
    }

}; // tick_axis<T>

enum class grid_dimension : char
{ one_dim, two_dim };

/// A class to repersent a grid skeleton, i.e. the axis of a grid.
template<typename       T,
         grid_dimension D>
    class grid_skeleton {};

/// Specialization: A class to hold one-dimensional grids. In fact, this is
/// just a wrapper for the tick_axis class.
template<typename T>
    class grid_skeleton<T, grid_dimension::one_dim>
: public tick_axis<T>
{
public:
    using typename tick_axis<T>::node;

    explicit grid_skeleton(T f, T t, T s) noexcept
        : tick_axis<T>(f, t, s)
    {}

}; // grid_skeleton<T,grid_dimension::one_dim>

/// Specialization: A class to hold two-dimensional grids. The axis of this
/// grid_skeleton are (by convension) called xaxis and yaxis.
template<typename T>
    class grid_skeleton<T, grid_dimension::two_dim>
{
public:
    /// A structure to denote the way that values are stored corresponding
    /// to the grid_skeleton nodes. See the index2index function for more info.
    struct normal_store    { using t = std::true_type;  };
    struct reverse_y_store { using t = std::false_type; };
    /// A class to represent a node in a two-dimensional grids.
    class node {
        /// An invalid, past-the end tick point
        static constexpr std::size_t invalid_index = -1;
    public:
        /// Constructor.
        explicit node(std::size_t x, std::size_t y, const grid_skeleton& g)
        noexcept
            : xindex_{x},
              yindex_{y},
              grid_  {g}
        {}
        /// Return a past-the-end node
        static constexpr node end(const grid_skeleton& g) noexcept
        { return node{invalid_index, invalid_index, g}; }
        /// Return the first node.
        static constexpr node begin(const grid_skeleton& g) noexcept
        { return node{0, 0, g}; }
        /// (Pre-)increment
        node& operator++() noexcept
        {
            if ( yindex_ == invalid_index || xindex_ == invalid_index ) {
                return *this;
            }

            if ( yindex_+1 < grid_.yaxis_.size() ) {
                ++yindex_;
            } else {
                yindex_ = 0;
                if ( xindex_+1 < grid_.xaxis_.size() ) {
                    ++xindex_;
                } else {
                    xindex_ = yindex_ = invalid_index;
                }
            }
            return *this;
        }
        /// Postincrement
        node operator++(int) noexcept
        {
            node n (*this);
            ++(*this);
            return n;
        }
        /// Equality operator.
        /// \warning Only works for nodes of the same grid.
        bool operator==(const node& n) const noexcept
        { return xindex_ == n.xindex_ && yindex_ == n.yindex_; }
        /// InEquality operator.
        /// \warning Only works for nodes of the same grid.
        bool operator!=(const node& n) const noexcept
        { return !(*this==n); }
        /// TODO this is only for debuging
        std::tuple<std::size_t,std::size_t>
        position() const noexcept
        { return std::make_tuple(xindex_,yindex_); }
    private:
        std::size_t xindex_, yindex_;
        const grid_skeleton<T, grid_dimension::two_dim>& grid_;
    }; // node

public:
    /// Constructor
    explicit grid_skeleton(T f1, T t1, T s1, T f2, T t2, T s2) noexcept
        : xaxis_{f1, t1, s1},
          yaxis_{f2, t2, s2}
    {}
    /// Get the first node.
    node begin() const noexcept
    { return node::begin(*this); }
    /// Get one-past-the-end node.
    node end() const noexcept
    { return node::end(*this); }
    /// Start of x-axis
    T x_axis_from() const noexcept
    { return xaxis_.from(); }
    /// Start of y axis
    T y_axis_from() const noexcept
    { return yaxis_.from(); }
    /// Stop of x axis
    T x_axis_to() const noexcept
    { return xaxis_.to(); }
    /// Stop of y axis
    T y_axis_to() const noexcept
    { return yaxis_.to(); }
    /// Step of x axis
    T x_axis_step() const noexcept
    { return xaxis_.step(); }
    /// Step of y axis
    T y_axis_step() const noexcept
    { return yaxis_.step(); }
    /// Return the size (nodes on x axis * nodes on y axis)
    std::size_t size() const noexcept
    { return xaxis_.size() * yaxis_.size(); }
    /// Transform a pair of x-axis/y-axis indexes to the corresponding index
    /// of a corresponding contiguous array. The template parameter D can be
    /// either grid_skeleton::normal_store or grid_skeleton::reverse_y_store.
    /// See the corresponding implementation functions for more details.
    template<typename D = normal_store>
    std::size_t index2index(std::size_t xi, std::size_t yi) const noexcept
    {
        using t = typename D::t;
        return resolve_data_index_impl(xi, yi, t{});
    }
    /// Return the nearest node given a point x
    node nearest_neighbor(T x, T y) const  noexcept
    {
        return node{xaxis_.nearest_neighbor_idx(x),
                    yaxis_.nearest_neighbor_idx(y),
                    *this};
    }
    /// Return the nearest node given a point x. What is actually returned, is
    /// is the index of the nearest neighbot node in a cintiguous memmory array
    /// holding the grid.
    node nearest_neighbor_idx(T x, T y) const  noexcept
    {
        return index2index(xaxis_.nearest_neighbor_idx(x),
                           yaxis_.nearest_neighbor_idx(y));
    }
    /// Interpolate on x-axis, given **NOT** a x value, but the index of the
    /// node at the left of some x value. This x value should be check for
    /// laying outside the grid's range. User also needs to supply the y-index
    /// (like the x-index) so that the function can locate the correct index
    /// in the data array.
    template<typename S>
        S linear_interpolation_on_x(T x, std::size_t xi, std::size_t yi, const S* d)
        const noexcept
    {
#ifdef GRID_DEBUG
        std::cout<<"\n\tInterpolating on x-axis";
#endif
        std::size_t x0i = resolve_data_index_impl(xi, yi, std::true_type{});
        if ( xi+1 > xaxis_.size()-1 ) {
            // std::cout<<"\n\tHit the limit!";
            return d[x0i];
        }
        std::size_t x1i = x0i + 1;
        S x0 = (S)xaxis_.from() + (S)xaxis_.step()*xi;
        S x1 = x0 + (S)xaxis_.step();
        S f0 = d[x0i];
        S f1 = d[x1i];
#ifdef GRID_DEBUG
        std::cout<<"\n\t "<<x0<<"--"<<x<<"--"<<x1;
#endif
        return f0 + (f1-f0)*(x-x0)/(x1-x0);
    }
    template<typename S>
        S linear_interpolation_on_y(T y, std::size_t xi, std::size_t yi, const S* d)
        const noexcept
    {
#ifdef GRID_DEBUG
        std::cout<<"\n\tInterpolating on y-axis";
#endif
        std::size_t y0i = resolve_data_index_impl(xi, yi, std::true_type{});
        if ( yi+1 > yaxis_.size()-1 ) {
            // std::cout<<"\n\tHit the limit!";
            return d[y0i];
        }
        std::size_t y1i = y0i + xaxis_.size();
        S y0 = (S)yaxis_.from() + (S)yaxis_.step()*yi;
        S y1 = y0 + (S)yaxis_.step();
        S f0 = d[y0i];
        S f1 = d[y1i];
#ifdef GRID_DEBUG
        std::cout<<"\n\t "<<y0<<"--"<<y<<"--"<<y1;
#endif
        return f0 + (f1-f0)*(y-y0)/(y1-y0);
    }
    /// Perform bilinear interpolation to compute the value at point P(x,y)
    template<typename S>
        S bilinear_interpolation(T x, T y, const S* d) const
    {
        if ( xaxis_.out_of_range(x) || yaxis_.out_of_range(y) ) {
            std::string xstr = std::to_string(x);
            std::string ystr = std::to_string(y);
            throw std::domain_error
                ("grid_skeleton::bilinear_interpolation -> out of range ("
                 +xstr+", "+ystr+") !");
        }
#ifdef GRID_DEBUG
        std::cout<<"\nInterpolating at ("<<x<<", "<<y<<")";
#endif
        // left node x-index
        std::size_t x0i = xaxis_.left_node_idx(x);
        // left y-index
        std::size_t y0i = yaxis_.left_node_idx(y);
        // right x-index; if this falls out of range, then try to
        // interpolate on the y axis.
        std::size_t x1i = x0i + 1;
        if ( x1i >= xaxis_.size() ) {
            return linear_interpolation_on_y(y, x0i, y0i, d);
        }
        // right y-index; if this falls out of range, then try to
        // interpolate on the x axis.
        std::size_t y1i = y0i + 1;
        if ( y1i >= yaxis_.size() ) {
            return linear_interpolation_on_x(x, x0i, y0i, d);
        }
        // find the corresponding indexes on the data array
        std::size_t bli = resolve_data_index_impl(x0i, y0i, std::true_type{});
        std::size_t bri = bli + 1;
        std::size_t tli = bli + xaxis_.size();
        std::size_t tri = tli + 1;
        // perform the interpolation
        S x0  = (S)xaxis_.from() + x0i*(S)xaxis_.step();
        S x1  = x0 + (S)xaxis_.step();
        S y0  = (S)yaxis_.from() + y0i*(S)yaxis_.step(); 
        S y1  = y0 + (S)yaxis_.step();
        S f00 = d[bli];
        S f01 = d[bri];
        S f10 = d[tli];
        S f11 = d[tri];
#ifdef GRID_DEBUG
        std::cout<<"\n\t ("<<x0<<","<<y1<<")        ("<<x1<<","<<y1<<")\n";
        std::cout<<"\n\t ("<<x0<<","<<y0<<")        ("<<x1<<","<<y0<<")";
#endif
        S denom ( (x1-x0)*(y1-y0) );
        return ((x1-x)*(y1-y)/denom)*f00
              +((x-x0)*(y1-y)/denom)*f10
              +((x1-x)*(y-y0)/denom)*f01
              +((x-x0)*(y-y0)/denom)*f11; 
    }
private:
    tick_axis<T> xaxis_;
    tick_axis<T> yaxis_;
    
    /** Transform a pair of x-axis, y-axis indexes/nodes to the corresponding
     * data index. For example, if we pass the indexes (2,3), this function
     * will return 17 if the template parameter is data_order::normal or 2 if
     * the template parameter is data_order::y_inverted. The given indexes are
     * **NOT** checked for laying within the bounds of the x/y axis.
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
    */
    std::size_t
    resolve_data_index_impl(std::size_t xidx, std::size_t yidx, std::true_type)
    const noexcept
    { return yidx * xaxis_.size() + xidx; }
    /// This is the same as resolve_data_index_impl() but for case B.
    std::size_t
    resolve_data_index_impl(std::size_t xidx, std::size_t yidx, std::false_type)
    const noexcept
    { return (yaxis_.size() - yidx -1) * xaxis_.size() + xidx; }

}; // grid_skeleton<T,grid_dimension::two_dim>

} // ngpt
#endif
