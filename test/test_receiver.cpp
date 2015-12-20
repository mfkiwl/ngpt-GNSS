#include <iostream>
#include <climits>
#include "receiver.hpp"

using namespace ngpt;

int main ()
{
    // Construct some receiver instances, using various
    // constructors.
    std::string srec { " GEODETIC III L1/L2   " };
    char buf[] = {"JAVAD TR_G3 ALPHA                   FOO             BAR"};
    receiver r1 { "JAVAD TR_G3 ALPHA    " };
    receiver r2 { "JAVAD TR_G4 ALPHA" };
    receiver r3 { buf };
    receiver r4 { srec };
    receiver r5 { std::string("JAVAD TR_G3 ALPHA tooooolongstring..") };


    // using the assignment operator
    receiver r6, r7;
    r6 = "JAVAD TR_G3 ALPHA                   FOO             BAR";
    r7 = std::string("JAVAD TR");
    
    // Print the receiver names
    std::cout<<"receiver r1 type: ["<< r1.toString() << "]";
    std::cout<<"\nreceiver r2 type: ["<< r2.toString() << "]";
    std::cout<<"\nreceiver r3 type: ["<< r3.toString() << "]";
    std::cout<<"\nreceiver r4 type: ["<< r4.toString() << "]";
    std::cout<<"\nreceiver r5 type: ["<< r5.toString() << "]";
    std::cout<<"\nreceiver r6 type: ["<< r6.toString() << "]";
    std::cout<<"\nreceiver r7 type: ["<< r7.toString() << "]";
    
    // Check wether some receivers are the same
    std::cout<<"\nAre r1 and r2 the same ? " << ( (r1==r2)?"yes":"no" );
    std::cout<<"\nAre r1 and r3 the same ? " << ( (r1==r3)?"yes":"no" );
    std::cout<<"\nAre r1 and r4 the same ? " << ( (r1==r4)?"yes":"no" );
    
    // Print the size of each instance
    std::cout<<"\nSize of receiver object = " << sizeof(r1)  
      << " bytes or " << sizeof(r1)*CHAR_BIT << " bits";

    // validate receiver models.
    /*
    std::cout<<"\nIs ["<< r1.toString() << "] a valid name ? " << ( (r1.validate())?"yes":"no" );
    std::cout<<"\nIs ["<< r2.toString() << "] a valid name ? " << ( (r2.validate())?"yes":"no" );
    std::cout<<"\nIs ["<< r3.toString() << "] a valid name ? " << ( (r3.validate())?"yes":"no" );
    std::cout<<"\nIs ["<< r4.toString() << "] a valid name ? " << ( (r4.validate())?"yes":"no" );
    std::cout<<"\nIs ["<< r5.toString() << "] a valid name ? " << ( (r5.validate())?"yes":"no" );
    std::cout<<"\nIs ["<< r6.toString() << "] a valid name ? " << ( (r6.validate())?"yes":"no" );
    std::cout<<"\nIs ["<< r7.toString() << "] a valid name ? " << ( (r7.validate())?"yes":"no" );
    */
 
    // The following should NOT compile! 
    /* std::cout << "\n" << receiver_max_chars << "\n"; */

    std::cout<<"\n";
    return 0;
}
