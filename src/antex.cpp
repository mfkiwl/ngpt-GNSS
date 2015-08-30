#include <cassert>
#include <stdlib.h>
#include <cmath>
#include <stdexcept>
#include <string.h>
#include "antex.hpp"

#ifdef DEBUG
  #include <iostream>
#endif

/// No header line can have more than 80 chars. However, there are cases when
/// they  exceed this limit, just a bit ...
constexpr int MAX_HEADER_CHARS { 85 };

/// Size of 'END OF HEADER' C-string.
constexpr std::size_t eoh_size { std::strlen("END OF HEADER") };

/// Max header lines.
constexpr int MAX_HEADER_LINES { 1000 };

/// Max grid line chars; i.e. the maximum number of chars any pcv line can
/// hold. Typical 'ZEN1 / ZEN2 / DZEN' = '0.0  90.0   5.0' and the format is
/// mF8.2 which is 19*8 = 152 + (~8 starting chars) = 160.
/// Let's make enough space for DZEN = 3 -> max chars = 31*8 + 10 = 258
///
/// \warning When using this constant always assert something like
///          8*(std::size_t)((zen2-zen1)/dzen) < MAX_GRID_CHARS-10
///
constexpr std::size_t MAX_GRID_CHARS { 258 };

/// \details Antex Constructor, using an antex filename. The constructor will
///          only initialize (set) the _filename attribute and also (try to)
///          open the input stream (i.e. _istream).
///
/// \todo    Is this a noexcept function?
///
ngpt::Antex::Antex(const char *filename)
: _filename{filename},
_istream{filename, std::ios_base::in}
{}

/// \details Read an Antex (instance) header. The format of the header should
///          closely follow the antex format specification (version 1.4).
///          This function will set the instance fields:
///          - _version (should be 1.4),
///          - _satsys,
///          - _type,
///          - _refant and
///          - __end_of_head
///          The function will exit after reading a header line, ending with
///          the (sub)string 'END OF HEADER'.
///
/// \throw   Will throw a std::runtime_error in case any one of the required 
///          instance fields can not be resolved, or the 'END OF HEADER' cannot
///          be found after MAX_HEADER_LINES are read.
///
/// \warning 
///          - The instance's input steam (i.e. _istream) should be open and 
///          valid.
///          - Note that the function expects that no header line contains more
///          than MAX_HEADER_CHARS chars.
///
/// \reference https://igscb.jpl.nasa.gov/igscb/station/general/antex14.txt
///
void ngpt::Antex::read_header()
{
  char line[MAX_HEADER_CHARS];

  // The stream should be open by now!
  assert(this->_istream.is_open());

  // Go to the top of the file.
  _istream.seekg(0);

  // Read the first line. Get version and system.
  // ----------------------------------------------------
  _istream.getline(line, MAX_HEADER_CHARS);
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
  _istream.getline(line, MAX_HEADER_CHARS);
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
  _istream.getline(line, MAX_HEADER_CHARS);
  while (dummy_it < MAX_HEADER_LINES && 
    strncmp(line+60, "END OF HEADER", eoh_size) ) {
    _istream.getline(line, MAX_HEADER_CHARS);
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

/// \details This is only a utility function. After it is handed an input
///          file stream (i.e. that of an Antex instance), positioned at the
///          begining of a 'METH / BY / # / DATE' field, it will continue
///          reading the antenna block untill the 'END OF ANTENNA' field.
///
/// \return  Everything other than 0 denots a failure.
///
/// \todo    Need to also read 'FREQ RMS'
bool __skip_rest_of_antenna__(std::ifstream& fin)
{
  static char line[MAX_HEADER_CHARS];
  static char grid_line[MAX_GRID_CHARS];

  // next field is 'METH / BY / # / DATE'
  if (!fin.getline(line, MAX_HEADER_CHARS) ||
    strncmp(line+60, "METH / BY / # / DATE", 20))
  {
    return 1;
  }

  // next field is 'DAZI'
  if (!fin.getline(line, MAX_HEADER_CHARS) ||
    strncmp(line+60, "DAZI", 4)) 
  {
    return 2;
  }
  // if DAZI > 0 we will need to read the azimouth-dependent grid.
  float dazi { std::stof(line+2, nullptr) };

  // next field is 'ZEN1 / ZEN2 / DZEN'
  if (!fin.getline(line, MAX_HEADER_CHARS) ||
    strncmp(line+60, "ZEN1 / ZEN2 / DZEN", 18))
  {
    return 3;
  }
  float zen1 { std::stof(line+2, nullptr) };
  float zen2 { std::stof(line+8, nullptr) };
  float dzen { std::stof(line+14, nullptr) };
  // see the decleration of MAX_GRID_CHARS for why this is needed.
  assert( 8*(std::size_t)((zen2-zen1)/dzen) < MAX_GRID_CHARS-10  );
  
  // next field is '# OF FREQUENCIES'
  if (!fin.getline(line, MAX_HEADER_CHARS) || 
    strncmp(line+60, "# OF FREQUENCIES", 16))
  {
    return 4;
  }
  int num_of_freqs { std::stoi(line, nullptr) };
  
  // loop for all frequencies ...
  while (num_of_freqs-- > 0) {
    
    // next required field is 'START OF FREQUENCY' but there can be several
    // other optional fields inbetween.
    fin.getline(line, MAX_HEADER_CHARS);
    while ( strncmp(line+60, "START OF FREQUENCY", 18) ) {
      if (!fin.getline(line, MAX_HEADER_CHARS)) return 5;
    }

    // next field is 'NORTH / EAST / UP'
    if (!fin.getline(line, MAX_HEADER_CHARS) || 
      strncmp(line+60, "NORTH / EAST / UP", 17)) 
    {
      return 6;
    }

    // reading the NOAZI values ...
    if (!fin.getline(grid_line, MAX_GRID_CHARS) ||
      strncmp(grid_line+3, "NOAZI", 5)) 
    {
      return 7;
    }

    // reading azimuth-dependent values ...
    float azi = .0;
    if (dazi > .0e0) {
      while (azi <= 360.0 && fin.getline(grid_line, MAX_GRID_CHARS)) {
        azi += dazi;
      }
    }
   
    // next field is 'END OF FREQUENCY'
    if (!fin.getline(line, MAX_HEADER_CHARS) ||
      strncmp(line+60, "END OF FREQUENCY", 16))
    {
      return 9;
    }

  } // end reading all frequencies.

  // all done! next field is 'END OF ANTENNA'
  if (!fin.getline(line, MAX_HEADER_CHARS) ||
    strncmp(line+60, "END OF ANTENNA", 14))
  {
    return 10;
  }

  return 0;
}

///
/// \return An integer denoting the exit status.
/// Integer | Status            
/// --------|----------------------------------------------
///      -1 | ANTEX format error; something went wrong while reading.
///       0 | Success; antenna found.
///       1 | Antenna could not be found.
///
int ngpt::Antex::find_antenna(const Antenna& antenna)
{
  char line[MAX_HEADER_CHARS];
  constexpr std::size_t soa_size { std::strlen("START OF ANTENNA") };
  std::size_t antennas_read { 0 };
  Antenna ant;
  int status;

  // The stream should be open by now!
  assert(this->_istream.is_open());

  // Go to the end of header.
  _istream.seekg(_end_of_head, std::ios_base::beg);

  // First line after end of header, should be the start of a new antenna.
  if (!_istream.getline(line, MAX_HEADER_CHARS) || 
    strncmp(line+60, "START OF ANTENNA", soa_size)) 
  {
    return -1;
  }
  // Read & check antenna type
  if (!_istream.getline(line, MAX_HEADER_CHARS) ||
    strncmp(line+60, "TYPE / SERIAL NO    ", 20))
  {
    return -1;
  }
  // Set the antenna type of the current antenna.
  ant.set_antenna(line);

  while (ant != antenna && _istream.good())
  {
#ifdef DEBUG
    std::cerr << "\n\t[DEBUG] INFO Skipping antenna: ["<< line << "]";
#endif
    // skip the antenna details ...
    if ( (status = __skip_rest_of_antenna__(_istream)) ) {
#ifdef DEBUG
    std::cerr << "\n\t[DEBUG] ERROR in __skip_rest_of_antenna__; status="<<status;
#endif
      return -1;
    }
    antennas_read++;

    // read new antenna ..
    if (!_istream.getline(line, MAX_HEADER_CHARS) ||
      strncmp(line+60, "START OF ANTENNA", soa_size))
    {
#ifdef DEBUG
      std::cerr << "\n[DEBUG] ERROR while reading antex file. 'START OF ANTENNA' expected";
#endif
      return -1;
    }
    // read & check antenna type
    if (!_istream.getline(line, MAX_HEADER_CHARS) ||
      strncmp(line+60, "TYPE / SERIAL NO", 16))
    {
#ifdef DEBUG
      std::cerr << "\n[DEBUG] ERROR while reading antex file. 'TYPE / SERIAL NO' expected";
#endif
      return -1;
    }
    // Set the antenna type.
    ant.set_antenna(line);
  }

  // .. we' ve stoped; either eof encountered ...
  if (_istream.eof()) {
    _istream.clear();
    return 1;
  }

  // .. or we found the antenna !!
  printf ("\nANTENNA FOUND AFTER READING %zu ANTENNAS",antennas_read);
  printf ("\n%s",line);
  return 0;
}