#include <algorithm>
#include <iterator>
#include "obstype.hpp"

/// Return the identifier of the input observable_type. The identifiers are
/// extracted from \cite rnx303.
///
/// \warning Keep this function updated! All possible ngpt::observable_type
///          should be included in the switch.
///
char
ngpt::obstype_to_char(ngpt::observable_type ot)
{
    switch ( ot ) {
        case ngpt::observable_type::pseudorange             :  return 'C';
        case ngpt::observable_type::carrier_phase           :  return 'L';
        case ngpt::observable_type::doppler                 :  return 'D';
        case ngpt::observable_type::signal_strength         :  return 'S';
        case ngpt::observable_type::ionosphere_phase_delay  :  return 'I';
        case ngpt::observable_type::receiver_channel_number :  return 'X';
        default : throw std::out_of_range
            ("ngpt::obstype_to_char -> Invalid Observable Type!!");
    }
}

/// \brief Add an observation type. If the type already exists, then 
///        just alter the coefficient (to be previous + this one).
///
std::size_t
ngpt::observation_type::add_type(const gnss_obs_details::_rawobs_& t, 
                                 double coef)
noexcept
{
    // Check if it (i.e. gnss_obs_details::_rawobs_) already exists.
    auto it = std::find_if(std::begin(cov_), std::end(cov_),
            [&t](const /*auto C++14*/CoefObsPair& i) ->bool { 
            return i.second == t; 
            });
    
    if (it == std::end(cov_)) {
        cov_.emplace_back(coef, t);
    } else {
        it->first += coef;
    }

    return cov_.size();
}
