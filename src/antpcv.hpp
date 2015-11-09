#ifndef __ANTPCV_HPP__
#define __ANTPCV_HPP__

#include <array>
#include "grid.hpp"
#include "obstype.hpp"

namespace ngpt
{

class frequency_pcv
{
public:
  explicit frequency_pcv(ngpt::ObservationType type, 
      std::size_t no_azi_hint = 1, std::size_t azi_hint = 1) noexcept
  : type_(type),
    eccentricity_vector_{.0e0, .0e0, .0e0}
    {
      no_azi_pcv_values_.reserve(no_azi_hint);
      azi_pcv_values_.reserve(azi_hint);
    }

    explicit 
      frequency_pcv(std::size_t no_azi_hint = 1, std::size_t azi_hint = 1) 
      noexcept
    : type_(),
      eccentricity_vector_{.0e0, .0e0, .0e0}
    {
      no_azi_pcv_values_.reserve(no_azi_hint);
      azi_pcv_values_.reserve(azi_hint);
    }

  ~frequency_pcv() noexcept {};

  friend void swap(frequency_pcv& lhs, frequency_pcv& rhs) noexcept
  {
    using std::swap;
    std::swap(lhs.type_,                rhs.type_);
    std::swap(lhs.eccentricity_vector_, rhs.eccentricity_vector_);
    std::swap(lhs.no_azi_pcv_values_,   rhs.no_azi_pcv_values_);
    std::swap(lhs.azi_pcv_values_,      rhs.azi_pcv_values_);
  }

  frequency_pcv(const frequency_pcv& rhs) noexcept(
         std::is_nothrow_copy_constructible<std::array<float,3>>::value
      && std::is_nothrow_copy_constructible<std::vector<float>>::value 
      ) = default;
#ifdef DEBUG
//  TODO:: Test#2 fails, i.e. std::vector<float> is not noexcept for the copy
//         constructor.
//
//  static_assert( std::is_nothrow_copy_constructible<std::array<float,3>>::value,
//      "std::array<float,3> -> throws for copy c'tor!" );
//  static_assert( std::is_nothrow_copy_constructible<std::vector<float>>::value,
//      "std::vector<float> -> throws for copy c'tor!" );
#endif
  /*: type_(rhs.type),
    eccentricity_vector_(rhs.eccentricity_vector_),
    no_azi_pcv_values_(rhs.no_azi_pcv_values_),
    azi_pcv_values_(rhs.azi_pcv_values_)
  {}*/

  frequency_pcv(frequency_pcv&& rhs) noexcept(
         std::is_nothrow_move_constructible<std::array<float,3>>::value
      && std::is_nothrow_move_constructible<std::vector<float>>::value 
      ) = default;
#ifdef DEBUG
  static_assert( std::is_nothrow_move_constructible<std::array<float,3>>::value,
      "std::array<float,3> -> throws for copy c'tor!" );
  static_assert( std::is_nothrow_move_constructible<std::vector<float>>::value,
      "std::vector<float> -> throws for copy c'tor!" );
#endif
  /*: type_               { std::move(rhs.type) },
    eccentricity_vector_{ std::move(rhs.eccentricity_vector_) },
    no_azi_pcv_values_  { std::move(no_azi_pcv_values_) },
    azi_pcv_values_     { std::move(azi_pcv_values_) }
  {}*/

  frequency_pcv& operator=(frequency_pcv rhs) noexcept
  {
    swap(*this, rhs);
    return *this;
  }

  frequency_pcv& operator=(frequency_pcv&& rhs) noexcept = default;

private:
  ngpt::ObservationType    type_;
  std::array<float,3>      eccentricity_vector_;
  std::vector<float>       no_azi_pcv_values_;
  std::vector<float>       azi_pcv_values_;
};

namespace antenna_pcv_details
{
  constexpr float azi1 {    .0e0 };
  constexpr float azi2 { 360.0e0 };
}

class antenna_pcv
{
public:
  
  explicit antenna_pcv(float zen1, float zen2, float dzen, 
      int freqs, 
      float dazi = 0)
  /*noexcept*/
  : no_azi_grid_(zen1, zen2, dzen),
    azi_grid_(nullptr)
  {
    assert( dzen > .0e0 );
    if ( dazi ) {
      azi_grid_ = new GridSkeleton<float, false, Grid_Dimension::TwoDim>
        (zen1, zen2, dzen, antenna_pcv_details::azi1, antenna_pcv_details::azi2, dazi);
    }
    std::size_t no_azi_hint = no_azi_grid_.size();
    std::size_t    azi_hint = ( azi_grid_ ) ? ( azi_grid_->size() ) : ( 0 );
    //freq_pcv.reserve(freqs);
    for (int i=0; i<freqs; ++i) {
      freq_pcv.emplace_back(no_azi_hint, azi_hint);
    }
  }

  friend void swap(antenna_pcv& p1, antenna_pcv& p2) noexcept
  {
    using std::swap;
    swap(p1.no_azi_grid_, p2.no_azi_grid_);
    swap(p1.azi_grid_,   p2.azi_grid_);
    swap(p1.freq_pcv,     p2.freq_pcv);
  }

  antenna_pcv(const antenna_pcv& other)
    : no_azi_grid_(other.no_azi_grid_),
      azi_grid_( other.azi_grid_
          ? new GridSkeleton<float, false, Grid_Dimension::TwoDim>
            (other.zen1(), other.zen2(), other.dzen(), 
            antenna_pcv_details::azi1, antenna_pcv_details::azi2, other.dazi())
          : nullptr ),
      freq_pcv(other.freq_pcv)
  {
  }

  antenna_pcv(antenna_pcv&& other)
    : no_azi_grid_(other.no_azi_grid_),
      azi_grid_(other.azi_grid_),
      freq_pcv(std::move(other.freq_pcv))
  {
    other.azi_grid_ = nullptr;
  }

  ~antenna_pcv() noexcept
  {
    delete azi_grid_;
  }

  float zen1() const noexcept { return no_azi_grid_.from(); }
  float zen2() const noexcept { return no_azi_grid_.to(); }
  float dzen() const noexcept { return no_azi_grid_.step(); }
  bool  has_azi_pcv() const noexcept { return azi_grid_ != nullptr; }
  float azi1() const noexcept { return azi_grid_->y_axis_from(); }
  float azi2() const noexcept { return azi_grid_->y_axis_to(); }
  float dazi() const noexcept { return azi_grid_->y_axis_step(); }

private:
  GridSkeleton<float, false, Grid_Dimension::OneDim>  no_azi_grid_;
  GridSkeleton<float, false, Grid_Dimension::TwoDim>* azi_grid_;
  std::vector<frequency_pcv> freq_pcv;
};

} // end namespace

#endif
