#include <iostream>
#include "datetime.hpp"

using ngpt::datetime;

int main()
{
    datetime<ngpt::datetime_clock::milli_seconds> d1 (2015, 12, 30);
            
    ngpt::datetime_v2<ngpt::seconds> dd1 (2015, 12, 30);
    ngpt::datetime_v2<ngpt::milliseconds> dd2 (2015, 12, 30);
    ngpt::datetime_v2<ngpt::nanoseconds> dd3 (2015, 12, 30);

    std::cout << "\n";
    return 0;   
}
