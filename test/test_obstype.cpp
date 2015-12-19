#include <iostream>
#include "satsys.hpp"
#include "obstype.hpp"

using namespace ngpt;

int main()
{
    // A simple, elementary observable: GPS L1C
    observation_type gps_l1c (satellite_system::gps, 
                              observable_type::carrier_phase, 1, 'C' );

    std::cout << "\nThe GPS L1C observation type: " << gps_l1c;

    // Let's make the GPS Ionosphere free LC
    // this is: 
    //
    //        f(1)**2 L(1) - f(2)**2 L(2)
    //  Lc =  ---------------------------
    //               f(1)**2 - f(2)**2
    //
    
    // to get the (nominal) frequency for a band we can do :
    double f1_freq = satellite_system_traits<satellite_system::gps>::frequency_map.at(1);
    // ... or a bit easier ...
    double f2_freq = nominal_frequency(2, satellite_system::gps);

    // the coefficients for L1 and L2 are
    double l1_coef = (f1_freq * f1_freq) / (f1_freq * f1_freq - f2_freq * f2_freq) ;
    double l2_coef = (f2_freq * f2_freq) / (f1_freq * f1_freq - f2_freq * f2_freq) ;
    l2_coef       *= -1.0;

    // we can construct L3 in two steps.
    // First initialize a new observable_type with the L1 component ...
    observation_type gps_l3c { satellite_system::gps,
                              observable_type::carrier_phase,
                              1, 'C', /* frequency band : 1 */
                              l1_coef
                             };
    // and then add the L2 component.
    gps_l3c.add_type( satellite_system::gps,
                      observable_type::carrier_phase,
                      2, 'C',        /* frequency band : 2 */
                      l2_coef
                    );

    std::cout << "\nThe GPS L3 observation type: " << gps_l3c;

    // We can construct L3 lc at once:
    // observable_type gps_l1c { satellite_system::gps,

    std::cout << "\n";
    return 0;
}
