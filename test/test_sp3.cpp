#include <iostream>
#include "sp3.hpp"

int main(int argv, char* argc[])
{
    if (argv != 2) {
        std::cerr<<"\nUsage: testSp3 <sp3 file>\n";
        return 1;
    }

    ngpt::sp3 sp3f (argc[1]);

    int status = 0;
    std::vector<ngpt::satellite>       sats;
    std::vector<ngpt::satellite_state> states;
    std::vector<ngpt::satellite_clock> clocks;
    ngpt::sp3::datetime_ms epoch;
    sats.reserve(sp3f.num_of_sats());
    states.reserve(sp3f.num_of_sats());
    clocks.reserve(sp3f.num_of_sats());

    std::cout<<"\nReading sp3 records.";
    while ( !(status = sp3f.get_next_epoch(epoch, sats, states, clocks)) ) {
        sats.clear();
        states.clear();
        clocks.clear();
    }
    if ( status != 999 ) {
        std::cerr<<"\nFailed reading satellite records.";
        return 1;
    }

    std::cout<<"\n";
    return 0;
}
