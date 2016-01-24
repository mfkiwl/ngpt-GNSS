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
    typedef int underlying_type;
    explicit constexpr year (int i) noexcept : y(i) {};
    constexpr int as_int() const noexcept { return y; }
};

class month {
    int m;
public:
    typedef int underlying_type;
    explicit constexpr month (int i) noexcept : m(i) {};
    constexpr int as_int() const noexcept { return m; }
};

class day {
    int d;
public:
    typedef int underlying_type;
    explicit constexpr day(int i) noexcept : d(i) {};
    constexpr int as_int() const noexcept { return d; }
};

class day_of_month {
    int d;
public:
    typedef int underlying_type;
    explicit constexpr day_of_month(int i) noexcept : d(i) {};
    constexpr int as_int() const noexcept { return d; }
};

class modified_julian_day {
    long m;
public:
    typedef long underlying_type;
    explicit constexpr modified_julian_day(long i) noexcept : m(i) {};
    constexpr long as_long() const noexcept { return m; }
    constexpr long& assign() noexcept { return m; }
    constexpr void operator+=(const modified_julian_day& d) noexcept
    { m += d.m; }
    constexpr void operator+=(const day& d) noexcept
    { m += d.as_int(); }
};

class julian_day {
    long j;
public:
    typedef long underlying_type;
    explicit constexpr julian_day(long i) noexcept : j(i) {};
    constexpr long as_long() const noexcept { return j; }
    constexpr long& assign() noexcept { return j; }
    constexpr void operator+=(const julian_day& d) noexcept
    { j += d.j; }
    constexpr void operator+=(const day& d) noexcept
    { j += d.as_int(); }
};

class seconds {
    long s;
public:
    static constexpr long max_in_day { 86400L };
    typedef long underlying_type;
    explicit constexpr seconds(long i=0L) noexcept : s(i) {};
    constexpr void operator+=(const seconds& sc) noexcept { s+=sc.s; }
    constexpr seconds operator/(const seconds& sc) noexcept
    { return seconds(s/sc.s); }
    static constexpr bool is_sec_type { true };
    constexpr bool more_than_day() const noexcept { return s>max_in_day; }
    constexpr long as_long() const noexcept { return s; }
    constexpr long& assign() noexcept { return s; }
    constexpr day to_days() const noexcept {
        return day(static_cast<int>(s/max_in_day));
    }
    constexpr double fractional_days() const noexcept {
        return static_cast<double>(s)/static_cast<double>(max_in_day);
    }
};

class milliseconds {
    long s;
public:
    static constexpr long max_in_day { 86400L * 1000L };
    typedef long underlying_type;
    explicit constexpr milliseconds(long i=0L) noexcept : s(i) {};
    constexpr explicit operator seconds() const { return seconds(s/1000L); }
    constexpr void operator+=(const milliseconds& ms) noexcept { s+=ms.s; }
    constexpr milliseconds operator/(const milliseconds& sc) noexcept
    { return milliseconds(s/sc.s); }
    static constexpr bool is_sec_type { true };
    constexpr bool more_than_day() const noexcept { return s>max_in_day; }
    constexpr long as_long() const noexcept { return s; }
    constexpr long& assign() noexcept { return s; }
    constexpr day to_days() const noexcept {
        return day(static_cast<int>(s/max_in_day));
    }
    constexpr double fractional_days() const noexcept {
        return static_cast<double>(s)/static_cast<double>(max_in_day);
    }
};

class nanoseconds {
    long s;
public:
    static constexpr long max_in_day { 86400L * 1000L };
    typedef long underlying_type;
    explicit constexpr nanoseconds(long i=0L) noexcept : s(i) {};
    constexpr explicit operator milliseconds() const { return milliseconds(s/1000L); }
    constexpr explicit operator seconds() const { return seconds(s/1000000L); }
    constexpr void operator+=(const nanoseconds& ns) noexcept { s+=ns.s; }
    constexpr nanoseconds operator/(const nanoseconds& sc) noexcept
    { return nanoseconds(s/sc.s); }
    static constexpr bool is_sec_type { true };
    constexpr bool more_than_day() const noexcept { return s>max_in_day; }
    constexpr long as_long() const noexcept { return s; }
    constexpr long& assign() noexcept { return s; }
    constexpr day to_days() const noexcept {
        return day(static_cast<int>(s/max_in_day));
    }
    constexpr double fractional_days() const noexcept {
        return static_cast<double>(s)/static_cast<double>(max_in_day);
    }
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
    constexpr void add_seconds(T t) noexcept
    { 
        //std::cout<<"\nSecs was -> "<< sect_.as_long();
        sect_ += (S)t;
        //std::cout<<" now became -> "<<sect_.as_long();
        if ( sect_.more_than_day() ) this->normalize();
        return;
    }

    constexpr void normalize() noexcept {
        //std::cout<<"\nNORMALIZING!";
        //std::cout<<"\nMJD="<<mjd_.as_long()<<", secs="<<sect_.as_long();
        long m = mjd_.as_long();
        long s = sect_.as_long();
        long d = S::max_in_day;
        m += s / d; 
        s %= d;
        mjd_.assign() = m;
        sect_.assign() = s;
        //std::cout<<"\nMJD="<<mjd_.as_long()<<", secs="<<sect_.as_long();
        return;
    }

    constexpr double as_mjd() const noexcept
    { return static_cast<double>(mjd_.as_long()) + sect_.fractional_days(); }
        
private:
    modified_julian_day mjd_;  ///< Modified Julian Day
    S                   sect_; ///< Fraction of day is milli/nano/seconds
};

} // end namespace

#endif // define DATETIME
