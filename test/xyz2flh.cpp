#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>

#include "car2ell.hpp"
#include "ell2car.hpp"
#include "car2top.hpp"

enum class units : char
{
    meters,
    radians,
    dec_degrees,
    hex_degress
};

class point {
public:
    explicit
    point() noexcept
    : x_(0),
      y_(0),
      z_(0),
      name_(""),
      unt_(units::meters),
      flag_('\0')
    {}

    int
    set_from_bern_crd( char* );

    int
    read_in_xyz( const char* );

    friend
    std::ostream&
    operator<<( std::ostream&, const point& );

private:
    double      x_, y_, z_;
    std::string name_;
    units       unt_;
    char        flag_;
};

int main( int argc, char* argv[] )
{
    char               line[256];
    std::vector<point> points;
    point              tmp;

    while ( std::cin.getline( line, 256 ) && *line )
    {
        tmp.read_in_xyz( line );
        std::cout << tmp << "\n";
        points.push_back( tmp );
    }

    for ( const auto& p : points )
    {
        std::cout << p << "\n";
    }

    return 0;
}

int
point::read_in_xyz( const char* xyz_line )
{
    unt_  = units::meters;
    flag_ = '\0';
    char *start, *end;

    x_ = std::strtod( xyz_line, &end );
    start = end;
    y_ = std::strtod( start, &end );
    start = end;
    z_ = std::strtod( start, &end );

    return 0;
}

int
point::set_from_bern_crd( char* crd_line )
{
    std::size_t len = std::strlen( crd_line );
    if ( len < 66 ) {
        return 1;
    }
    char* end;
    // [0-5)   is the number
    int aa ( std::atoi( crd_line ) );
    // [15-20] is the name
    name_.assign( crd_line+5, 15 );
    // [21-36) x
    x_ = std::strtod( crd_line+21, &end );
    // [21-36) x
    y_ = std::strtod( crd_line+36, &end );
    // [21-36) x
    z_ = std::strtod( crd_line+51, &end );
    while ( *end ) {
        if ( *end != ' ' ) {
            flag_ = *end;
            break;
        }
        ++end;
    }
    unt_ = units::meters;
    return 0;
}

std::ostream&
operator<<(std::ostream& os, const point& p)
{
    os << p.name_ << " " << p.x_ << " " << p.y_ << " " << p.z_ ;
    return os;
}
