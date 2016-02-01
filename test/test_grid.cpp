#include <iostream>
#include <vector>
#include "grid.hpp"

using std::cout;
template<class T> void ignore( const T& ) { }

int main()
{
    ngpt::grid_skeleton<float, true, ngpt::Grid_Dimension::TwoDim>
        d2grid (0.0, 90.0, 1, 0.0, 360.0, 5.0);
    auto n1 = d2grid.nearest_neighbor( 32.78, 302.99 );
    ignore(n1);
    std::cout<<"\n------------------------1";
    
    auto idx = n1.node_index();
    std::cout<<"\n------------------------2";
    std::cout<<"\nIndexes: "<< std::get<0>(idx)<<", "<<std::get<1>(idx);
    std::cout<<"\n------------------------3";
    auto v1 = n1.value();
    std::cout<<"\n------------------------4";
    std::cout<<"\nValues:  "<< std::get<0>(v1) <<", "<<std::get<1>(v1);
    std::cout<<"\n//---------------------------------------------------//";

    auto nn1 = d2grid.neighbor_nodes( 32.78, 302.99 );
    auto upleft = std::get<3>(nn1);
    std::cout<<"    ("<<std::get<0>(upleft.node_index()) << "," << std::get<1>(upleft.node_index())<<")";
    
    upleft = std::get<2>(nn1);
    std::cout<<"\n"<<std::get<0>(upleft.node_index()) << "," << std::get<1>(upleft.node_index())<<")";
    
    upleft = std::get<0>(nn1);
    std::cout<<"\n"<<std::get<0>(upleft.node_index()) << "," << std::get<1>(upleft.node_index())<<")";
    
    upleft = std::get<1>(nn1);
    std::cout<<"    ("<<std::get<0>(upleft.node_index()) << "," << std::get<1>(upleft.node_index())<<")";
    std::cout<<"\n";

/*
  cout << "\n==================================================================";
  cout << "\nTESTING THE CLASS: ngpt::tick_axis_impl<>";
  cout << "\n==================================================================";

  cout << "\n>>ngpt::tick_axis_impl<> in ascending order";
  // Make a grid of float values, with range check, from -10.0 to 10.0 with
  // step 2.5
  ngpt::tick_axis_impl<float, true> gridA (-10.0f, 10.0f, 2.5f);

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
  
  cout << "\n\n>>ngpt::tick_axis_impl<> in descending order";
  // This is (pretty much) the same grid, though in descending order.
  // Let's try the same
  ngpt::tick_axis_impl<float, true> gridB (10.0f,-10.0f, -2.5f);
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
  
  cout << "\n\n==================================================================";
  cout << "\nTESTING THE CLASS: ngpt::grid_skeleton<>";
  cout << "\n==================================================================";

  // Make a 2-D Grid skeleton
  float azi1 (0), azi2(360), dazi(5);
  float ele1 (0), ele2(90),  dele(2.5);
  
  cout << "\n>>ngpt::grid_skeleton<> in ascending order";
  ngpt::grid_skeleton<float, true, ngpt::Grid_Dimension::TwoDim>
    d2grd (ele1, ele2, dele, azi1, azi2, dazi);
  std::cout <<"\nXaxis (i.e. elevation): "<<d2grd.x_axis_from()<<"/"<<d2grd.x_axis_to()<<"/"<<d2grd.x_axis_step();
  std::cout <<"\nYaxis (i.e. azimouth ): "<<d2grd.y_axis_from()<<"/"<<d2grd.y_axis_to()<<"/"<<d2grd.y_axis_step();
  
  float ptx (21.34), pty (254.12);
  auto smth = d2grd.nearest_neighbor(ptx, pty);
  cout <<"\nCalling nearest_neighbor on a 2-D Grid (Skeleton) ";
  cout << "\nThe neighbor tick point to point " << ptx << ", "<< pty << " is:"
    << "\n\tXaxis-> index: " << std::get<0>(smth) << " with value: " << std::get<1>(smth)
    << "\n\tYaxis-> index: " << std::get<2>(smth) << " with value: " << std::get<3>(smth)
    << "\n";

  // Same grid_skeleton as above, but with y-axis inverted.
  cout << "\n\n>>ngpt::grid_skeleton<> in descending order";
  ngpt::grid_skeleton<float, true, ngpt::Grid_Dimension::TwoDim>
    d2grd_1 (ele1, ele2, dele, azi2, azi1, -1*dazi);

  smth = d2grd_1.nearest_neighbor(ptx, pty);
  cout <<"\nCalling nearest_neighbor on a 2-D Grid (Skeleton) ";
  cout << "\nThe neighbor tick point to point " << ptx << ", "<< pty << " is:"
    << "\n\tXaxis-> index: " << std::get<0>(smth) << " with value: " << std::get<1>(smth)
    << "\n\tYaxis-> index: " << std::get<2>(smth) << " with value: " << std::get<3>(smth)
    << "\n";
  
  cout << "\n";
  cout << "\n\n==================================================================";
  cout << "\nTESTING THE CLASS: ngpt::grid_skeleton<>";
  cout << "\n==================================================================";
  
  cout << "\n>>ngpt::grid_skeleton<> in ascending order";
  ngpt::grid_skeleton<float, true, ngpt::Grid_Dimension::TwoDim> d2grd2 (0,-4,-1,0,5,1);
  std::cout <<"\nXaxis : "<<d2grd2.x_axis_from()<<"/"<<d2grd2.x_axis_to()<<"/"<<d2grd2.x_axis_step();
  std::cout <<"\nYaxis : "<<d2grd2.y_axis_from()<<"/"<<d2grd2.y_axis_to()<<"/"<<d2grd2.y_axis_step();
  std::vector<int> ivec (d2grd2.size(), 0);
  for (std::size_t i=0; i<ivec.size(); ++i) ivec[i] = i;

  auto cell = d2grd2.neighbor_nodes(-2.2, 3.8);
    // resolve the tuple
    std::size_t x0_idx = std::get<0>( cell );
    std::size_t y0_idx = std::get<4>( cell );
#ifdef DEBUG
    std::size_t x1_idx = std::get<2>( cell );
    std::size_t y1_idx = std::get<6>( cell );
    assert( x1_idx == x0_idx + 1 );
    assert( y1_idx == y0_idx + 1 );
#else
    std::size_t x1_idx = x0_idx + 1;
    std::size_t y1_idx = y0_idx + 1;
#endif

    float x0 = std::get<1>(cell);
    float x1 = std::get<3>(cell);
    float y0 = std::get<5>(cell);
    float y1 = std::get<7>(cell);
    // Normally we don't have to do this! see the assert stetements!
    std::size_t lowleft= y0_idx*d2grd2.x_axis_pts()+x0_idx;
    int f00 = ivec[lowleft];
    int f01 = ivec[lowleft+d2grd2.x_axis_pts()];
    int f10 = ivec[lowleft+1];
    int f11 = ivec[lowleft+d2grd2.x_axis_pts()+1];
    assert( lowleft == d2grd2.node_to_index(x0_idx, y0_idx) );
    assert( lowleft+d2grd2.x_axis_pts() == d2grd2.node_to_index(x0_idx, y1_idx) );
    assert( lowleft+1 == d2grd2.node_to_index(x1_idx, y0_idx) );
    assert( lowleft+d2grd2.x_axis_pts()+1 == d2grd2.node_to_index(x1_idx, y1_idx) );

std::cout <<"\nInterpolating at: ("<<2.2<<", "<<3.8<<")";
std::cout <<"\n("<<x0_idx<<", "<<y1_idx<<") ----   ("<<x1_idx<<", "<<y1_idx<<")";
std::cout <<"\n  |                  |";
std::cout <<"\n  |                  |";
std::cout <<"\n  |                  |";
std::cout <<"\n("<<x0_idx<<", "<<y0_idx<<") ----   ("<<x1_idx<<", "<<y0_idx<<")";
std::cout<<"\n";
std::cout <<"\nInterpolating at: ("<<2.2<<", "<<3.8<<")";
std::cout <<"\n("<<x0<<", "<<y1<<")="<<f01<<" ----   ("<<x1<<", "<<y1<<")="<<f11;
std::cout <<"\n  |                  |";
std::cout <<"\n  |                  |";
std::cout <<"\n  |                  |";
std::cout <<"\n("<<x0<<", "<<y0<<")="<<f00<<" ----   ("<<x1<<", "<<y0<<")="<<f10;
std::cout<<"\n";
 */ 
  return 0;
}
