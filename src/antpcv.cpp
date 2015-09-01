#ifndef __GRID__HPP__
#define __GRID__HPP__

#include <cassert>
#include "antpcv.hpp"

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
/// Constructor.
ngpt::PcvGrid_Noazi::PcvGrid_Noazi(grid_t z1, grid_t z2, grid_t dz) noexcept
:zen1_{ z1 },
zen2_{ z2 },
dzen_{ dz },
pcvs_{ nullptr },
npts_{ static_cast<std::size_t>((z2-z1)/dz)+1 }
{
  assert( zen2_ >= zen1_ && dzen_ > .0e0 );
  try {
    pcvs_ = new data_t[npts_];
  } catch (std::bad_alloc&) {
    pcvs_ = nullptr;
  }
  assert( pcvs_ );
}

/// Re-initialize.
void
ngpt::PcvGrid_Noazi::grid_reset_size(grid_t z1, grid_t z2, grid_t dz)
noexcept
{
  if (pcvs_) {
    delete[] pcvs_;
  }
  zen1_ = z1;
  zen2_ = z2;
  dzen_ = dz; 
  npts_ = static_cast<std::size_t>((z2-z1)/dz)+1; 
  try {
    pcvs_ = new data_t[npts_];
  } catch (std::bad_alloc&) {
    pcvs_ = nullptr;
  }
  assert( pcvs_ );
}

/// Copy constructor.
ngpt::PcvGrid_Noazi::PcvGrid_Noazi(const PcvGrid_Noazi& rhs) noexcept
:zen1_{ rhs.zen1_ }, 
zen2_{ rhs.zen2_ }, 
dzen_{ rhs.dzen_ }, 
pcvs_{ nullptr },
npts_{ rhs.npts_ }
{
  if (rhs.pcvs_) {
    try {
      pcvs_ = new data_t[npts_];
      std::copy(rhs.pcvs_, rhs.pcvs_+npts_, pcvs_);
    } catch (std::bad_alloc&) {
      pcvs_ = nullptr;
    }
    assert( pcvs_ );
  }
}

/// Move constructor.
ngpt::PcvGrid_Noazi::PcvGrid_Noazi(PcvGrid_Noazi&& rhs) noexcept
:zen1_{ std::move(rhs.zen1_) }, 
zen2_{ std::move(rhs.zen2_) }, 
dzen_{ std::move(rhs.dzen_) }, 
pcvs_{ rhs.pcvs_ },
npts_{ std::move(rhs.npts_) }
{
  rhs.pcvs_ = nullptr;
}

/// Copy assignment operator.
ngpt::PcvGrid_Noazi& 
ngpt::PcvGrid_Noazi::operator=(const PcvGrid_Noazi& rhs) noexcept
{
  if (this != &rhs) {
    if (rhs.npts_ != npts_) {
      if (pcvs_) {
        delete[] pcvs_;
      }
      try {
        pcvs_ = new data_t[rhs.npts_];
      } catch (std::bad_alloc&) {
        pcvs_ = nullptr;
      }
    }
    zen1_ = rhs.zen1_;
    zen2_ = rhs.zen2_;
    dzen_ = rhs.dzen_; 
    npts_ = rhs.npts_;
    assert( pcvs_ );
    std::copy(rhs.pcvs_, rhs.pcvs_+npts_, pcvs_);
  }
  return *this;
}

/// Move assignment operator.
ngpt::PcvGrid_Noazi&
ngpt::PcvGrid_Noazi::operator=(PcvGrid_Noazi&& rhs) noexcept
{
  if (this != &rhs) {
    zen1_ = std::move(rhs.zen1_);
    zen2_ = std::move(rhs.zen2_);
    dzen_ = std::move(rhs.dzen_);
    pcvs_ = rhs.pcvs_;
    npts_ = std::move(rhs.npts_);
    rhs.pcvs_ = nullptr;
  }
  return *this;
}

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
std::vector<ngpt::GridPoint<ngpt::PcvGrid_Noazi::grid_t,ngpt::PcvGrid_Noazi::data_t>> 
ngpt::PcvGrid_Noazi::neighboring_cells(grid_t zen) const
{
  // find tick on the left.
  grid_t tmp { (zen - zen1_) / dzen_ };
  if (tmp < 0.0e0 || tmp >= npts_) {
    throw std::out_of_range (
    "ERROR. PcvGrid_Noazi::neighboring_cells() -> out_of_range !!");
  }
  std::size_t left_tick_index { static_cast<std::size_t>(tmp) };

  // zen2 > zen1, so
  std::size_t right_tick_index { left_tick_index + 1 };
  if (right_tick_index >= npts_) {
    throw std::out_of_range (
    "ERROR. PcvGrid_Noazi::neighboring_cells() -> out_of_range !!");
  }

  // If we are in a one-dimensional grid, that's it !
  return std::vector<GridPoint<grid_t,data_t>> {
    {zen1_+left_tick_index*dzen_,  0.0, pcvs_[left_tick_index]},
    {zen1_+right_tick_index*dzen_, 0.0, pcvs_[right_tick_index]}
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
ngpt::PcvGrid_Noazi::data_t 
ngpt::PcvGrid_Noazi::linear_interpolation(grid_t zen) const
{
  std::vector<ngpt::GridPoint<grid_t,data_t>> cells 
  { this->neighboring_cells(zen) };
  
  return cells[0].val + (cells[1].val - cells[0].val) * 
    static_cast<data_t>((zen-cells[0].x)/(cells[1].x-cells[0].x));

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
ngpt::PcvGrid_Noazi::data_t 
ngpt::PcvGrid_Noazi::nearest_neighbor(grid_t zen) const noexcept
{
  std::size_t col { 
    static_cast<std::size_t>((zen + dzen_/2.0e0 - zen1_) / dzen_ ) 
  };

  if (col >= npts_) {
    col = npts_ - 1;
  }

  return pcvs_[col];
}

/// Constructor.
ngpt::PcvGrid::PcvGrid(float z1, float z2, float dz, float da) noexcept
:dazi_{ da },
zen1_{ z1 },
zen2_{ z2 },
dzen_{ dz },
pcvs_{ nullptr },
cols_{ static_cast<std::size_t>((z2-z1)/dz)+1 },
rows_{ da?static_cast<std::size_t>((PCV_AZIMOUTH_STOP-PCV_AZIMOUTH_START)/da)+1:1 }
{
  assert( zen2_ >= zen1_ && dzen_ > .0e0 );
  try {
    pcvs_ = new data_t[rows_ * cols_];
  } catch (std::bad_alloc&) {
    pcvs_ = nullptr;
  }
  assert( pcvs_ );
}

/// Re-initialize.
void
ngpt::PcvGrid::grid_reset_size(float z1, float z2, float dz, float da)
noexcept
{
  if (pcvs_) {
    delete[] pcvs_;
  }
  dazi_ = da;
  zen1_ = z1;
  zen2_ = z2;
  dzen_ = dz; 
  cols_ = static_cast<std::size_t>((z2-z1)/dz)+1; 
  rows_ = da?static_cast<std::size_t>((PCV_AZIMOUTH_STOP-PCV_AZIMOUTH_START)/da)+1:1;
  try {
    pcvs_ = new data_t[rows_ * cols_];
  } catch (std::bad_alloc&) {
    pcvs_ = nullptr;
  }
  assert( pcvs_ );
}

/// Copy constructor.
ngpt::PcvGrid::PcvGrid(const ngpt::PcvGrid& rhs) noexcept
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
      pcvs_ = new data_t[rows_ * cols_];
      std::copy(rhs.pcvs_, rhs.pcvs_+(rows_*cols_), pcvs_);
    } catch (std::bad_alloc&) {
      pcvs_ = nullptr;
    }
    assert( pcvs_ );
  }
}

/// Move constructor.
ngpt::PcvGrid::PcvGrid(ngpt::PcvGrid&& rhs) noexcept
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
ngpt::PcvGrid& ngpt::PcvGrid::operator=(const ngpt::PcvGrid& rhs) noexcept
{
  if (this != &rhs) {
    if (rhs.cols_*rhs.rows_ != cols_*rows_) {
      if (pcvs_) {
        delete[] pcvs_;
      }
      try {
        pcvs_ = new data_t[rhs.cols_*rhs.rows_];
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
ngpt::PcvGrid& ngpt::PcvGrid::operator=(ngpt::PcvGrid&& rhs) noexcept
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
std::vector<ngpt::GridPoint<ngpt::PcvGrid::grid_t,ngpt::PcvGrid::data_t>>
ngpt::PcvGrid::neighboring_cells(grid_t zen, grid_t azi)
const
{
  // find tick on the left.
  grid_t tmp { (zen - zen1_) / dzen_ };
  if (tmp < 0.e0 || tmp >= cols_) {
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

  // Do the same for the y-axis (i.e. azimouth axis).
  tmp = (azi - PCV_AZIMOUTH_STOP) / dazi_;
  if (tmp < 0 || tmp >= rows_) {
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
  data_t *ptr { pcvs_+(left_tick_index_2*cols_+left_tick_index) };
  grid_t x0 { zen1_+left_tick_index*dzen_ };
  grid_t x1 { zen1_+right_tick_index*dzen_ };
  grid_t y0 { PCV_AZIMOUTH_START+left_tick_index_2*dazi_ };
  grid_t y1 { PCV_AZIMOUTH_START+right_tick_index_2*dazi_ };
  return std::vector<GridPoint<grid_t,data_t>> {
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
ngpt::PcvGrid::data_t
ngpt::PcvGrid::linear_interpolation(grid_t zen, grid_t azi) const
{
  std::vector<GridPoint<grid_t,data_t>> cells 
    { this->neighboring_cells(zen, azi) };

  // Reference for bilinear 
  // https://en.wikipedia.org/wiki/Bilinear_interpolation
  data_t x1  { static_cast<data_t>(cells[0].x) };
  data_t x2  { static_cast<data_t>(cells[2].x) };
  data_t y1  { static_cast<data_t>(cells[0].y) };
  data_t y2  { static_cast<data_t>(cells[1].y) };
  data_t f11 { cells[0].val };
  data_t f12 { cells[1].val };
  data_t f22 { cells[2].val };
  data_t f21 { cells[3].val };

  data_t f_xy1 { ((x2-zen)/(x2-x1))*f11 + ((zen-x1)/(x2-x1))*f21 };
  data_t f_xy2 { ((x2-zen)/(x2-x1))*f12 + ((zen-x1)/(x2-x1))*f22 };

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
ngpt::PcvGrid::data_t
ngpt::PcvGrid::nearest_neighbor(grid_t zen, grid_t azi)
const noexcept
{
  std::size_t col { 
    static_cast<std::size_t>((zen + dzen_/2 - zen1_) / dzen_ ) 
  };
  if (col >= cols_) {
    col = cols_ - 1;
  }

  std::size_t row {
    static_cast<std::size_t>((azi + dazi_/2 - PCV_AZIMOUTH_START) / dazi_ )
  };
  if (row >= rows_) {
      row = rows_ - 1;
  }

  return pcvs_[row * cols_ + col];
}
