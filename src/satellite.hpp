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
        no_velocity   = 16
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
        : _x{x}, _y{y}, _z{z}, _sx{sx}, _sy{sy}, _sz{sz},
          _flag{f},
          _vx{vx}, _vy{vy}, _vz{vz}, _svx{svx}, _svy{svy}, _svz{svz}
    {}

private:
    double _x,   _y,   _z;
    double _sx,  _sy,  _sz;
    double _vx,  _vy,  _vz;
    double _svx, _svy, _svz;
    satellite_state_flag _flag;

}; // satellite_state

} // ngpt

#endif
