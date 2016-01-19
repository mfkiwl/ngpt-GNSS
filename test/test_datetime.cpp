#include <iostream>
#include "datetime.hpp"

using ngpt::datetime;

int main()
{
    datetime<ngpt::datetime_clock::milli_seconds> d1 (2015, 12, 30);
    datetime<> d2 (2015, 12, 30);

    for (int i=0; i<86401; ++i) {
        d2.add_seconds((double)i);
    }
    
    std::cout << "\n";
    return 0;   
}
