#include <iostream>
#include <chrono>
#include "datetime.hpp"
#include "datetime_v2.hpp"

using ngpt::datetime;
using ngpt::datev2;

int main()
{
    datetime<ngpt::datetime_clock::milli_seconds> d1 (2015, 12, 30);
    datev2<ngpt::seconds> d2(ngpt::year(2015), ngpt::month(12), ngpt::day_of_month(30));
    d2.add_seconds(ngpt::seconds(10));
    
    // nice, the following won't compile
    //datev2<ngpt::year> d3(ngpt::year(2015), ngpt::month(12), ngpt::day_of_month(30));
 
    // this is fine; nanoseconds to seconds is allowed
    datev2<ngpt::seconds> d4(ngpt::year(2015), ngpt::month(12),
        ngpt::day_of_month(30), ngpt::nanoseconds(100));

    // the opposite however id not allowed!
    //datev2<ngpt::nanoseconds> d5(ngpt::year(2015), ngpt::month(12),
    //    ngpt::day_of_month(30), ngpt::seconds(100));
            
    std::cout<<"\nSize of v1 class: "<< sizeof(d1);
    std::cout<<"\nSize of v2 class: "<< sizeof(d2);

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
    for (int i=0; i<86400 * 2.5; ++i) {
        d2.add_seconds( ngpt::seconds(1L) );
        //mjd = dd1.as_mjd();
    }
    end = std::chrono::steady_clock::now();
    std::cout << "\nAdding two days in v2: " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() <<std::endl;
    std::cout << "\nmjd= " << mjd;

    std::cout << "\n";
    return 0;   
}
