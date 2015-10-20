#include <iostream>
#include "grid.hpp"

int main()
{

  // Make a grid of float values, with range check, from -10.0 to 10.0 with
  // step 2.0
  ngpt::TickAxisImpl<float, true> gridA (-10.0f, 10.0f, 2.0f);

  //
  float pt = -1.4f;

  auto tpl = gridA.nearest_neighbor(pt);
  std::cout <<"\nCalling nearest_neighbor on a TickAxis from " << gridA.from()
    << " to " << gridA.to() << " with step " << gridA.step() << ".";
  std::cout << "\nThe nearest tick point to value " << pt
    << " is at index: " << std::get<0>(tpl) << " with value: " << std::get<1>(tpl);
  auto tpl2 = gridA.neighbor_nodes(pt);
  std::cout << "\nThe neighbor tick points to value " << pt << " are:"
    << "\n\tLeft : index: " << std::get<0>(tpl2) << " with value: " << std::get<1>(tpl2)
    << "\n\tRight: index: " << std::get<2>(tpl2) << " with value: " << std::get<3>(tpl2);
  
  pt = -14.5;
  try {
    tpl = gridA.nearest_neighbor(pt);
    std::cout <<"\nCalling nearest_neighbor on a TickAxis from " << gridA.from()
      << " to " << gridA.to() << " with step " << gridA.step() << ".";
    std::cout << "\nThe nearest tick point to value " << pt
      << " is at index: " << std::get<0>(tpl) << " with value: " << std::get<1>(tpl);
  } catch (std::out_of_range&) {
    std::cout << "\nEXCEPTION CAUGHT! Trying to compute nearest_neighbor fpr point " << pt;
  }
  try {
    tpl2 = gridA.neighbor_nodes(pt);
    std::cout << "\nThe neighbor tick points to value " << pt << " are:"
      << "\n\tLeft : index: " << std::get<0>(tpl2) << " with value: " << std::get<1>(tpl2)
      << "\n\tRight: index: " << std::get<2>(tpl2) << " with value: " << std::get<3>(tpl2);
  } catch (std::out_of_range&) {
    std::cout << "\nEXCEPTION CAUGHT! Trying to compute neighbor_nodes fpr point " << pt;
  }

  std::cout << "\n";
  return 0;
}
