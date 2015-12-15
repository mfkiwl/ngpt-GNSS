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

  // header is read during construction.
  antex atx (argv[1]);
  int status;
  
  // try to find a valid antenna in the atx file.
  antenna ant ("TRM14177.00     NONE");
  status = atx.find_antenna(ant);
  if (status>0) {
    std::cout << "\nAntenna not found!";
  } else if (status < 0) {
    std::cout << "\nError while reading antex!";
    return 1;
  } else {
    std::cout << "\nAntenna found!";
  }

  // get the antenna pcv pattern
  atx.get_antenna_pattern( ant );
  
  // exit.
  std::cout <<"\nAll done!\n";
  return 0;
}
