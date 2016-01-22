#include <iostream>
#include <chrono>
#include "datetime.hpp"

using ngpt::datetime;

int main()
{
    datetime<ngpt::datetime_clock::milli_seconds> d1 (2015, 12, 30);
            
    ngpt::datetime_v2<ngpt::seconds> dd1 (2015, 12, 30);
    ngpt::datetime_v2<ngpt::milliseconds> dd2 (2015, 12, 30);
    ngpt::datetime_v2<ngpt::nanoseconds> dd3 (2015, 12, 30);

    std::cout<<"\nSize of v1 class: "<< sizeof(d1);
    std::cout<<"\nSize of v2 class: "<< sizeof(dd3);

    std::chrono::steady_clock::time_point begin, end;
    double mjd;

    begin = std::chrono::steady_clock::now();
    for (int i=0; i<86400 * 2.5; ++i) {
        d1.add_seconds( 1.0e0 );
        mjd = d1.as_mjd();
    }
    end = std::chrono::steady_clock::now();

    std::cout << "\nAdding two days in v1: " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() <<std::endl;
    std::cout << "\nmjd= " << mjd;
    
    begin = std::chrono::steady_clock::now();
    //ngpt::nanoseconds s (1L);
    for (int i=0; i<86400 * 2.5; ++i) {
        dd3.add_seconds( ngpt::nanoseconds(1L) );
        mjd = dd3.as_mjd();
    }
    end = std::chrono::steady_clock::now();
    std::cout << "\nAdding two days in v2: " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() <<std::endl;
    std::cout << "\nmjd= " << mjd;

    std::cout << "\n";
    return 0;   
}
