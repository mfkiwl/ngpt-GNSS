#ifndef _PCV_ANTEX_HPP_
#define _PCV_ANTEX_HPP_

#include <algorithm>
//#include "antex.hpp"
#include "obstype.hpp"

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
class PcvGrid_Noazi
{

typedef float  grid_t;  /// Type of data describing the grid nodes.
typedef double data_t;  /// Type of data describing the grid values.

public:

  /// Empty constructor; use PcvGrid_Noazi::grid_reset(T z1, T z2, T dz)
  /// later to re-initialize.
  explicit PcvGrid_Noazi() noexcept
  :zen1_{ 0 },
  zen2_{ 0 },
  dzen_{ 0 },
  pcvs_{ nullptr },
  npts_{ 0 }
  {};

  /// Constructor.
  explicit PcvGrid_Noazi(grid_t z1, grid_t z2, grid_t dz) noexcept;

  /// Destructor.
  ~PcvGrid_Noazi() noexcept { if (pcvs_) delete [] pcvs_; }

  /// Copy constructor.
  PcvGrid_Noazi(const PcvGrid_Noazi&) noexcept;

  /// Move constructor.
  PcvGrid_Noazi(PcvGrid_Noazi&&) noexcept;

  /// Copy assignment operator.
  PcvGrid_Noazi& operator=(const PcvGrid_Noazi&) noexcept;
  
  /// Move assignment operator.
  PcvGrid_Noazi& operator=(PcvGrid_Noazi&&) noexcept;
  
  /// Re-initialize.
  void grid_reset_size(grid_t z1, grid_t z2, grid_t dz) noexcept;
  
  std::vector<GridPoint<grid_t,data_t>> neighboring_cells(grid_t zen) const;

  data_t linear_interpolation(grid_t zen) const;

  data_t nearest_neighbor(grid_t zen) const noexcept;

private:
  grid_t       zen1_;  ///< Starting zenith angle (degrees).
  grid_t       zen2_;  ///< End zenith angle (degrees).
  grid_t       dzen_;  ///< Increment in zenith angle.
  data_t       *pcvs_; ///< Pointer to start of actual pcv value.
  std::size_t  npts_;  ///< Number of zenith points (nodes).

}; // end PcvGrid_Noazi

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
class PcvGrid
{

private:
typedef float  grid_t;  /// Type of data describing the grid nodes.
typedef double data_t;  /// Type of data describing the grid values.

/// Azimouth-dependent pcv, always start at azimouth = 0.0 (see, antex V1.4).
static constexpr grid_t PCV_AZIMOUTH_START { 0.0e0 };

/// Azimouth-dependent pcv, always start at azimouth = 360.0 (see, antex V1.4).
static constexpr grid_t PCV_AZIMOUTH_STOP { 360.0e0 };

public:

  /// Empty constructor; use PcvGrid::__grid_reset__(T z1, T z2, T dz, T da = 0)
  /// later to re-initialize.
  explicit PcvGrid() noexcept
  :dazi_{ 0 },
  zen1_{ 0 },
  zen2_{ 0 },
  dzen_{ 0 },
  pcvs_{ nullptr },
  cols_{ 0 },
  rows_{ 0 }
  {};

  /// Constructor.
  explicit PcvGrid(grid_t z1, grid_t z2, grid_t dz, grid_t da) noexcept;

  /// Re-initialize.
  void grid_reset_size(grid_t z1, grid_t z2, grid_t dz, grid_t da) noexcept;

  /// Destructor.
  ~PcvGrid() noexcept { if (pcvs_) delete[] pcvs_; }

  /// Copy constructor.
  PcvGrid(const PcvGrid&) noexcept;

  /// Move constructor.
  PcvGrid(PcvGrid&&) noexcept;

  /// Copy assignment operator.
  PcvGrid& operator=(const PcvGrid&) noexcept;
  
  /// Move assignment operator.
  PcvGrid& operator=(PcvGrid&&) noexcept;
  
  std::vector<GridPoint<grid_t,data_t>>
  neighboring_cells(grid_t zen, grid_t azi) const;

  data_t linear_interpolation(grid_t zen, grid_t azi) const;

  data_t nearest_neighbor(grid_t zen, grid_t azi) const noexcept;

private:
  grid_t      dazi_;  ///< Increment of the azimuth: 0 to 360 with increment dazi_ (degrees).
  grid_t      zen1_;  ///< Starting zenith angle (degrees).
  grid_t      zen2_;  ///< End zenith angle (degrees).
  grid_t      dzen_;  ///< Increment in zenith angle.
  data_t      *pcvs_; ///< Pointer to start of actual pcv value.
  std::size_t cols_;  ///< Number of zenith points (nodes).
  std::size_t rows_;  ///< Number of azimout points (nodes); at least 1.

}; // end PcvGrid

class AntennaPattern
{
private:
  std::vector<PcvGrid_Noazi> noazi_pcv_grid_; ///< NOAZI pattern; one for each observation type.
  std::vector<PcvGrid> azi_pcv_grid_;         ///< Azimouth-dependent pattern; one for each observation type.
  char calibration_method_[20];               ///< Calibration method.
  std::vector<ngpt::ObservationType> otypes_; ///< The observations types for wich we have pcv grids.
  std::vector<double> neu_;                   ///< North, east and up pc corrections.

public:
  explicit AntennaPattern(double zen1, double zen2
};

} // end ngpt

#endif
