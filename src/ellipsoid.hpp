#ifndef __REFERENCE_ELLIPSOID__
#define __REFERENCE_ELLIPSOID__

#include <cmath>

namespace ngpt {

enum class ellipsoid : char {
    grs80,
    wgs84,
    pz90
};

template<ellipsoid E>
struct ellipsoid_traits { };

template<>
struct ellipsoid_traits<ellipsoid::grs80>
{
    static constexpr double a      { 6378137.0e0 };
    static constexpr double f      { 1.0e00/298.257222101e0 };
    static constexpr const char* n { "GRS80" };
};

template<>
struct ellipsoid_traits<ellipsoid::wgs84>
{
    static constexpr double a      { 6378137.0e0 };
    static constexpr double f      { 1.0e00/298.257223563e0 };
    static constexpr const char* n { "WGS84" };
};

template<>
struct ellipsoid_traits<ellipsoid::pz90>
{
    static constexpr double a      { 6378135.0e0 };
    static constexpr double f      { 1.0e00/298.257839303e0 };
    static constexpr const char* n { "PZ90" };
};

template<ellipsoid E>
#if __cplusplus > 201103L
    constexpr double 
#else
    double
#endif
    eccentricity_squared()
    noexcept
{
    constexpr double f { ellipsoid_traits<E>::f };
    return ( 2.0e0 - f ) * f;
}

template<ellipsoid E>
#if __cplusplus > 201103L
    constexpr double 
#else
    double
#endif
    semi_minor()
    noexcept
{
  constexpr double a { ellipsoid_traits<E>::a };
  return a - eccentricity_squared<E>() * a;
}

/// Normal radious of curvature
/// Physical Geodesy, p. 194
template<ellipsoid E>
    double
    N (double lat) 
    noexcept
{
    constexpr double a { ellipsoid_traits<E>::a };

    double cosf  { std::cos(lat) };
    double sinf  { std::sin(lat) };
    double acosf { a * cosf };
    double bsinf { semi_minor<E>() * sinf };
    double den   { std::sqrt(acosf*acosf + bsinf*bsinf) };

    return (a * a) / den;
}

} // end namespace geodesy

#endif
