#include <stdexcept>
#include <cmath>
#include "datetime.hpp"

/// Calendar date (i.e. year-month-day) to Modified Julian Date.
///
/// \return    The Modified Julian Date (as \c long).
///
/// \throw     A runtime_error if the month and/or day is invalid.
///
/// Reference: iauCal2jd
///
long ngpt::cal2mjd(int iy, int im, int id)
{
    // Month lengths in days
    static constexpr int mtab[] = 
        {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Validate month
    if ( im < 1 || im > 12 ) {
        throw std::out_of_range("ngpt::cal2mjd -> Invalid Month.");
    }

    // If February in a leap year, 1, otherwise 0
    int ly ( (im == 2) && /*ngpt::*/is_leap(iy) );

    // Validate day, taking into account leap years
    if ( (id < 1) || (id > (mtab[im-1] + ly))) {
        throw std::out_of_range("ngpt::cal2mjd -> Invalid Day of Month.");
    }

    // Compute mjd
    int  my    { (im-14) / 12 };
    long iypmy { static_cast<long>(iy + my) };

        
    return  (1461L * (iypmy + 4800L)) / 4L
            + (367L * static_cast<long>(im - 2 - 12 * my)) / 12L
            - (3L * ((iypmy + 4900L) / 100L)) / 4L
            + static_cast<long>(id) - 2432076L;
}

/// Convert  hours, minutes, seconds to fractional days.
/// \warning No check is performed for the limits of input arguments (i.e.
///          \c hours can be > 24).
///          The computation is performed using the absolute values of the
///          input arguments.
///
double
ngpt::hms2fd(int h, int m, double s)
noexcept
{
    return ( 60.0 * ( 60.0 * ( static_cast<double>(std::abs(h)))  +
                             ( static_cast<double>(std::abs(m)))) +
                             std::abs(s))   / ngpt::sec_per_day;
}

/*
 * Convert Modified Julian Date to Calendar data (i.e. year, month, day)
 *
 * Reference: Source code for the Remondi Date/Time Algorithms, GPS-TOOLBOX,
 *            http://www.ngs.noaa.gov/gps-toolbox/bwr-02.htm
 */
void
ngpt::mjd2cal(long mjd, double fd)
noexcept
{
    static long month_day[2][13] = {
        {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365},
        {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366}
    };

    long days_fr_jan1_1901 = mjd - 15385L;
    long num_four_yrs      = days_fr_jan1_1901/1461L;
    long years_so_far      = 1901L + 4*num_four_yrs;
    long days_left         = days_fr_jan1_1901 - 1461*num_four_yrs;
    long delta_yrs         = days_left/365 - days_left/1460;

    long year              = years_so_far + delta_yrs;
    long yday              = days_left - 365*delta_yrs + 1;
    long leap              = ( year%4 == 0 );
    long guess             = yday*0.032;
    long more              = (( yday - month_day[leap][guess+1] ) > 0);
    
    month = static_cast<int>(guess + more + 1);
    mday  = static_cast<int>(yday - month_day[leap][guess+more]);
    iyear = static_cast<int>(year);
}

template<typename T>
    inline constexpr
    T sofa_dint(T a)
    noexcept
{
    return a < (T)0 ? std::ceil(a) : std::floor(a);
}

template<typename T>
    inline constexpr
    T sofa_dnint(T a)
    noexcept
{
    return a < (T)0 
           ? std::ceil (a - (T)0.5) 
           : std::floor(a + (T).05);
}

/// Decompose (fractional) days to hours, minutes, seconds, fraction.
/// The argument \c ndp must be a positive integer and is interpreted as 
/// follows:
/// ndp | resolution
/// ------------------
/// 0   | 0 00 01
/// 1   | 0 00 00.1
/// 2   | 0 00 00.01
/// 3   | 0 00 00.001
/// :   | ...
/// The largest positive useful value for ndp is determined by the
/// size of days, the format of double on the target platform, and
/// the risk of overflowing ihmsf[3].
///
/// \warning
///          -# \c ndp should be at max 12
///          -# The caller has to test for and handle the case where days is 
///             very nearly 1.0 and rounds up to 24 hours, by testing for 
///             ihmsf[0]=24 and setting ihmsf[0-3] to zero.
///
/// Reference iauD2tf (shamelesly stolen!)
///
void
ngpt::fd2hms(double days, int ndp, int ihmsf[4])
{
    // Express in seconds
    double a { days * ngpt::sec_per_day };

    // Express the unit of each field in resolution units.
    int nrs { 1 };
    for (int n = 1; n <= ndp; n++) {
        nrs *= 10;
    }
    double rs { static_cast<double>(nrs) };
    double rm { rs * 60.0 };
    double rh { rm * 60.0 };

    // Round the interval and express in resolution units.
    a = sofa_dnint(rs * a);

    // Break into fields.
    double ah { a / rh };
    ah = sofa_dint(ah);
    a -= ah * rh;
    double am { a / rm };
    am = sofa_dint(am);
    a -= am * rm;
    double as { a / rs };
    as = sofa_dint(as);
    double af { a - as * rs };

    // Format the result
    ihmsf[0] = static_cast<int>( ah );
    ihmsf[1] = static_cast<int>( am );
    ihmsf[2] = static_cast<int>( as );
    ihmsf[3] = static_cast<int>( af );
}
