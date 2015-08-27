#include <cassert>
#include <stdlib.h>
#include <cmath>
#include <stdexcept>
#include <string.h>
#include "antex.hpp"

/// No header line can have more than 80 chars.
constexpr int MAX_HEADER_CHARS { 81 };

/// Size of 'END OF HEADER' C-string.
constexpr std::size_t eoh_size { std::strlen("END OF HEADER") };

/// Max header lines.
constexpr int MAX_HEADER_LINES { 1000 };

ngpt::Antex::Antex(const char *filename)
: _filename{filename},
_istream{filename, std::ios_base::in}
{}

void ngpt::Antex::read_header()
{
  char line[MAX_HEADER_CHARS];

  // The stream should be open by now!
  assert(this->_istream.is_open());

  // Go to the top of the file.
  _istream.seekg(0);

  // Read the first line. Get version and system.
  // ----------------------------------------------------
  _istream.getline(line, 81);
  // strtod will keep on reading untill a non-valid
  // char is read. Fuck this, lets split the string
  // so that it only reads the valid chars (for version).
  *(line+15) = '\0';
  float fvers = std::strtod(line,nullptr);
  if (std::abs(fvers - 1.4) > .001 ) {
    throw std::runtime_error
    ("ngpt::Antex::read_header -> Invalid Antex version.");
  } else {
    this->_version = ngpt::Antex::ATX_VERSION::v14;
  }
  // Resolve the satellite system.
  try {
    this->_satsys = ngpt::charToSatSys(line[20]);
  } catch (std::runtime_error& e) {
    throw std::runtime_error
    ("ngpt::Antex::read_header -> Invalid Satellite System.");
  }

  // Read the second line. Get PCV TYPE / REFANT.
  // ----------------------------------------------------
  _istream.getline(line, 81);
  if (*line == 'A') {
    this->_type = ngpt::Antex::PCV_TYPE::Absolute;
  } else if (*line == 'R') {
    this->_type = ngpt::Antex::PCV_TYPE::Relative;
  } else {
    throw std::runtime_error
    ("ngpt::Antex::read_header -> Invalid PCV type.");
  }
  // In case of a relative antenna, we also have the
  // REFANT field.
  if (this->_type == ngpt::Antex::PCV_TYPE::Relative) {
    this->_refant.set_antenna(line+20);
  }

  // Keep on readling lines until 'END OF HEADER'.
  // ----------------------------------------------------
  int dummy_it = 0;
  _istream.getline(line, 81);
  while (dummy_it < MAX_HEADER_LINES && 
    strncmp(line+60, "END OF HEADER", eoh_size) ) {
    _istream.getline(line, 81);
    dummy_it++;
  }
  if (dummy_it >= MAX_HEADER_LINES) {
    throw std::runtime_error
    ("ngpt::Antex::read_header -> Could not find 'END OF HEADER'.");
  }

  this->_end_of_head = _istream.tellg();

  // All done !
  return;
}
