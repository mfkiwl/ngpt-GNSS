#ifndef _PCV_ANTEX_HPP_
#define _PCV_ANTEX_HPP_

#include <algorithm>
#include "antex.hpp"

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

/// Phase Center Variations can be given either in an azimouth- and zenith-
/// dependent 2-dimensional grid (denoted as 'AZI'), or in an zenith-only
/// dependent 1-dimensional grid (denoted as 'NOAZI'). 
enum class AtxPcvType : char { AZI, NOAZI };

/// Azimouth-dependent pcv, always start at azimouth = 0.0 (see, antex V1.4).
constexpr double PCV_AZIMOUT_START { 0.0e0 };

/// Azimouth-dependent pcv, always start at azimouth = 360.0 (see, antex V1.4).
constexpr double PCV_AZIMOUT_STOP { 360.0e0 };

/// Simple struct to denote a point on a grid. Note that for a one-dimensional
/// grid, y is not used!
template<typename T, typename S>
struct GridPoint
{
  T x,y;
  S val;
};

///
/// \note  
///        - 360 degrees have to be divisible by dazi_. For non-azimuth-
///           dependent phase center variations '0.0' has to be specified.
///        - ZEN1 / ZEN2 / DZEN : For receiver antennas, Zenith distance 
///           'ZEN1' to 'ZEN2' with increment 'DZEN' (in degrees). 'DZEN'
///           has to be > 0.0. 'ZEN1' and 'ZEN2' always have to be multiples of
///           'DZEN'. 'ZEN2' always has to be greater than 'ZEN1'.
///        - ZEN1 / ZEN2 / DZEN : For satellite antennas, Definition of the 
///           grid in nadir angle: Nadir angle 'ZEN1' to 'ZEN2' with increment 
///           'DZEN' (in degrees). 'DZEN' has to be > 0.0. 'ZEN1' and 'ZEN2' 
///           always have to be multiples of 'DZEN'. 'ZEN2' always has to be 
///           greater than 'ZEN1'.
///
template<typename T, typename S>
class PcvGrid
{
private:
  T  dazi_; ///< Increment of the azimuth: 0 to 360 with increment dazi_ (degrees).
  T  zen1_; ///< Starting zenith angle (degrees).
  T  zen2_; ///< End zenith angle (degrees).
  T  dzen_; ///< Increment in zenith angle.
  S *pcvs_; ///< Pointer to start of actual pcv value.
  std::size_t rows_; ///< Number of azimout points (nodes); at least 1.
  std::size_t cols_; ///< Number of zenith points (nodes).

public:

  /// Given a point in a grid, return the neighboring cells. If the grid has
  /// no azimouth-dependent pcv values (i.e. dazi_ = 0), then only the 'zen'
  /// input argument is used; else, both 'zen' and 'azi' are used to find
  /// the neighboring nodes. The returned vector of grid points, follows the
  /// clockwise order:
  ///
  /// x denotes the point x(zen, azi)
  /// 2-D grid  1-D grid
  ///
  ///  1    2
  ///  +----+
  ///  | x  |      
  ///  +----+     +---x---+
  ///  0    3     0       1
  ///
  /// \param[in] zen  The zenith (or nadir) angle to interpolate at.
  /// \param[in] azi  The azimouth angle to interpolate at (only used if
  ///                 the grid is azimouth-dependent).
  /// \return         A vector of GridPoint instances (as ploted above), in
  ///                 clockwise order, bordering the input point. The size
  ///                 of the vector is 2 or a one-dimensional grid, or 4 for
  ///                 a two-dimensional grid. Also note that for a 
  ///                 one-dimensional grid, the y component of the returned
  ///                 GridPoint is unused.
  /// \throw          The function will throw an std::out_of_range if the input
  ///                 point falls outside of the grid.
  ///
  std::vector<GridPoint<T,S>> neighboring_cells(T zen, T azi = .0e0) const
  {
    // find tick on the left.
    T tmp { (zen - zen1_) / dzen_ };
    if (tmp < static_cast<T>(0) || tmp >= cols_) {
      throw std::out_of_range (
      "ERROR. PcvGrid::neighboring_cells() -> out_of_range !!");
    }
    std::size_t left_tick_index { static_cast<std::size_t>(tmp) };

    // zen2 > zen1, so
    std::size_t right_tick_index { left_tick_index + 1 };
    if (right_tick_index >= cols_) {
      throw std::out_of_range (
      "ERROR. PcvGrid::neighboring_cells() -> out_of_range !!");
    }

    // If we are in a one-dimensional grid, that's it !
    if (!dazi_) {
      return std::vector<GridPoint<T,S>> {
        {zen1_+left_tick_index*dzen_, (T)0, pcvs_[left_tick_index]},
        {zen1_+right_tick_index*dzen_, (T)0, pcvs_[right_tick_index]}
      };
    }

    // Do the same for the y-axis (i.e. azimouth axis).
    tmp { (azi - static_cast<T>(PCV_AZIMOUT_STOP)) / dazi_ };
    if (tmp < static_cast<T>(0) || tmp >= rows_) {
      throw std::out_of_range (
      "ERROR. PcvGrid::neighboring_cells() -> out_of_range !!");
    }
    std::size_t left_tick_index_2 { static_cast<std::size_t>(tmp) };

    // azi_stop > azi_start, so
    std::size_t right_tick_index_2 { left_tick_index_2 + 1 };
    if (right_tick_index_2 >= rows_) {
      throw std::out_of_range (
      "ERROR. PcvGrid::neighboring_cells() -> out_of_range !!");
    }

    //
    //  1    2
    //  +----+
    //  |    |
    //  +----+
    //  0    3
    //
    S* ptr { left_tick_index_2*cols_+left_tick_index };
    T x0 { zen1_+left_tick_index*dzen_ };
    T x1 { zen1_+right_tick_index*dzen_ };
    T y0 { static_cast<T>(PCV_AZIMOUT_START)_+left_tick_index_2*dazi_ };
    T y1 { static_cast<T>(PCV_AZIMOUT_START)_+right_tick_index_2*dazi_ };
    return std::vector<GridPoint<T,S>> {
      // Node 0 
      { x0, y0, *ptr },
      // Node 1
      {x0, y1, *(ptr+cols_) },
      // Node 2
      {x1, y1, *(ptr+cols_+1) },
      // Node 3
      {x1, y0, *(ptr+1) }
    };
  }

  /// This function will perform a linear (or bilinear) interpolation, to
  /// compute the value at point (zen,azi), using the sourounding nodes.
  /// In case of a one-dimensional grid (i.e. dazi_=0), only the input
  /// parameter zen is considered, and the interolation is performed using the
  /// nodes left and right of the point. In case of a two-dimensional grid
  /// (i.e. dazi_!=0), the 4 surounding nodes are used to perform a bilinear
  /// interpolation.
  ///
  /// \param[in] zen The zenith (or nadir) distance to interpolate at.
  /// \param[in] azi The azimouth angle to interpolate at; only considered
  ///                in case of an azimouth-dependent grid.
  /// \return        The pcv value at the point zen or (zen,azi).
  ///
  /// \throw         neighboring_cells() may throw in case the point is
  ///                outside grid limits.
  ///
  S linear_interpolation(T zen, T azi = .0e0) const
  {
    std::vector<GridPoint<T,S>> cells { this->neighboring_cells(zen, azi); }
    
    if (!dazi_) {
      return cells[0].val + (cells[1].val - cells[0].val) * 
      ((zen-cells[0].x)/(cells[1].x-cells[0].x));
    }

    // Reference for bilinear 
    // https://en.wikipedia.org/wiki/Bilinear_interpolation
    S x1  { static_cast<S>(cells[0].x) };
    S x2  { static_cast<S>(cells[2].x) };
    S y1  { static_cast<S>(cells[0].y) };
    S y2  { static_cast<S>(cells[1].y) };
    S f11 { cells[0].val };
    S f12 { cells[1].val };
    S f22 { cells[2].val };
    S f21 { cells[3].val };

    S f_xy1 { ((x2-zen)/(x2-x1))*f11 + ((zen-x1)/(x2-x1))*f21 };
    S f_xy2 { ((x2-zen)/(x2-x1))*f12 + ((zen-x1)/(x2-x1))*f22 };
    return ((y2-azi)/(y2-y1))*f_xy1 + ((azi-y1)/(y2-y1))*f_xy2;
  }

  /// Interpolate using the nearest-neighbor algorithm. If the grid has no
  /// azimouth-dependent pcv values (i.e. dazi_ = 0), then only the 'zen'
  /// input argument is used; else, both 'zen' and 'azi' are used to find
  /// the nearest pcv value.
  ///
  /// \param[in] zen The zenith (or nadir) distance to interpolate at.
  /// \param[in] azi The azimouth angle to interpolate at; only considered
  ///                in case of an azimouth-dependent grid.
  /// \return        The pcv value at the point zen or (zen,azi).
  ///
  S nearest_neighbor(T zen, T azi = 0) const noexcept
  {
    std::size_t col { 
      static_cast<std::size_t>((zen + dzen_/2 - zen1_) / dzen_ ) 
    };
    if (col >= cols_)
      col = cols_ - 1;

    std::size_t row;
    if ( dazi_ ) {
      row = static_cast<std::size_t>((azi + dazi_/2 - PCV_AZIMOUT_START) / dazi_ ) 
      if (row >= rows_)
        row = rows_ - 1;
    } else {
      row = 0;
    }

    return pcvs_[row * cols_ + col];
  }
  
  /// Constructor.
  explicit PcvGrid(T z1, T z2, T dz, T da = 0) noexcept
  :dazi_{ da }, zen1_{ z1 }, zen2_{ z2 }, dzen_{ dz }, pcvs_{ nullptr },
  cols_{ static_cast<std::size_t>((z2-z1)/dz)+1 },
  rows_{ da?
    static_cast<std::size_t>((PCV_AZIMOUT_STOP-PCV_AZIMOUT_START)/da)+1:
    1 }
  {
    assert( zen2_ >= zen1_ && dzen_ > .0e0 );
    try {
      pcvs_ = new S [rows_ * cols_];
    } catch (std::bad_alloc&) {
      pcvs_ = nullptr;
    }
    assert( pcvs_ );
  }

  /// Destructor.
  ~PcvGrid() noexcept
  {
    if (pcvs_)
      delete [] pcvs;
  }

  /// Copy constructor.
  PcvGrid(const PcvGrid& rhs) noexcept
  :dazi_{ rhs.dazi_ }, 
  zen1_{ rhs.zen1_ }, 
  zen2_{ rhs.zen2_ }, 
  dzen_{ rhs.dzen_ }, 
  pcvs_{ nullptr },
  cols_{ rhs.cols_ },
  rows_{ rhs.rows_ }
  {
    if (rhs.pcvs_) {
      try {
        pcvs_ = new S [rows_ * cols_];
        std::copy(rhs.pcvs_, rhs.pcvs_+(rows_*cols_), pcvs_);
      } catch (std::bad_alloc&) {
        pcvs_ = nullptr;
      }
      assert( pcvs_ );
    }
  }

  /// Move constructor.
  PcvGrid(PcvGrid&& rhs) noexcept
  :dazi_{ std::move(rhs.dazi_) }, 
  zen1_{ std::move(rhs.zen1_) }, 
  zen2_{ std::move(rhs.zen2_) }, 
  dzen_{ std::move(rhs.dzen_) }, 
  pcvs_{ rhs.pcvs_ },
  cols_{ std::move(rhs.cols_) },
  rows_{ std::move(rhs.rows_) }
  {
    rhs.pcvs_ = nullptr;
  }

  /// Copy assignment operator.
  PcvGrid& operator=(const PcvGrid& rhs) noexcept
  {
    if (this != &rhs) {
      if (rhs.cols_*rhs.rows_ != cols_*rows_) {
        if (pcvs_) {
          delete [] pcvs_;
        }
        try {
          pcvs_ = new S [rhs.cols_*rhs.rows_];
        } catch (std::bad_alloc&) {
          pcvs_ = nullptr;
        }
      }
      dazi_ = rhs.dazi_;
      zen1_ = rhs.zen1_;
      zen2_ = rhs.zen2_;
      dzen_ = rhs.dzen_; 
      cols_ = rhs.cols_;
      rows_ = rhs.rows_;
      assert( pcvs_ );
      std::copy(rhs.pcvs_, rhs.pcvs_+(rows_*cols_), pcvs_);
    }
    return *this;
  }
  
  /// Move assignment operator.
  PcvGrid& operator=(PcvGrid&& rhs) noexcept
  {
    if (this != &rhs) {
      dazi_ = std::move(rhs.dazi_);
      zen1_ = std::move(rhs.zen1_);
      zen2_ = std::move(rhs.zen2_);
      dzen_ = std::move(rhs.dzen_);
      pcvs_ = rhs.pcvs_;
      cols_ = std::move(rhs.cols_);
      rows_ = std::move(rhs.rows_);
      rhs.pcvs_ = nullptr;
    }
    return *this;
  }
}; // end PcvGrid

} // end ngpt

#endif
