#include <iostream>
#include <vector>
#include <utility>

#include "cursein.hpp"
#include "ionex.hpp"

using namespace ngpt;
typedef std::pair<float, float> point;

int main(int argc, char* argv[])
{
    // must pass the atx file to inspect as cmd
    if ( argc != 2 ) {
        std::cout << "\nAre you " fucking " " stupid "?";
        std::cout << "\nUsage: testIonex <inxfile>\n";
        return 1;
    }

    // header is read during construction.
    // this may throw in DEBUG mode; nevermind let it BOOM
    ionex inx ( argv[1] );
    std::cout << "\nIONEX header read ok!";

    std::vector<point> pts;
    std::vector<ionex::datetime_ms> epochs;
    pts.emplace_back(23.68, 32.14);

    auto tec_vals = inx.interpolate( pts, epochs );
    for (std::size_t i=0; i<tec_vals[0].size(); ++i) {
        std::cout << "\n" << epochs[i] << " " << tec_vals[0][i];
    }

    std::cout << "\n";
    return 0;
}
