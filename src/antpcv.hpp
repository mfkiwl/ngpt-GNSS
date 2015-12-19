#ifndef __ANTPCV_HPP__
#define __ANTPCV_HPP__

#include <array>
#include "grid.hpp"
#include "obstype.hpp"

#ifdef DEBUG
    #include <iostream>
#endif

namespace ngpt
{

/**
 *   Hold phase center variation pattern for a single frequency (i.e. 
 *   ObservationType). This class will hold:
 *   - the antenna phase center offset vector
 *   - 'NO-AZI' (i.e. non-azimouth dependent) phase center variation values
 *   - 'AZI' (i.e. azimouth dependent) phase center variation values
 * 
 *  \note This class has absolutely no clue of the grid these values correspond
 *        to (i.e. starting/ending azimouth, starting/ending zenith angle).
 *        Only vectors of pcv values are stored here, which actualy cannot be
 *        correctly indexed.
 * 
 *  \warning Watch the fuck out! When intializing/constructing an instance of
 *           this class, the (member) vectors have zero-size (despite the fact
 *           that at least some memory is allocated). You need to 
 *           push_back/emplace_back. I was stupid enough to spent a few hours
 *           debuging.
 * 
 *  Template Parameter \c T should be either \c float or \c double depending
 *  on the precission we want for the PCV values.
 */ 
template<typename T>
class frequency_pcv
{

typedef std::vector<T>  fltvec;
typedef std::array<T,3> fltarr;

public:
    explicit frequency_pcv(ngpt::ObservationType type, 
             std::size_t no_azi_hint = 1, std::size_t azi_hint = 1)
    noexcept
        : type_{type},
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

    ~frequency_pcv() noexcept = default;

    frequency_pcv(const frequency_pcv& rhs) noexcept(
        std::is_nothrow_copy_constructible<fltarr>::value
     && std::is_nothrow_copy_constructible<fltvec>::value 
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

    frequency_pcv(frequency_pcv&& rhs) noexcept(
        std::is_nothrow_move_constructible<fltarr>::value
     && std::is_nothrow_move_constructible<fltvec>::value 
        ) = default;
#ifdef DEBUG
    static_assert( std::is_nothrow_move_constructible<fltarr>::value,
      "std::array<float,3> -> throws for copy c'tor!" );
    static_assert( std::is_nothrow_move_constructible<fltvec>::value,
      "std::vector<float> -> throws for copy c'tor!" );
#endif

    frequency_pcv& operator=(const frequency_pcv& rhs) noexcept(
         std::is_nothrow_copy_assignable<fltarr>::value
      && std::is_nothrow_copy_assignable<fltvec>::value 
         ) = default;

    frequency_pcv& operator=(frequency_pcv&& rhs) noexcept(
        std::is_nothrow_move_assignable<fltarr>::value
      && std::is_nothrow_move_assignable<fltvec>::value 
        ) = default;

    T& north() noexcept
    {
        return eccentricity_vector_[0];
    }
    T& east() noexcept
    {
        return eccentricity_vector_[1];
    }
    T& up() noexcept
    {
        return eccentricity_vector_[2];
    }
    T north() const noexcept
    {
        return eccentricity_vector_[0];
    }
    T east() const noexcept
    {
        return eccentricity_vector_[1];
    }
    T up() const noexcept
    {
        return eccentricity_vector_[2];
    }

    ngpt::ObservationType& type()       noexcept { return type_; }
    
    ngpt::ObservationType  type() const noexcept { return type_; }

    fltvec&
    no_azi_vector()
    noexcept
    {
        return no_azi_pcv_values_;
    }
    
    fltvec&
    azi_vector()
    noexcept
    {
        return azi_pcv_values_;
    }

    T& no_azi_vector(std::size_t i)       { return no_azi_pcv_values_[i]; }
    T  no_azi_vector(std::size_t i) const { return no_azi_pcv_values_[i]; }

private:
    ngpt::ObservationType type_;                 ///< Observation type
    fltarr                eccentricity_vector_;  ///< phase center offset (NEU in mm)
    fltvec                no_azi_pcv_values_;    ///< 'NO_AZI' pcv
    fltvec                azi_pcv_values_;       ///< 'AZI' pcv
};

namespace antenna_pcv_details
{
    /// If we have azimout-dependent calibration, the min azimouth is always
    /// 0 degrees.
    constexpr float azi1 {    .0e0 };
    /// If we have azimout-dependent calibration, the max azimouth is always
    /// 360 degrees.
    constexpr float azi2 { 360.0e0 };
}

/**
 *   This class hold all information for a given antenna recorded in an ANTEX
 *   file. Since all frequency pcv patterns correspond to the same grid (i.e.
 *   the zen1, zen2, dzen, azi1, azi2 and dazi) are the same for all recorded
 *   frequencies, the class has a Grid for the 'NO-AZI' pcv's and one for the
 *   azimouth-dependent patterns (if and only if dazi != 0).
 *   Apart from the grid(s), the class also hold a vector of frequency_pcv
 *   instances, one per each frequency recorded in the ANTEX file.
 * 
 *  \warning Watch the fuck out! When intializing/constructing an instance of
 *           this class, the (member) vectors have zero-size (despite the fact
 *           that at least some memory is allocated). You need to 
 *           push_back/emplace_back. I was stupid enough to spent a few hours
 * 
 *  Template Parameter \c T should be either \c float or \c double depending
 *  on the precission we want for the PCV values.
 */ 
//TODO: Copy c'tors are fucked up in here !!
template<typename T>
class antenna_pcv
{

typedef GridSkeleton<T, false, Grid_Dimension::OneDim> dim1_grid;
typedef GridSkeleton<T, false, Grid_Dimension::TwoDim> dim2_grid;
typedef std::vector<frequency_pcv<T>>                  fr_pcv_vec;

private:
    dim1_grid  no_azi_grid_; ///< Non-azimouth dependent grid (skeleton)
    dim2_grid* azi_grid_;    ///< Azimouth dependent grid (skeleton)
    fr_pcv_vec freq_pcv_;    ///< A vector of frequency_pcv

public:

    /// Default constructor
    explicit antenna_pcv()
        : no_azi_grid_(1, 1, 1),
          azi_grid_   (nullptr)
    {}

    /// Constructor. 
    explicit antenna_pcv(T zen1, T zen2, T dzen,
                         int freqs, T dazi = 0)
    /*noexcept*/
        : no_azi_grid_(zen1, zen2, dzen),
          azi_grid_(dazi
                    ? new dim2_grid{zen1, zen2, dzen, antenna_pcv_details::azi1, antenna_pcv_details::azi2, dazi}
                    : nullptr)
    {
        assert( dzen > .0e0 );

        std::size_t no_azi_hint = no_azi_grid_.size();
        std::size_t    azi_hint = ( azi_grid_ ) ? ( azi_grid_->size() ) : ( 0 );
    
        for (int i=0; i<freqs; ++i) {
            freq_pcv_.emplace_back( no_azi_hint, azi_hint );
        }
    }

    /// Copy constructor.
    antenna_pcv(const antenna_pcv& other)
        : no_azi_grid_ {other.no_azi_grid_},
          azi_grid_    {other.azi_grid_
            ? new dim2_grid( *(other.azi_grid_) )
            : nullptr },
          freq_pcv_    {other.freq_pcv_}
    {
#ifdef DEBUG
        std::cout <<"\n[DEBUG] Fucking realy ?? you want to copy an antenna_pcv ?"
                  <<"\n        this will re-intialize " << sizeof(other) <<" bytes";
#endif
    }

    /// Assignment op
    antenna_pcv& operator=(const antenna_pcv& rhs)
    {
#ifdef DEBUG
        std::cout <<"\n[DEBUG] Fucking realy ?? you want to copy an antenna_pcv ?"
                  <<"\n        this will re-intialize " << sizeof(rhs) <<" bytes"
                  <<"\n        and delete "<< sizeof(*this) <<" bytes.";
#endif
        delete azi_grid_;
        azi_grid_     = nullptr;
        azi_grid_     = new dim2_grid( *(rhs.azi_grid_) );
        no_azi_grid_  = rhs.no_azi_grid_;
        freq_pcv_     = rhs.freq_pcv_;
        return *this;
    }

    /// Move constructor.
    antenna_pcv(antenna_pcv&& other)
        : no_azi_grid_{ std::move(other.no_azi_grid_) },
          azi_grid_   { std::move(other.azi_grid_)    },
          freq_pcv_   { std::move(other.freq_pcv_)    }
    {
        other.azi_grid_ = nullptr;
    }

    /// Destructor
    ~antenna_pcv() noexcept
    {
        delete azi_grid_;
    }

    // Return/access a fequency_pcv based on its frequency. The matching
    // (between freq_pcv.obtype and type) must be performed based on Satellite
    // System and frequency number/band.
    frequency_pcv<T>& 
    freq_pcv_pattern( ngpt::ObservationType type )
    {
        assert( type.raw_obs_num() == 1 );

        for (auto& i : freq_pcv_) {
            if (   i.raw_obs(0).satsys() == type.raw_obs(0).satsys() 
                && i.raw_obs(0).band()   == type.raw_obs(0).band() ) {
                return i;
            }
        }
        throw std::runtime_error("antenna_pcv::freq_pcv_pattern -> Invalid frequency");
    }
    
    // Return/access a fequency_pcv based on its index.
    frequency_pcv<T>&
    freq_pcv_pattern( std::size_t i )
    {
        return freq_pcv_[i];
    }

    T     zen1() const noexcept { return no_azi_grid_.from(); }
    T     zen2() const noexcept { return no_azi_grid_.to();   }
    T     dzen() const noexcept { return no_azi_grid_.step(); }
    bool  has_azi_pcv() const noexcept { return azi_grid_ != nullptr; }
    /// \warning Watch yourself bitch! can cause UB if azi_grid_ is invalid.
    T     azi1() const noexcept { return azi_grid_->y_axis_from(); }
    /// \warning Watch yourself bitch! can cause UB if azi_grid_ is invalid.
    T     azi2() const noexcept { return azi_grid_->y_axis_to();   }
    /// \warning Watch yourself bitch! can cause UB if azi_grid_ is invalid.
    T     dazi() const noexcept { return azi_grid_->y_axis_step(); }

    std::size_t no_azi_grid_pts() const noexcept { return no_azi_grid_.size(); }
    std::size_t azi_grid_pts()    const noexcept 
    { 
        return   azi_grid_ 
               ? azi_grid_->size()
               : 0;
    }

};

} // end namespace

#endif
