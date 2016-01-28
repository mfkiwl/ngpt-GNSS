#ifndef __DATETIME_V2_NGPT__
#define __DATETIME_V2_NGPT__

#include <ostream>
#include <iomanip>
#include <cstdio>
#include <limits>
#include <cassert>
#include <cmath>
#include <tuple>
#ifdef DEBUG
    #include <iostream>
#endif
//#include "datetime.hpp"

namespace ngpt {

/// Check if long is big enough to hold two days in nanoseconds.
static_assert( 86400L  * 1000000L * 2 < std::numeric_limits<long>::max(),
    "FUCK! Long is not big enough to hold two days in nanoseconds" );

/// Jan 1st 1980
constexpr long jan61980 { 44244L };

constexpr long jan11901 { 15385L };

/// Forward declerations
class year;
class month;
class day_of_month;
class day_of_year;
class day;
class modified_julian_day;
class julian_day;
class seconds;
class milliseconds;
class nanoseconds;

/// Format options
namespace datetime_format_options {
    enum class year_digits : char { two_digit, four_digit };
}

/// let's be a little more short.
//typedef datetime_format_options::year_digits::four_digit four_digit_year;
//typedef datetime_format_options::year_digits::two_digit  two_digit_year;
//using four_digit_year = datetime_format_options::year_digits::four_digit;
//using two_digit_year = datetime_format_options::year_digits::two_digit;

/// Forward declare the class's '<<' operators
template<datetime_format_options::year_digits F = datetime_format_options::year_digits::four_digit>
std::ostream& operator<<(std::ostream&, const year&);

/// A wrapper class for years.
class year {
    int y;
public:
    /// Years are represented as integers.
    typedef int underlying_type;
    /// Constructor.
    explicit constexpr year (underlying_type i) noexcept : y(i) {};
    /// Get the underlying int.
    constexpr underlying_type as_underlying_type() const noexcept
    { return y; }
    /// overload operator "<<"
    template<datetime_format_options::year_digits>
    friend std::ostream& operator<<(std::ostream&, const year&);
};

/// Define the '<<' operator(s)
template<datetime_format_options::year_digits F>
std::ostream& operator<<(std::ostream& o, const year& yr)
{
    o << std::setw(4) << yr.y;
    return o;
}

/// Specialization of 2-digit year
template<>
std::ostream& operator<<<datetime_format_options::year_digits::two_digit>(std::ostream& o, const year& yr)
{ 
    o << std::setw(2) << ( yr.y > 2000L ? (2000L - yr.y) : (yr.y - 1900L ) );
    return o;
}

/// A wrapper class for months.
class month {
    int m;
    constexpr static const char* short_names[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    constexpr static const char* long_names[] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };
public:
    /// Months are represented as int.
    typedef int underlying_type;
    /// how to represent (for output).
    enum class format : char { two_digit, short_name, long_name };
    /// Constructor.
    explicit constexpr month (underlying_type i) noexcept : m(i) {};
    /// Get the underlying int.
    constexpr underlying_type as_underlying_type() const noexcept
    { return m; }
    /// Access (get/set) the underlying type
    constexpr underlying_type& assign() noexcept { return m; }
    /// overload operator "<<"
    template<format f = format::two_digit>
    friend std::ostream& operator<<(std::ostream& o, const month& mm)
    { o << std::setw(2) << mm.m; return o; }
};

/// A wrapper class for days (in general!).
class day {
    int d;
public:
    /// Days are represented as int.
    typedef int underlying_type;
    /// Constructor.
    explicit constexpr day(underlying_type i) noexcept : d(i) {};
    /// Get the underlying int.
    constexpr underlying_type as_underlying_type() const noexcept
    { return d; }
    /// overload operator "<<"
    //friend std::ostream& operator<<(std::ostream& o, const day& dd)
    //{ o << dd.d; return o; }
};

/// A wrapper class for day of month.
class day_of_month {
    int d;
public:
    /// Days are represented as int.
    typedef int underlying_type;
    /// Constructor.
    explicit constexpr day_of_month(underlying_type i) noexcept : d(i) {};
    /// Get the underlying int.
    constexpr underlying_type as_underlying_type() const noexcept
    { return d; }
    /// Access (get/set) the underlying type
    constexpr underlying_type& assign() noexcept { return d; }
};

/// A wrapper class for Modified Julian Days.
class modified_julian_day {
    long m;
public:
    /// MJDs are represented as long ints.
    typedef long underlying_type;
    /// Constructor.
    explicit constexpr modified_julian_day(underlying_type i) noexcept
        : m(i) 
    {};
    /// Get the underlying long int.
    constexpr underlying_type as_underlying_type() const noexcept
    { return m; }
    /// Access (get/set) the underlying long.
    constexpr underlying_type& assign() noexcept { return m; }
    /// Define addition (between MJDs).
    constexpr void operator+=(const modified_julian_day& d) noexcept
    { m += d.m; }
    /// Define addition (between an MJDs and a day).
    constexpr void operator+=(const day& d) noexcept
    { m += static_cast<underlying_type>(d.as_underlying_type()); }
    /// Cast to year, day_of_year
    constexpr year to_ydoy(day_of_year&) const noexcept;
    /// Cast to year, month, day_of_month
    constexpr year to_ymd(month&, day_of_month&) const noexcept;
};

/// A wrapper class for Julian Days.
/// TODO JD have a fraction part!
class julian_day {
    long j;
public:
    /// MJDs are represented as long ints.
    typedef long underlying_type;
    explicit constexpr julian_day(long i) noexcept : j(i) {};
    constexpr long as_long() const noexcept { return j; }
    /*
    constexpr long& assign() noexcept { return j; }
    constexpr void operator+=(const julian_day& d) noexcept
    { j += d.j; }
    constexpr void operator+=(const day& d) noexcept
    { j += d.as_int(); }
    */
};

/// A wrapper class for day of year
class day_of_year {
    int d;
public:
    /// DOY represented by ints.
    typedef int underlying_type;
    /// Constructor.
    explicit constexpr day_of_year(underlying_type i=0) noexcept : d(i) {};
    /// Cast to underlying type
    constexpr underlying_type as_underlying_type() const noexcept
    { return d; }
};

class hours {
    int h;
public:
    /// Hours are represented by ints.
    typedef int underlying_type;
    /// Constructor
    explicit constexpr hours(underlying_type i) noexcept : h(i) {};
};

class minutes {
    int m;
public:
    /// Minutes are represented by ints
    typedef int underlying_type;
    /// Constructor
    explicit constexpr minutes(underlying_type i) noexcept : m(i) {};
};

/// A wrapper class for seconds.
class seconds {
    long s;
public:
    /// Seconds are represented as long ints.
    typedef long underlying_type;
    /// Seconds is a subdivision of seconds.
    static constexpr bool is_of_sec_type { true };
    /// Max seconds in day.
    static constexpr underlying_type max_in_day { 86400L };
    /// Constructor
    explicit constexpr seconds(underlying_type i=0L) noexcept : s(i) {};
    /// Addition operator between seconds.
    constexpr void operator+=(const seconds& sc) noexcept { s+=sc.s; }
    /// Division operator between seconds.
    //constexpr seconds operator/(const seconds& sc) noexcept
    //{ return seconds(s/sc.s); }
    /// Do the secods sum up to more than one day?
    constexpr bool more_than_day() const noexcept { return s>max_in_day; }
    /// Get the underlying type numeric.
    constexpr underlying_type as_underlying_type() const noexcept { return s; }
    /// Access (get/set) the underlying type (long).
    constexpr underlying_type& assign() noexcept { return s; }
    /// If the seconds sum up to more (or equal to) one day, remove the integer
    /// days (and return them); reset the seconds to seconds of the new day.
    constexpr day remove_days() noexcept {
        day d ( static_cast<day::underlying_type>(s/max_in_day) );
        s %= max_in_day;
        return d;
    }
    /// Return the integral number of days.
    constexpr day to_days() const noexcept {
        return day(static_cast<day::underlying_type>(s/max_in_day));
    }
    /// Interpret the seconds as fractional days.
    constexpr double fractional_days() const noexcept {
        return static_cast<double>(s)/static_cast<double>(max_in_day);
    }
    /// Cast to double (i.e. fractional seconds)
    constexpr double to_fractional_seconds() const noexcept
    { return static_cast<double>(s); }
    /// Translate to hours minutes and seconds
    constexpr std::tuple<hours, minutes, seconds> to_hms() const noexcept
    {
        return std::make_tuple(hours(s / 3600),
                               minutes((s % 3600) / 60),
                               seconds((s % 3600) % 60) );
    }
};

/// A wrapper class to represent a datetime in GPSTime, i.e. gps week and
/// seconds of (gps) week.
class gps_datetime {
    long   week_;
    double sec_of_week_;
public:
    explicit constexpr gps_datetime(long w, double s) noexcept
        : week_(w), sec_of_week_(s) {};
};

/// A wrapper class for milliseconds.
class milliseconds {
    long s;
public:
    /// MilliSeconds are represented as long ints.
    typedef long underlying_type;
    /// MilliSeconds are a subdivision of seconds.
    static constexpr bool is_of_sec_type { true };
    /// Max milliseconds in one day.
    static constexpr long max_in_day { 86400L * 1000L };
    /// Cinstructor.
    explicit constexpr milliseconds(underlying_type i=0L) noexcept : s(i) {};
    /// Milliseconds can be cast to seconds (with a loss of precission).
    constexpr explicit operator seconds() const { return seconds(s/1000L); }
    /// Overload operator "+=" between milliseconds.
    constexpr void operator+=(const milliseconds& ms) noexcept { s+=ms.s; }
    /// Overload operator "/" between milliseconds.
    //constexpr milliseconds operator/(const milliseconds& sc) noexcept
    //{ return milliseconds(s/sc.s); }
    /// Do the milliseconds sum up to more than one day ?
    constexpr bool more_than_day() const noexcept { return s>max_in_day; }
    /// Get the milliseconds cast to the underlying type.
    constexpr underlying_type as_underlying_type() const noexcept { return s; }
    /// Access (get/set) the underlying type (long int).
    constexpr underlying_type& assign() noexcept { return s; }
    /// If the milliseconds sum up to more (or equal to) one day, remove the 
    /// integral days (and return them); reset the milliseconds to milliseconds
    /// of the new day.
    constexpr day remove_days() noexcept {
        day d ( static_cast<day::underlying_type>(s/max_in_day) );
        s %= max_in_day;
        return d;
    }
    /// Return the milliseconds as whole day(s) .
    constexpr day to_days() const noexcept {
        return day(static_cast<day::underlying_type>(s/max_in_day));
    }
    /// Cast to fractional days.
    constexpr double fractional_days() const noexcept {
        return static_cast<double>(s)/static_cast<double>(max_in_day);
    }
    /// Cast to fractional seconds
    constexpr double to_fractional_seconds() const noexcept
    { return static_cast<double>(s)*1.0e-3; }
};

/// A wrapper class for nanoseconds.
class nanoseconds {
    long s;
public:
    /// Nanoseconds are represented as long integers.
    typedef long underlying_type;
    /// Nanoseconds is a subdivision of seconds.
    static constexpr bool is_of_sec_type { true };
    /// Max nanoseconds in day.
    static constexpr long max_in_day { 86400L * 1000000L };
    /// Constructor.
    explicit constexpr nanoseconds(underlying_type i=0L) noexcept : s(i) {};
    /// Nanoseconds can be cast to milliseconds will a loss of accuracy.
    constexpr explicit operator milliseconds() const
    { return milliseconds(s/1000L); }
    /// Nanoseconds can be cast to seconds will a loss of accuracy.
    constexpr explicit operator seconds() const { return seconds(s/1000000L); }
    /// Overload operatpr "+=" between nanoseconds.
    constexpr void operator+=(const nanoseconds& ns) noexcept { s+=ns.s; }
    /// Overload operatpr "/" between nanoseconds.
    //constexpr nanoseconds operator/(const nanoseconds& sc) noexcept
    //{ return nanoseconds(s/sc.s); }
    /// Do the nanoseconds sum up to more than one day?
    constexpr bool more_than_day() const noexcept { return s>max_in_day; }
    /// Cast to underlying type.
    constexpr underlying_type as_underlying_type() const noexcept { return s; }
    /// Access (set/get) the underlying type.
    constexpr underlying_type& assign() noexcept { return s; }
    /// If the nanoseconds sum up to more (or equal to) one day, remove the
    /// integral days (and return them); reset the nanoseconds to nanoseconds
    /// of the new day.
    constexpr day remove_days() noexcept {
        day d ( static_cast<int>(s/max_in_day) );
        s %= max_in_day;
        return d;
    }
    /// Cast to days.
    constexpr day to_days() const noexcept {
        return day(static_cast<day::underlying_type>(s/max_in_day));
    }
    /// Cast to fractional days.
    constexpr double fractional_days() const noexcept {
        return static_cast<double>(s)/static_cast<double>(max_in_day);
    }
    /// Cast to fractional seconds
    constexpr double to_fractional_seconds() const noexcept
    { return static_cast<double>(s)*1.0e-6; }
};

/// Calendar date (i.e. year, momth, day) to MJDay.
modified_julian_day
cal2mjd(year, month, day_of_month);

/// Valid output formats
enum class datetime_output_format : char {
    ymd, ymdhms, ydhms, gps, ydoy, jd, mjd
};

/*
 * A datetime class. Holds (integral) days as MJD and fraction of day as any
 * of the is_of_sec_type class (i.e. seconds/milli/nano).
 */
template<class S>
class datev2 {
public:

    /// Only allow S parameter to be of sec type (seconds/milli/nano).
    static_assert( S::is_of_sec_type, "" );
    
    /// Default (zero) constructor.
    explicit constexpr datev2() noexcept : mjd_(0), sect_(0) {};
    
    /// Constructor from year, month, day of month anss any sec type.
    explicit constexpr datev2(year y, month m, day_of_month d, S s=S())
        : mjd_ (cal2mjd(y, m, d)),
          sect_(s)
        {}

    template<class T>
    explicit datev2(year y, month m, day_of_month d, T t)
        : mjd_ (cal2mjd(y, m, d)),
          sect_(S(t))
    {}

    template<class T>
    constexpr void add_seconds(T t) noexcept
    { 
        sect_ += (S)t;
        if ( sect_.more_than_day() ) this->normalize();
        return;
    }

    /// Overload equality operator.
    constexpr bool operator==(const datev2& d) const noexcept
    { return mjd_ == d.mjd_ && sect_ == d.sect; }

    /// Overload ">" operator.
    constexpr bool operator>(const datev2& d) const noexcept
    { return mjd_ > d.mjd_ || (mjd_ == d.mjd_ && sect_ > d.sect); }
    
    /// Overload ">=" operator.
    constexpr bool operator>=(const datev2& d) const noexcept
    { return mjd_ > d.mjd_ || (mjd_ == d.mjd_ && sect_ >= d.sect); }
    
    /// Overload "<" operator.
    constexpr bool operator<(const datev2& d) const noexcept
    { return mjd_ < d.mjd_ || (mjd_ == d.mjd_ && sect_ < d.sect); }
    
    /// Overload "<=" operator.
    constexpr bool operator<=(const datev2& d) const noexcept
    { return mjd_ < d.mjd_ || (mjd_ == d.mjd_ && sect_ <= d.sect); }

    /// Reset the seconds/milli/nano after removing whole days.
    constexpr void normalize() noexcept
    {
        mjd_ += sect_.remove_days();
        return;
    }

    /// Cast to double Modified Julian Date.
    constexpr double as_mjd() const noexcept
    {
        return static_cast<double>(mjd_.as_underlying_type())
                                + sect_.fractional_days();
    }

    /// Cast to gps_datetime.
    constexpr gps_datetime as_gps_datetime() const noexcept
    {
        long week   = (mjd_.as_underlying_type() - jan61980)/7L;
        double secs = sect_.as_fractional_seconds();
        secs       += static_cast<double>(
                    ((mjd_.as_underlying_type() - jan61980) - week*7L ) * 86400L);
        return gps_datetime( week, secs );
    }

    /// Overload operatoe "<<"
    template<datetime_output_format F>
    friend
    std::ostream& operator<<(std::ostream&, const datev2&);
        
private:
    modified_julian_day mjd_;  ///< Modified Julian Day
    S                   sect_; ///< Fraction of day in milli/nano/seconds
};

/*
template<>
std::ostream& datev2::operator<<<datetime_output_format::ymd>(std::ostream& o, const datev2& d)
{
    o
}
*/

} // end namespace

#endif // define DATETIME
