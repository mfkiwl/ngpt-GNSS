#include <algorithm>
#include <iterator>
#include "obstype.hpp"

using ngpt::gnss_obs_details::_rawobs_;

/*
 *  Return the identifier of the input observable_type. The identifiers are
 *  extracted from \cite rnx303.
 * 
 *  \warning Keep this function updated! All possible ngpt::observable_type
 *           should be included in the switch.
 */ 
char
ngpt::obstype_to_char(ngpt::observable_type ot)
{
    switch ( ot )
    {
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

/*
 *  Return the observable_type given an indentifier. The identifiers are
 *  extracted from \cite rnx303.
 * 
 *  \warning Keep this function updated! All possible ngpt::observable_type
 *           should be included in the switch.
 */ 
ngpt::observable_type
ngpt::char_to_obstype(char c)
{

    switch ( c )
    {
        case 'C' : return ngpt::observable_type::pseudorange;
        case 'L' : return ngpt::observable_type::carrier_phase;
        case 'D' : return ngpt::observable_type::doppler;
        case 'S' : return ngpt::observable_type::signal_strength;
        case 'I' : return ngpt::observable_type::ionosphere_phase_delay;
        case 'X' : return ngpt::observable_type::receiver_channel_number;
        default  : throw std::out_of_range
            ("ngpt::char_to_obstype -> Invalid Observable Type!!");
    }
}

void
_rawobs_::set_from_str(const std::string& str,
                       char* sys)
{
    ngpt::satellite_system ss { ngpt::char_to_satsys(*sys) };
    this->set_from_str(str, &ss);
}

///
void
_rawobs_::set_from_str(const std::string& str,
                       ngpt::satellite_system* sys)
{
    satsys_ = sys ? *sys : ngpt::satellite_system::gps;

    std::string obs_str;

#if __cplusplus > 201103L
    std::copy_if(std::cbegin(str),
                 std::cend(str),
                 std::back_inserter(obs_str),
                 [](char c){ return c != ' '; });
#else
    std::copy_if(str.begin(),
                 str.end(),
                 std::back_inserter(obs_str),
                 [](char c){ return c != ' '; });
#endif

    if ( obs_str.size() < 2 ) {
        throw std::out_of_range
        ("ngpt::_rawobs_::set_from_str -> Invalid string! Cannot cast to _rawobs_");
    }

    // first char should be the observation type
    obstype_    = char_to_obstype( obs_str.at(0) );

    // first char should be the frequency band
    // hint -> 
    // http://stackoverflow.com/questions/5029840/convert-char-to-int-in-c-and-c
    nfrequency_ = obs_str.at(1) - '0';

    if ( obs_str.size() > 2 ) {
        attribute_ = ngpt::obs_attribute( obs_str.at(2) );
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
