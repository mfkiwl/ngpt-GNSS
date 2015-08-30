#include <iostream>
#include "antex.hpp"
#include "antenna.hpp"

using namespace ngpt;

int main ()
{
  Antex atx ("igs08.atx");
  //Antex atx ("I08.ATX");
  int status;
  
  // read the atx header.
  atx.read_header();
  
  // try to find a valid antenna in the atx file.
  Antenna ant ("TRM14177.00     NONE");
  status = atx.find_antenna(ant);
  if (status>0) {
    std::cout << "\nAntenna not found!";
  } else if (status < 0) {
    std::cout << "\nError while reading antex!";
  } else {
    std::cout << "\nAntenna found!";
  }
  
  // exit.
  std::cout <<"\nAll done!\n";
  return 0;
}