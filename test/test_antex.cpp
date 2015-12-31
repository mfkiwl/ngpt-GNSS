#include <iostream>
#include "antex.hpp"
#include "antenna.hpp"

using namespace ngpt;

// print a bunch of details for an antenna pcv pattern, nothing fancy
void print_pcv_info(const antenna_pcv<pcv_type>&);

int main(int argc, char* argv[])
{
    // must pass the atx file to inspect as cmd
    if ( argc != 2 ) {
        std::cout << "\nUsage: testAntex <atxfile>\n";
        return 1;
    }

    // header is read during construction.
    antex atx (argv[1]);
    //int status;

    // an empty antenna pcv pattern
    antenna_pcv<pcv_type> pcv;

    // try to find a valid antenna in the atx file.
    antenna ant ("TRM14177.00     NONE");
    /*status = atx.find_antenna(ant);
    if (status>0) {
        std::cout << "\nAntenna not found!";
    } else if (status < 0) {
        std::cout << "\nError while reading antex!";
        return 1;
    } else {
        std::cout << "\nAntenna found!";
    }

    // now get the antenna pcv pattern
    pcv = atx.get_antenna_pattern( ant );

    // let's see some info of the antenna pattern
    print_pcv_info( pcv );
    */

    // cool! let's try again with a different antenna
    ant = "TRMSPS985       NONE";
    pcv = atx.get_antenna_pattern( ant );
    //print_pcv_info( pcv );
    // let's print the NOAZI pcv
    for ( float zen = .1; zen < 90.0 ; zen += 1.0 ) {
        std::cout << "\n\tZenith = " << zen << " pcv = " << pcv.no_azi_pcv(zen, 0);
        for ( float azi = .1; azi < 360.0 ; azi += 1.0 ) {
            std::cout << "\n\t" << zen << " " << azi << " " << pcv.azi_pcv(zen, azi, 0); 
        }
    }

    // exit.
    std::cout <<"\nAll done!\n";
    return 0;
}

void print_pcv_info(const antenna_pcv<pcv_type>& pcv)
{
    // let's see some info of the antenna pattern
    std::cout <<"\nAntenna Pattern: number of NOAZI values: " << pcv.no_azi_grid_pts();
    std::cout <<"\nAntenna Pattern: number of AZI values  : " << pcv.azi_grid_pts();
    std::cout <<"\nNOAZI grid: from zen=" << pcv.zen1() 
              << " to zen=" << pcv.zen2() 
              << " with step size=" << pcv.dzen();
    if ( pcv.azi_grid_pts() ) {
        std::cout <<"\nAZI grid: from   azi=" << pcv.azi1() 
                  << " to azi=" << pcv.azi2() 
                  << " with step size=" << pcv.dazi();
    }
}
