#include <iostream>
#include "sp3.hpp"

int main(int argv, char* argc[])
{
    if (argv != 2) {
        std::cerr<<"\nUsage: testSp3 <sp3 file>\n";
        return 1;
    }

    ngpt::sp3 sp3f (argc[1]);

    std::cout<<"\n";
    return 0;
}
