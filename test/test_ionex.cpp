#include <iostream>
#include "ionex.hpp"

using namespace ngpt;

int main(int argc, char* argv[])
{
    // must pass the atx file to inspect as cmd
    if ( argc != 2 ) {
        std::cout << "\nUsage: testIonex <inxfile>\n";
        return 1;
    }

    // header is read during construction.
    // this may throw in DEBUG mode; nevermind lt it BOOM
    ionex inx ( argv[1] );
    std::cout << "\nIONEX header read ok!";

    std::cout << "\n";
    return 0;
}
