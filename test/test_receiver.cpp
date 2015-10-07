#include <iostream>
#include <climits>
#include "receiver.hpp"
/* #include "antenna.hpp" */

using namespace ngpt;

int main ()
{
    // Construct some Receiver instances, using various
    // constructors.
    std::string srec { " GEODETIC III L1/L2   " };
    char buf[] = {"JAVAD TR_G3 ALPHA                   FOO             BAR"};
    Receiver r1 { "JAVAD TR_G3 ALPHA    " };
    Receiver r2 { "JAVAD TR_G4 ALPHA" };
    Receiver r3 { buf };
    Receiver r4 { srec };
    Receiver r5 { std::string("JAVAD TR_G3 ALPHA tooooolongstring..") };


    // using the assignment operator
    Receiver r6, r7;
    r6 = "JAVAD TR_G3 ALPHA                   FOO             BAR";
    r7 = std::string("JAVAD TR");
    
    // Print the receiver names
    std::cout<<"Receiver r1 type: "<< r1.toString();
    std::cout<<"\nReceiver r2 type: "<< r2.toString();
    std::cout<<"\nReceiver r3 type: "<< r3.toString();
    std::cout<<"\nReceiver r4 type: "<< r4.toString();
    std::cout<<"\nReceiver r5 type: "<< r5.toString();
    std::cout<<"\nReceiver r6 type: "<< r6.toString();
    std::cout<<"\nReceiver r7 type: "<< r7.toString();
    
    // Check wether some receivers are the same
    std::cout<<"\nAre r1 and r2 the same ? " << ( (r1==r2)?"yes":"no" );
    std::cout<<"\nAre r1 and r3 the same ? " << ( (r1==r3)?"yes":"no" );
    std::cout<<"\nAre r1 and r4 the same ? " << ( (r1==r4)?"yes":"no" );
    
    // Print the size of each instance
    std::cout<<"\nSize of receiver object = " << sizeof(r1)  
      << " bytes or " << sizeof(r1)*CHAR_BIT << " bits";
 
    // The following should NOT compile! 
    /* std::cout << "\n" << receiver_max_chars << "\n"; */

 /*   
    // Construct some Antennas
    srec = "TPSCR3_GGD";
    char buf_a1[] = { "TRM12562.10+RGP        4000SLD L1/L2, round groundplane" };
    Antenna a1 { srec };
    Antenna a2 { buf_a1 };
    Antenna a3 { "BLOCK IIR-M          |  GPS Block IIR-M" }; // Satellite antenna
    Antenna a4 { "ASH701945C_M    SCIS" };
    Antenna a5 { "ASH701945C_M    " };
    
    // Print the antenna names
    std::cout<<"\nAntenna a1 type: "<< a1.to_string();
    std::cout<<"\nAntenna a2 type: "<< a2.to_string();
    std::cout<<"\nAntenna a3 type: "<< a3.to_string();
    std::cout<<"\nAntenna a4 type: "<< a4.to_string();
    std::cout<<"\nAntenna a5 type: "<< a5.to_string();
    
    // Check wether some antennas are the same
    std::cout<<"\nAre a4 and a1 the same ? " << ( (a4==a1)?"yes":"no" );
    std::cout<<"\nAre a4 and a2 the same ? " << ( (a4==a2)?"yes":"no" );
    std::cout<<"\nAre a4 and a3 the same ? " << ( (a4==a3)?"yes":"no" );
    std::cout<<"\nAre a4 and a5 the same ? " << ( (a4==a5)?"yes":"no" );
    
    // Change/set the radome of a5
    a5.set_radome( "SCIS " );
    std::cout<<"\nChanged radome of antenna a5 to : " << a5.radome_to_string();
    
    // Check wether some antennas are the same
    std::cout<<"\nAre a4 and a1 the same ? " << ( (a4==a1)?"yes":"no" );
    std::cout<<"\nAre a4 and a2 the same ? " << ( (a4==a2)?"yes":"no" );
    std::cout<<"\nAre a4 and a3 the same ? " << ( (a4==a3)?"yes":"no" );
    std::cout<<"\nAre a4 and a5 the same ? " << ( (a4==a5)?"yes":"no" );
*/

    std::cout<<"\n";
    return 0;
}
