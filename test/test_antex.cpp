#include <iostream>
#include "antex.hpp"
#include "antenna.hpp"

using namespace ngpt;

int main(int argc, char* argv[])
{
  // must pass the atx file to inspect as cmd
  if ( argc != 2 ) {
    std::cout << "\nUsage: testAntex <atxfile>\n";
    return 1;
  }

  antex atx (argv[1]);
  //Antex atx ("I08.ATX");
  int status;
  
  // read the atx header.
  atx.read_header();
  
  // try to find a valid antenna in the atx file.
  antenna ant ("TRM14177.00     NONE");
  status = atx.__find_antenna(ant);
  if (status>0) {
    std::cout << "\nAntenna not found!";
  } else if (status < 0) {
    std::cout << "\nError while reading antex!";
  } else {
    std::cout << "\nAntenna found!";
  }

  // get the antenna pcv pattern
  atx.get_antenna_pattern( ant );
  
  // exit.
  std::cout <<"\nAll done!\n";
  return 0;
}
