#include <iostream>
#include "antex.hpp"
#include "antenna.hpp"

using namespace ngpt;

int main ()
{
  Antex atx ("igs08.atx");
  
  atx.read_header();
  
  std::cout <<"\nAll done!\n";
  return 0;
}