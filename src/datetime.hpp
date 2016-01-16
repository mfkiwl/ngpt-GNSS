#ifndef __DATETIME_NGPT__
#define __DATETIME_NGPT__

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

/// Calendar date to MJD.
long cal2mjd(int, int, int);

/// Convert hours, minutes, seconds into fractional days.
double hms2fd(int, int, double) noexcept;

/// Decompose fraction days to hours, minutes, seconds and fractional sec
/// with a gicen precision.
void fd2hms(double, int, int ihmsf[4]);

/// Return true if given year is leap.
bool
is_leap(int iy)
noexcept
{
    return !(iy%4) && (iy%100 || !(iy%400));
}

namespace datetime_clock {
    template<int N>
    struct clock {};

    template<> 
    struct clock<0>
    {
        static constexpr double sec_tolerance { 1 };
        static constexpr double day_tolerance { 1 / sec_per_day };
    };

    template<>    
    struct clock<3>
    {
        static constexpr double sec_tolerance { 1e-3 };
        static constexpr double day_tolerance { 1e-3 / sec_per_day };
    };
    
    template<>
    struct clock<6>
    {
        static constexpr double sec_tolerance { 1e-6 };
        static constexpr double day_tolerance { 1e-6 / sec_per_day };
    };

    typedef clock<0> seconds;
    typedef clock<3> milli_seconds;
    typedef clock<6> nano_seconds;
}

template<class C = datetime_clock::milli_seconds>
class datetime {
public:

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

    inline
    bool operator==(const datetime& lhs)
    const noexcept
    {
        return ( mjd_ == lhs.mjd_ &&
                 std::abs(fd_ - lhs.fd) < C::day_tolerance );
    }
    
    inline
    bool operator>(const datetime& lhs)
    const noexcept
    {
        return ( mjd_ > lhs.mjd_ || 
               ( mjd_ == lhs.mjd_ && (fd_ - lhs.fd > C::day_tolerance) ) );
    }
    
    inline
    bool operator>=(const datetime& lhs)
    const noexcept
    {
        return ( mjd_ > lhs.mjd_ || 
               ( mjd_ == lhs.mjd_ && (fd_ - lhs.fd >= C::day_tolerance) ) );
    }
    
    inline
    bool operator<(const datetime& lhs)
    const noexcept
    {
        return ( mjd_ < lhs.mjd_ || 
               ( mjd_ == lhs.mjd_ && (fd_ - lhs.fd < C::day_tolerance) ) );
    }
    
    inline
    bool operator<=(const datetime& lhs)
    const noexcept
    {
        return ( mjd_ < lhs.mjd_ || 
               ( mjd_ == lhs.mjd_ && (fd_ - lhs.fd <= C::day_tolerance) ) );
    }

private:
    long   mjd_;
    double fd_;
};

template<class C = datetime_clock::milli_seconds>
constexpr
datetime<C> min_date()
noexcept
{
    return datetime<C>{1821, 1, 1};
}

template<class C = datetime_clock::milli_seconds>
constexpr
datetime<C> max_date()
noexcept
{
    return datetime<C>{2500, 1, 1};
}

} // end namespace

#endif
