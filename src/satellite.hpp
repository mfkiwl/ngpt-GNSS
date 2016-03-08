#ifndef __GNSS_SATELLITES_HPP
#define __GNSS_SATELLITES_HPP

#include <type_traits>
#include "satsys.hpp"

namespace ngpt
{

class satellite
{
public:
    /// Default constructor
    explicit satellite(ngpt::satellite_system s=ngpt::satellite_system::mixed,
                       int prn=0) noexcept
        : _sys{s}, _prn{prn}
    {}
    /// Constructor from a string
    explicit satellite(const char*);
    /// Copy constructor
    satellite(const satellite&) noexcept = default;
    /// Move Constructor
    satellite(satellite&&) noexcept = default;
    /// Assignment
    satellite& operator=(const satellite&) noexcept = default;
    /// Move assignment
    satellite& operator=(satellite&&) noexcept = default;
    /// Destructor
    ~satellite() noexcept {};
    /// Equality operator
    bool operator==(const satellite& rhs) const noexcept
    { return _sys == rhs._sys && _prn == rhs._prn; }
    /// InEquality operator
    bool operator!=(const satellite& rhs) const noexcept
    { return !(*this == rhs); }
private:
    ngpt::satellite_system _sys;
    int                    _prn;
}; // satellite


/// Enumeration type to classify a satellite state vector.
class satellite_state_flag
{
public:
    /// underlying enumeration type.
    typedef int flag_integral_type;
private:
    /// The actual flag
    flag_integral_type _flag;
public:
    /// A satellite state vector can be:
    enum class flag_type : flag_integral_type
    {
        bad_or_absent = 1,
        unknown_acc   = 2,
        maneuver      = 4,
        prediction    = 8,
        no_velocity   = 16,
        no_vel_acc    = 32
    };
    /// Default constructor (nothing set)
    explicit satellite_state_flag() : _flag{0} {};
    /// Constructor from state
    explicit satellite_state_flag(flag_type f)
        : _flag(static_cast<flag_integral_type>(f))
    {}
    /// Cast to (flag) underlying type
    flag_integral_type as_integral() const noexcept { return _flag; }
    /// Set a flag
    void set(flag_type f) noexcept
    { _flag |= static_cast<flag_integral_type>(f); }
    /// Clear a flag
    void clear(flag_type f) noexcept
    { _flag &= ~(static_cast<flag_integral_type>(f)); }
    /// Check if a flag is set
    bool check(flag_type f) const noexcept
    { return _flag & static_cast<flag_integral_type>(f); }

}; // satellite_position_flag

class satellite_state
{
public:
    explicit satellite_state(double x, double y, double z,
              double sx=.0f, double sy=.0f, double sz=.0f,
              satellite_state_flag f=satellite_state_flag{},
              double vx=.0f, double vy=.0f, double vz=.0f,
              double svx=.0f, double svy=.0f, double svz=.0f)
    noexcept
        : _x{x}, _y{y}, _z{z}, _sx{sx}, _sy{sy}, _sz{sz},
          _vx{vx}, _vy{vy}, _vz{vz}, _svx{svx}, _svy{svy}, _svz{svz},
          _flag{f}
    {}

    satellite_state_flag& flag()       noexcept { return _flag; }
    satellite_state_flag  flag() const noexcept { return _flag; }

    double&   x()       noexcept { return _x; }
    double    x() const noexcept { return _x; }
    double&   y()       noexcept { return _y; }
    double    y() const noexcept { return _y; }
    double&   z()       noexcept { return _z; }
    double    z() const noexcept { return _z; }
    double&  sx()       noexcept { return _sx; }
    double   sx() const noexcept { return _sx; }
    double&  sy()       noexcept { return _sy; }
    double   sy() const noexcept { return _sy; }
    double&  sz()       noexcept { return _sz; }
    double   sz() const noexcept { return _sz; }
    double&  vx()       noexcept { return _vx; }
    double   vx() const noexcept { return _vx; }
    double&  vy()       noexcept { return _vy; }
    double   vy() const noexcept { return _vy; }
    double&  vz()       noexcept { return _vz; }
    double   vz() const noexcept { return _vz; }
    double& svx()       noexcept { return _svx; }
    double  svx() const noexcept { return _svx; }
    double& svy()       noexcept { return _svy; }
    double  svy() const noexcept { return _svy; }
    double& svz()       noexcept { return _svz; }
    double  svz() const noexcept { return _svz; }

private:
    double _x,   _y,   _z;
    double _sx,  _sy,  _sz;
    double _vx,  _vy,  _vz;
    double _svx, _svy, _svz;
    satellite_state_flag _flag;

}; // satellite_state

/// Enumeration type to classify a satellite clock information (correction).
class satellite_clock_flag
{
public:
    /// underlying enumeration type.
    typedef int flag_integral_type;
private:
    /// The actual flag
    flag_integral_type _flag;
public:
    /// A satellite state vector can be:
    enum class flag_type : flag_integral_type
    {
        bad_or_absent = 1,
        unknown_acc   = 2,
        discontinuity = 4,
        prediction    = 8,
        no_velocity   = 16,
        no_vel_acc    = 32
    };
    /// Default constructor (nothing set)
    explicit satellite_clock_flag() : _flag{0} {};
    /// Constructor from state
    explicit satellite_clock_flag(flag_type f)
        : _flag(static_cast<flag_integral_type>(f))
    {}
    /// Cast to (flag) underlying type
    flag_integral_type as_integral() const noexcept { return _flag; }
    /// Set a flag
    void set(flag_type f) noexcept
    { _flag |= static_cast<flag_integral_type>(f); }
    /// Clear a flag
    void clear(flag_type f) noexcept
    { _flag &= ~(static_cast<flag_integral_type>(f)); }
    /// Check if a flag is set
    bool check(flag_type f) const noexcept
    { return _flag & static_cast<flag_integral_type>(f); }

}; // satellite_clock_flag

/// A class to hold a satellite's clock correction (and drift).
class satellite_clock
{
public:
    explicit satellite_clock(double c, double sc=.0f,
                satellite_clock_flag f=satellite_clock_flag{},
                double vc=.0f, double svc=.0f)
    noexcept
        : _c{c}, _sc{sc}, _vc{vc}, _svc{svc}, _flag{f}
    {}
    
    satellite_clock_flag& flag()       noexcept { return _flag; }
    satellite_clock_flag  flag() const noexcept { return _flag; }
    double&   c()       noexcept { return _c; }
    double    c() const noexcept { return _c; }
    double&  sc()       noexcept { return _sc; }
    double   sc() const noexcept { return _sc; }
    double&  vc()       noexcept { return _vc; }
    double   vc() const noexcept { return _vc; }
    double& svc()       noexcept { return _svc; }
    double  svc() const noexcept { return _svc; }

private:
    double _c,  _sc;
    double _vc, _svc;
    satellite_clock_flag _flag;
}; // satellite_clock

} // ngpt

#endif
