#ifndef __DATETIME_V2_NGPT__
#define __DATETIME_V2_NGPT__

#include <ostream>
#include <cstdio>
#include <limits>
#include <cassert>
#include <cmath>
#ifdef DEBUG
    #include <iostream>
#endif
#include "datetime.hpp"

namespace ngpt {

/// Check if long is big enough to hold two days in nanoseconds.
static_assert( 86400L  * 1000000L * 2 < std::numeric_limits<long>::max(),
    "FUCK! Long is not big enough to hold two days in nanoseconds" );

class year {
    int y;
public:
    explicit constexpr year (int i) noexcept : y(i) {};
    constexpr int as_int() const noexcept { return y; }
};

class month {
    int m;
public:
    explicit constexpr month (int i) noexcept : m(i) {};
    constexpr int as_int() const noexcept { return m; }
};

class day_of_month {
    int d;
public:
    explicit constexpr day_of_month(int i) noexcept : d(i) {};
    constexpr int as_int() const noexcept { return d; }
};

class modified_julian_day {
    long m;
public:
    explicit constexpr modified_julian_day(long i) noexcept : m(i) {};
};

class julian_day {
    long j;
public:
    explicit constexpr julian_day(long i) noexcept : j(i) {};
};

class seconds {
    long s;
    static constexpr long max_in_day { 86400L };
public:
    explicit constexpr seconds(long i=0L) noexcept : s(i) {};
    constexpr void operator+=(const seconds& sc) noexcept { s+=sc.s; }
    static constexpr bool is_sec_type { true };
};

class milliseconds {
    long s;
    static constexpr long max_in_day { 86400L * 1000L };
public:
    explicit constexpr milliseconds(long i=0L) noexcept : s(i) {};
    constexpr explicit operator seconds() const { return seconds(s/1000L); }
    constexpr void operator+=(const milliseconds& ms) noexcept { s+=ms.s; }
    static constexpr bool is_sec_type { true };
};

class nanoseconds {
    long s;
    static constexpr long max_in_day { 86400L * 1000L };
public:
    explicit constexpr nanoseconds(long i=0L) noexcept : s(i) {};
    constexpr explicit operator milliseconds() const { return milliseconds(s/1000L); }
    constexpr explicit operator seconds() const { return seconds(s/1000000L); }
    constexpr void operator+=(const nanoseconds& ns) noexcept { s+=ns.s; }
    static constexpr bool is_sec_type { true };
};

template<class S>
class datev2 {
public:

    /// Only allow S parameter to be of sec type (seconds/milli/nano).
    static_assert( S::is_sec_type, "" );

    explicit constexpr datev2() noexcept : mjd_(0), sect_(0) {};
    
    explicit datev2(year y, month m, day_of_month d, S s=S())
        : mjd_ (cal2mjd(y.as_int(), m.as_int(), d.as_int())),
          sect_(s)
        {}

    template<class T>
    explicit datev2(year y, month m, day_of_month d, T t)
        : mjd_(cal2mjd(y.as_int(), m.as_int(), d.as_int())),
          sect_(S(t))
    {}

    template<class T>
    constexpr void add_seconds(T&& t) noexcept
    { sect_+=(S)t; } 

        
private:
    modified_julian_day mjd_;  ///< Modified Julian Day
    S                   sect_; ///< Fraction of day is milli/nano/seconds
};

} // end namespace

#endif // define DATETIME
