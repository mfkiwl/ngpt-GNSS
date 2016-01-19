#include <iostream>
#include <vector>
#include <utility>
#include "ionex.hpp"

using namespace ngpt;

typedef std::pair<float, float> point;

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

    std::vector<point> pts;
    pts.emplace_back(-180.0, 87.0);

    inx.get_tec_at( pts );

    std::cout << "\n";
    return 0;
}
