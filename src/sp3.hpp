#ifndef __SP3__HPP__
#define __SP3__HPP__

#include <fstream>
#include <vector>
#include "datetime_v2.hpp"
#include "satellite.hpp"

namespace ngpt
{

class sp3
{
/// Let's not write this more than once.
typedef std::ifstream::pos_type pos_type;
/// This is the datetime resolution for sp3
typedef ngpt::datev2<ngpt::milliseconds> datetime_ms;

public:
    /// Constructor from filename.
    explicit sp3(const char*);
    /// Destructor
    ~sp3() noexcept = default;
    /// Copy not allowed.
    sp3(const sp3&) = delete;
    /// Assignment not allowed.
    sp3& operator=(const sp3&) = delete;

private:
    /// Read sp3 header.
    int read_header();

    std::string     _filename;
    std::ifstream   _istream;
    pos_type        _end_of_head;
    datetime_ms     _first_epoch;
    datetime_ms     _last_epoch;
    int             _num_of_epochs;
    ngpt::satellite_system _satsys;
    char            _coordSys[6];
    char            _orbType[4];
    std::vector<ngpt::satellite> _sat_vec;
    std::vector<short int> _sat_acc;
    double _base_for_pos;
    double _base_for_clk;
     
}; // sp3

} // ngpt

#endif
