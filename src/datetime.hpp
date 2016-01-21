#ifndef __DATETIME_NGPT__
#define __DATETIME_NGPT__

#include <ostream>
#include <cstdio>
#include <limits>
#ifdef DEBUG
    #include <iostream>
#endif

namespace ngpt {

/// Seconds per day.
constexpr double sec_per_day         { 86400.0e0 };

/// Days per Julian year.
constexpr double days_in_julian_year { 365.25e0 };

/// Days per Julian century.
constexpr double days_in_julian_cent { 36525.0e0 };

/// Reference epoch (J2000.0), Julian Date.
constexpr double j2000_jd            { 2451545.0e0 };

/// Reference epoch (J2000.0), Modified Julian Date.
constexpr double j2000_mjd           { 51544.5e0 };

/// Julian Date of Modified Julian Date zero.
constexpr double mjd0_jd             { 2400000.5e0 };

/// TT minus TAI (s)
constexpr double tt_minus_tai        { 32.184e0 };

/// Time-Scales
enum class time_scale : char
{ tai, tt, utc, ut1 };

/// Time-Formats
enum class datetime_format : char
{ ymd, ymdhms, gps, ydoy, jd, mjd };

/// Calendar date to MJD.
long cal2mjd(int, int, int);

/// MJD to calendar date.
//void mjd2cal(long, int&, int&, int&) noexcept;

/// Convert hours, minutes, seconds into fractional days.
double hms2fd(int, int, double) noexcept;

/// Decompose fractional days to hours, minutes, seconds and fractional seconds
/// with a given precision.
void fd2hms(double, int, int ihmsf[4]);

/// Return true if given year is leap.
inline bool is_leap(int iy) noexcept
{ return !(iy%4) && (iy%100 || !(iy%400)); }

namespace datetime_clock {
    template<int N> struct clock {};

    template<> struct clock<0>
    {
        static constexpr double sec_tolerance  { 1 };
        static constexpr double day_tolerance  { 1 / sec_per_day };
        static constexpr int    exp            { 1 };
        static constexpr bool   is_second_type { true };
    };

    template<> struct clock<3>
    {
        static constexpr double sec_tolerance  { 1e-3 };
        static constexpr double day_tolerance  { 1e-3 / sec_per_day };
        static constexpr int    exp            { 3 };
        static constexpr bool   is_second_type { true };
    };
    
    template<> struct clock<6>
    {
        static constexpr double sec_tolerance  { 1e-6 };
        static constexpr double day_tolerance  { 1e-6 / sec_per_day };
        static constexpr int    exp            { 6 };
        static constexpr bool   is_second_type { true };
    };

    //typedef clock<0> seconds;
    typedef clock<3> milli_seconds;
    typedef clock<6> nano_seconds;
}

class milliseconds;
class nanoseconds;

struct seconds {
    static constexpr bool   is_second_type { true };
    static constexpr long   to_sec         { 1L   };
    long value_;

    explicit constexpr seconds(long s=0) noexcept : value_(s) {};

    template<class T>
    constexpr T
    transform_to() noexcept {}
};

struct milliseconds {
    static constexpr bool   is_second_type { true };
    static constexpr long   to_sec         { 1L/1000L };
    long value_;
    
    explicit constexpr milliseconds(long s=0) noexcept : value_(s) {};
    
    template<class T>
    constexpr T
    transform_to() noexcept {}
};

struct nanoseconds {
    static constexpr bool   is_second_type { true };
    static constexpr long   to_sec         { 1L/1000000L };
    long value_;

    explicit constexpr nanoseconds(long s=0) noexcept : value_(s) {};
    
    template<class T>
    constexpr T
    transform_to() noexcept {}
};

template<>
constexpr milliseconds
seconds::transform_to() noexcept
{ return milliseconds(value_ * 1000L); }

template<>
constexpr nanoseconds
seconds::transform_to() noexcept
{ return nanoseconds(value_ * 1000000L); }

template<>
constexpr seconds
milliseconds::transform_to() noexcept
{ return seconds(value_ / 1000L); }

template<>
constexpr nanoseconds
milliseconds::transform_to() noexcept
{ return nanoseconds(value_ * 1000L); }

template<>
constexpr seconds
nanoseconds::transform_to() noexcept
{ return seconds(value_ / 1000000L); }

template<>
constexpr milliseconds
nanoseconds::transform_to() noexcept
{ return milliseconds(value_ / 1000L); }

template<typename To,
         typename = typename std::enable_if<To::is_sec_type>::type,
         typename From,
         typename = typename std::enable_if<From::is_sec_type>::type
         >
To resolve_sec(From f) noexcept
{ return f.transform_to<To>(); }

/// Check if long is big enough to hold two days in nanoseconds.
static_assert( 86400L  * 1000000L * 2 < std::numeric_limits<long>::max(),
    "FUCK! Long is not big enough to hold two days in nanoseconds" );

template<typename C>
class datetime_v2 {
public:
    
    explicit
    datetime_v2(int iy, int im, int id)
    : mjd_ ( cal2mjd(iy, im, id) ),
      csec_(0L)
    {}

    template<typename T,
             typename = typename std::enable_if<T::is_sec_type>::type
             >
    datetime_v2(int iy, int im, int id, T sec)
    : mjd_ ( cal2mjd(iy, im, id) ),
      csec_( resolve_sec<C,T>(sec) )
    {}

private:
    long mjd_;
    long csec_;
};

template<class C>
class datetime {
public:

    /// Default constructor (makes min date)
    constexpr explicit
    datetime()
    noexcept
        : mjd_{cal2mjd(1821, 1, 1)},
          fd_ {0.0f}
    {}

    /// Constructor from YYMMDD HH:MM:SS.SSS
    explicit 
    datetime(int year, int month, int day, 
             int hour = 0, int minute = 0, double sec = 0)
        : mjd_{cal2mjd(year, month, day) },
          fd_ {hms2fd(hour, minute, sec) }
    {}

    datetime(const datetime&)            noexcept = default;
    datetime(datetime&&)                 noexcept = default;
    datetime& operator=(const datetime&) noexcept = default;
    datetime& operator=(datetime&&)      noexcept = default;

    long   mjd() const noexcept { return mjd_; }
    double fd()  const noexcept { return fd_;  }

    template<typename T>
    void
    add_seconds()
    {
        //TODO
    }

    inline
    bool operator==(const datetime& lhs)
    const noexcept
    {
        return ( mjd_ == lhs.mjd_ &&
                 std::abs(fd_ - lhs.fd_) < C::day_tolerance );
    }
    
    inline
    bool operator>(const datetime& lhs)
    const noexcept
    {
        return ( mjd_ > lhs.mjd_ || 
               ( mjd_ == lhs.mjd_ && (fd_ - lhs.fd_ > C::day_tolerance) ) );
    }
    
    inline
    bool operator>=(const datetime& lhs)
    const noexcept
    {
        return ( mjd_ > lhs.mjd_ || 
               ( mjd_ == lhs.mjd_ && (fd_ - lhs.fd_ >= C::day_tolerance) ) );
    }
    
    inline
    bool operator<(const datetime& lhs)
    const noexcept
    {
        return ( mjd_ < lhs.mjd_ ||
               ( mjd_ == lhs.mjd_ && (fd_ - lhs.fd_ > C::day_tolerance) ) );
    }
    
    inline
    bool operator<=(const datetime& lhs)
    const noexcept
    {
        return ( mjd_ < lhs.mjd_ || 
               ( mjd_ == lhs.mjd_ && (fd_ - lhs.fd_ >= C::day_tolerance) ) );
    }

    friend
    std::ostream& operator<<(std::ostream& o, const datetime& d)
    {
        o << ( static_cast<double>(d.mjd_) + d.fd_ );
        return o;
    }

private:
    long   mjd_;
    double fd_;
};

template<class C = datetime_clock::milli_seconds>
constexpr
datetime<C> min_date()
noexcept
{ return datetime<C>{1821, 1, 1}; }

template<class C = datetime_clock::milli_seconds>
constexpr
datetime<C> max_date()
noexcept
{ return datetime<C>{2500, 1, 1}; }

/*
template<typename C, datetime_format FORMAT>
std::string dt_to_string(const datetime<C>& d)
{}

template<typename C>
std::string dt_to_string<C, datetime_format::ymd>(const datetime<C>& d)
{
    int year, month, day;
    mjd2cal(d.mjd(), year, month, day);
    static char str[4+1+2+1+2+1 = 11]; // TODO
    str[10] = '\0';
    std::sprintf(str, "%04i/%02i/%02i");
    return std::string( str );
}
template<typename C>
std::string dt_to_string<C, datetime_format::ymdhms>(const datetime<C>& d)
{
    //TODO
    int ihmsf[4];
    int year, month, day;
    mjd2cal(d.mjd(), year, month, day);
    fd2hms(d.fd(), C::exp, ihmsf);
    static char str[]; //TODO
    str[10] = '\0';
    std::sprintf(str, "%04i/%02i/%02i");
    return std::string( str );
}
*/

} // end namespace

#endif
