#ifndef __GNSS_SATELLITES_HPP
#define __GNSS_SATELLITES_HPP

#include <type_traits>
#include "satsys.hpp"
#include "genflags.hpp"

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

/// A satellite state vector can be:
enum class satellite_state_option_flag
    : int
{
    bad_or_absent = 1,
    unknown_acc   = 2,
    maneuver      = 4,
    prediction    = 8,
    no_velocity   = 16,
    no_vel_acc    = 32
};

class satellite_state
{
public:
    typedef ngpt::satellite_state_option_flag flag_option;
    typedef ngpt::flag<flag_option>           state_flag;

public:
    explicit satellite_state(double x=.0f, double y=.0f, double z=.0f,
              double sx=.0f, double sy=.0f, double sz=.0f,
              /*flag_option f=flag_option::bad_or_absent,*/
              state_flag f=state_flag(),
              double vx=.0f, double vy=.0f, double vz=.0f,
              double svx=.0f, double svy=.0f, double svz=.0f)
    noexcept
        : _x{x}, _y{y}, _z{z}, _sx{sx}, _sy{sy}, _sz{sz},
          _vx{vx}, _vy{vy}, _vz{vz}, _svx{svx}, _svy{svy}, _svz{svz},
          _flag{f}
    {}

    state_flag& flag()       noexcept { return _flag; }
    state_flag  flag() const noexcept { return _flag; }
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
    state_flag _flag;

}; // satellite_state

/// A satellite clock state vector can be:
enum class satellite_clock_option_flag
    : int
{
    bad_or_absent = 1,
    unknown_acc   = 2,
    discontinuity = 4,
    prediction    = 8,
    no_velocity   = 16,
    no_vel_acc    = 32
};

/// A class to hold a satellite's clock correction (and drift).
class satellite_clock
{
public:
    typedef ngpt::satellite_clock_option_flag flag_option;
    typedef ngpt::flag<flag_option>           clock_flag;

public:
    explicit satellite_clock(double c=.0f, double sc=.0f,
                /*flag_option f=satellite_clock_option_flag::bad_or_absent,*/
                clock_flag f=clock_flag(),
                double vc=.0f, double svc=.0f)
    noexcept
        : _c{c}, _sc{sc}, _vc{vc}, _svc{svc}, _flag{f}
    {}
    
    clock_flag& flag()       noexcept { return _flag; }
    clock_flag  flag() const noexcept { return _flag; }
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
    clock_flag _flag;
}; // satellite_clock

} // ngpt

#endif
