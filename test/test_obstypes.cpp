#include <iostream>
#include "obstype.hpp"

using namespace ngpt;

int main()
{
    /// Let's make an observation the most simple way
    ObservationType o1 (
        SATELLITE_SYSTEM::GPS,
        OBSERVATION_TYPE::PSEUDORANGE, 
        2, 
        'x' );
    
    /// add a type to the above observation
    o1.add_type(SATELLITE_SYSTEM::GPS,OBSERVATION_TYPE::PSEUDORANGE,1,'x');
    
    /// compute the frequency of this observable
    double freq = o1.frequency();
    
    /// The following is an error:
    /// Add an invalid type to the observable
    o1.add_type(SATELLITE_SYSTEM::GPS,OBSERVATION_TYPE::PSEUDORANGE,15,'x');
    /// and get the frequency
    try {
      freq = o1.frequency();
    } catch ( ... ) {
      std::cerr << "\n[ERROR] Invalid observation type!\n";
    }
    
    
    return 0;
}