#include <iostream>
#include "grid.hpp"

using namespace ngpt;

int main()
{
  std::size_t idx;
  Axis<float> grid_no_check {-5, 5, 2.5};
  Axis<float,Grid_Range_Check::Yes> grid_wt_check {5, -5, -2.5};

  std::cout << "\nGrid-1 : axis value at index 2 = " << grid_no_check.grid_value_at(2);
  std::cout << "\nGrid-2 : axis value at index 2 = " << grid_wt_check.grid_value_at(2);

  std::cout << "\nGrid-1 point at index 100 = " << grid_no_check.grid_value_at(100) << " (INVALID!! but no range check)";
  
  // The following should case the program to fail!
  //std::cout << "\nGrid-1 point at index 100 = " << grid_wt_check.grid_value_at(100) << " (INVALID!! but no range check)";
  
  std::cout << "\nGrid-1 : Nearest point at -6.7 = " << grid_no_check.nearest_grid_value_impl(-6.7, idx);
  std::cout << "\nGrid-1 : Nearest point at -5.7 = " << grid_no_check.nearest_grid_value_impl(-5.7, idx);
  std::cout << "\nGrid-1 : Nearest point at -2.7 = " << grid_no_check.nearest_grid_value_impl(-2.7, idx);
  std::cout << "\nGrid-1 : Nearest point at -0.7 = " << grid_no_check.nearest_grid_value_impl(-0.7, idx);
  std::cout << "\nGrid-1 : Nearest point at  0.7 = " << grid_no_check.nearest_grid_value_impl(0.7, idx);
  std::cout << "\nGrid-1 : Nearest point at  0.8 = " << grid_no_check.nearest_grid_value_impl(0.8, idx);
  std::cout << "\nGrid-1 : Nearest point at  1.7 = " << grid_no_check.nearest_grid_value_impl(1.7, idx);
  std::cout << "\nGrid-1 : Nearest point at  5.7 = " << grid_no_check.nearest_grid_value_impl(5.7, idx);

  std::cout << "\nGrid-2 : Nearest point at -6.7 = " << grid_wt_check.nearest_grid_value_impl(-6.7, idx);
  std::cout << "\nGrid-2 : Nearest point at -5.7 = " << grid_wt_check.nearest_grid_value_impl(-5.7, idx);
  std::cout << "\nGrid-2 : Nearest point at -2.7 = " << grid_wt_check.nearest_grid_value_impl(-2.7, idx);
  std::cout << "\nGrid-2 : Nearest point at -0.7 = " << grid_wt_check.nearest_grid_value_impl(-0.7, idx);
  std::cout << "\nGrid-2 : Nearest point at  0.7 = " << grid_wt_check.nearest_grid_value_impl(0.7, idx);
  std::cout << "\nGrid-2 : Nearest point at  0.8 = " << grid_wt_check.nearest_grid_value_impl(0.8, idx);
  std::cout << "\nGrid-2 : Nearest point at  1.7 = " << grid_wt_check.nearest_grid_value_impl(1.7, idx);
  std::cout << "\nGrid-2 : Nearest point at  5.7 = " << grid_wt_check.nearest_grid_value_impl(5.7, idx);
  
  std::cout << "\n";
  return 0;
}
