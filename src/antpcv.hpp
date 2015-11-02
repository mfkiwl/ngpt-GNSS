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
  
  ~frequency_pcv() noexcept {};

  friend void swap(frequency_pcv& lhs, frequency_pcv& rhs) noexcept
  {
    using std::swap;
    std::swap(lhs.type_,                rhs.type_);
    std::swap(lhs.eccentricity_vector_, rhs.eccentricity_vector_);
    std::swap(lhs.no_azi_pcv_values_,   rhs.no_azi_pcv_values_);
    std::swap(lhs.azi_pcv_values_,      rhs.azi_pcv_values_);
  }

  frequency_pcv(const frequency_pcv& rhs) noexcept = default;
  /*: type_(rhs.type),
    eccentricity_vector_(rhs.eccentricity_vector_),
    no_azi_pcv_values_(rhs.no_azi_pcv_values_),
    azi_pcv_values_(rhs.azi_pcv_values_)
  {}*/

  frequency_pcv(frequency_pcv&& rhs) noexcept = default;
  /*: type_               { std::move(rhs.type) },
    eccentricity_vector_{ std::move(rhs.eccentricity_vector_) },
    no_azi_pcv_values_  { std::move(no_azi_pcv_values_) },
    azi_pcv_values_     { std::move(azi_pcv_values_) }
  {}*/

  frequency_pcv& operator=(const frequency_pcv rhs) noexcept
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

} // end namespace

#endif
