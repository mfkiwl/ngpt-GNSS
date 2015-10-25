#include <iostream>
#include "grid.hpp"

using std::cout;

int main()
{

  cout << "\nTESTING THE CLASS: ngpt::TickAxisImpl<>";
  cout << "\n==================================================================";

  // Make a grid of float values, with range check, from -10.0 to 10.0 with
  // step 2.0
  ngpt::TickAxisImpl<float, true> gridA (-10.0f, 10.0f, 2.0f);

  //
  float pt = -1.4f;

  auto tpl = gridA.nearest_neighbor(pt);
  cout <<"\nCalling nearest_neighbor on a TickAxis from " << gridA.from()
    << " to " << gridA.to() << " with step " << gridA.step() << ".";
  cout << "\nThe nearest tick point to value " << pt
    << " is at index: " << std::get<0>(tpl) << " with value: " << std::get<1>(tpl);
  auto tpl2 = gridA.neighbor_nodes(pt);
  cout << "\nThe neighbor tick points to value " << pt << " are:"
    << "\n\tLeft : index: " << std::get<0>(tpl2) << " with value: " << std::get<1>(tpl2)
    << "\n\tRight: index: " << std::get<2>(tpl2) << " with value: " << std::get<3>(tpl2);
  
  pt = -14.5;
  try {
    tpl = gridA.nearest_neighbor(pt);
    cout <<"\nCalling nearest_neighbor on a TickAxis from " << gridA.from()
      << " to " << gridA.to() << " with step " << gridA.step() << ".";
    cout << "\nThe nearest tick point to value " << pt
      << " is at index: " << std::get<0>(tpl) << " with value: " << std::get<1>(tpl);
  } catch (std::out_of_range&) {
    cout << "\nEXCEPTION CAUGHT! Trying to compute nearest_neighbor fpr point " << pt;
  }
  try {
    tpl2 = gridA.neighbor_nodes(pt);
    cout << "\nThe neighbor tick points to value " << pt << " are:"
      << "\n\tLeft : index: " << std::get<0>(tpl2) << " with value: " << std::get<1>(tpl2)
      << "\n\tRight: index: " << std::get<2>(tpl2) << " with value: " << std::get<3>(tpl2);
  } catch (std::out_of_range&) {
    cout << "\nEXCEPTION CAUGHT! Trying to compute neighbor_nodes fpr point " << pt;
  }
  
  // This is (pretty much) the same grid, though in descending order.
  // Let's try the same
  ngpt::TickAxisImpl<float, true> gridB (10.0f,-10.0f, -2.0f);
  pt = -1.4f;
  tpl = gridB.nearest_neighbor(pt);
  cout <<"\nCalling nearest_neighbor on a TickAxis from " << gridB.from()
    << " to " << gridB.to() << " with step " << gridB.step() << ".";
  cout << "\nThe nearest tick point to value " << pt
    << " is at index: " << std::get<0>(tpl) << " with value: " << std::get<1>(tpl);
  tpl2 = gridB.neighbor_nodes(pt);
  cout << "\nThe neighbor tick points to value " << pt << " are:"
    << "\n\tLeft : index: " << std::get<0>(tpl2) << " with value: " << std::get<1>(tpl2)
    << "\n\tRight: index: " << std::get<2>(tpl2) << " with value: " << std::get<3>(tpl2);
  
  pt = -14.5;
  try {
    tpl = gridB.nearest_neighbor(pt);
    cout <<"\nCalling nearest_neighbor on a TickAxis from " << gridB.from()
      << " to " << gridB.to() << " with step " << gridB.step() << ".";
    cout << "\nThe nearest tick point to value " << pt
      << " is at index: " << std::get<0>(tpl) << " with value: " << std::get<1>(tpl);
  } catch (std::out_of_range&) {
    cout << "\nEXCEPTION CAUGHT! Trying to compute nearest_neighbor fpr point " << pt;
  }
  try {
    tpl2 = gridB.neighbor_nodes(pt);
    cout << "\nThe neighbor tick points to value " << pt << " are:"
      << "\n\tLeft : index: " << std::get<0>(tpl2) << " with value: " << std::get<1>(tpl2)
      << "\n\tRight: index: " << std::get<2>(tpl2) << " with value: " << std::get<3>(tpl2);
  } catch (std::out_of_range&) {
    cout << "\nEXCEPTION CAUGHT! Trying to compute neighbor_nodes fpr point " << pt;
  }
  
  cout << "\nTESTING THE CLASS: ngpt::GridSkeleton<>";
  cout << "\n==================================================================";

  // Make a 2-D Grid skeleton
  float azi1 (0), azi2(360), dazi(5);
  float ele1 (0), ele2(90),  dele(2.5);
  ngpt::GridSkeleton<float, true, ngpt::Grid_Dimension::TwoDim>
    d2grd (ele1, ele2, dele, azi1, azi2, dazi);

  float ptx (21.34), pty (254.12);
  auto smth = d2grd.nearest_neighbor(ptx, pty);
  cout <<"\nCalling nearest_neighbor on a 2-D Grid (Skeleton) ";
  cout << "\nThe neighbor tick point to point " << ptx << ", "<< pty << " is:"
    << "\n\tXaxis-> index: " << std::get<0>(smth) << " with value: " << std::get<1>(smth)
    << "\n\tYaxis-> index: " << std::get<2>(smth) << " with value: " << std::get<3>(smth)
    << "\n";

  // Same GridSkeleton as above, but with y-axis inverted.
  ngpt::GridSkeleton<float, true, ngpt::Grid_Dimension::TwoDim>
    d2grd_1 (ele1, ele2, dele, azi2, azi1, -1*dazi);

  smth = d2grd_1.nearest_neighbor(ptx, pty);
  cout <<"\nCalling nearest_neighbor on a 2-D Grid (Skeleton) ";
  cout << "\nThe neighbor tick point to point " << ptx << ", "<< pty << " is:"
    << "\n\tXaxis-> index: " << std::get<0>(smth) << " with value: " << std::get<1>(smth)
    << "\n\tYaxis-> index: " << std::get<2>(smth) << " with value: " << std::get<3>(smth)
    << "\n";

  cout << "\n";
  return 0;
}
