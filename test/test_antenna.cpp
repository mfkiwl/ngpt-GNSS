#include <iostream>
#include <climits>

#include "antenna.hpp" /* provides the ngpt::antenna class */

using namespace ngpt;  /* ngpt::antenna is too long */

int main ()
{
    std::string srec { "JAV_GRANT-G3T+G" };  /* an std::string */
    char buf[]     = {"LEICA ATX1230+GNSS"}; /* a c-string     */
    
    // Construct some antenna instances, using various constructors.
    antenna r1 { "NOV703GGG.R2   DOME" };      
    antenna r2 { "JAVAD TR_G4    ALPHA" };
    antenna r3 { buf };
    antenna r4 { srec };
    antenna r5 { std::string("LEIAR10         NONE12356022") }; /* including serial nr */

    // using the assignment operator
    antenna r6, r7;
    r6 = "TRIMBLE 4000SSI-SS";
    r7 = std::string("TRIMBLE R7 GNSS");
    
    // Print the antennas
    std::cout<<"\nAntenna r1 type: ["<< r1.to_string() << "]";
    std::cout<<"\nAntenna r2 type: ["<< r2.to_string() << "]";
    std::cout<<"\nAntenna r3 type: ["<< r3.to_string() << "]";
    std::cout<<"\nAntenna r4 type: ["<< r4.to_string() << "]";
    std::cout<<"\nAntenna r5 type: ["<< r5.to_string() << "]";
    std::cout<<"\nAntenna r6 type: ["<< r6.to_string() << "]";
    std::cout<<"\nAntenna r7 type: ["<< r7.to_string() << "]";

    // add some serial numbers
    r1.set_serial_nr("1440925503");
    r2.set_serial_nr("08430019");
    std::cout<<"\nAntenna r1 type: ["<< r1.to_string() << "]";
    std::cout<<"\nAntenna r2 type: ["<< r2.to_string() << "]";
    
    // Check wether some Antennas are the equal
    std::cout<<"\nAre r1 and r2 equal ? " << ( (r1==r2)?"yes":"no" );
    std::cout<<"\nAre r1 and r3 equal ? " << ( (r1==r3)?"yes":"no" );
    std::cout<<"\nAre r1 and r4 equal ? " << ( (r1==r4)?"yes":"no" );
    
    // Print the size of each instance
    std::cout<<"\nSize of Antenna object = " << sizeof(r1)  
      << " bytes or " << sizeof(r1)*CHAR_BIT << " bits";

    // validate Antenna models.
    /*
    std::cout<<"\nIs ["<< r1.toString() << "] a valid name ? " << ( (r1.validate())?"yes":"no" );
    std::cout<<"\nIs ["<< r2.toString() << "] a valid name ? " << ( (r2.validate())?"yes":"no" );
    std::cout<<"\nIs ["<< r3.toString() << "] a valid name ? " << ( (r3.validate())?"yes":"no" );
    std::cout<<"\nIs ["<< r4.toString() << "] a valid name ? " << ( (r4.validate())?"yes":"no" );
    std::cout<<"\nIs ["<< r5.toString() << "] a valid name ? " << ( (r5.validate())?"yes":"no" );
    std::cout<<"\nIs ["<< r6.toString() << "] a valid name ? " << ( (r6.validate())?"yes":"no" );
    std::cout<<"\nIs ["<< r7.toString() << "] a valid name ? " << ( (r7.validate())?"yes":"no" );
    */

    std::cout<<"\n";
    return 0;
}
