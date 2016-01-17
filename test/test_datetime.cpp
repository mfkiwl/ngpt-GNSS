#include <iostream>
#include "datetime.hpp"

using ngpt::datetime;

int main()
{
    datetime<ngpt::datetime_clock::milli_seconds> d1 (2015, 12, 30);
    datetime<> d2 (2015, 12, 30);
    
    std::cout << "\n";
    return 0;   
}
