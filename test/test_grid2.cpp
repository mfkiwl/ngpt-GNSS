#include <random>
#include <chrono>
#include <iostream>
#include <utility>
#include "grid.hpp"

using ngpt::tick_axis;
using ngpt::grid_skeleton;

constexpr int N (3600/25+1);
constexpr int LOOPS_MAX (2000);
constexpr int TEST_PASS (0);
constexpr int TEST_FAIL (1);

int main()
{
    int status = TEST_PASS;
    std::cout<<"\nTesting grid_skeleton<> ## 2";
    std::cout<<"\n=============================================";

    // random number generator (normal distribution)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> dist_x(0.0, 60.0);
    std::normal_distribution<> dist_y(0.0, 30.0);
    
    // a tick axis of floats
    //tick_axis<float> t1 (-180.0, 180.0, 2.5);
    grid_skeleton<float, ngpt::grid_dimension::two_dim>
                t1(180.0, -180.0, -2.5 , -90.0, 90.0, 5.0);


    // let's see if we can walk through the skeleton correctly via
    // a node;
    std::size_t num_of_rows = 3600/25+1;
    std::cout<<"\n# rows "<<num_of_rows;
    std::size_t num_of_cols = 180/5+1;
    std::cout<<"\n# cols "<<num_of_cols;
    std::vector<double> vals;
    std::size_t row(0), col(0), iter(0);
    auto nd = t1.begin();
    for (; nd!=t1.end(); ++nd) {
        auto tpl = nd.position();
        vals.push_back(dist_x(gen));
        // std::cout<<"\npos: "<<std::get<0>(tpl)<<", "<<std::get<1>(tpl);
        if ( std::get<0>(tpl) != row || std::get<1>(tpl) != col ) {
            std::cerr<<"\n["<<iter<<"] Failed running through the grid!";
            std::cerr<<"\nComputed node ("<<std::get<0>(tpl)<<", "<<std::get<1>(tpl)<<")"
                     <<" should be ("<<row<<", "<<col<<")";
            status = TEST_FAIL;
        }
        ++col;
        if (col == num_of_cols) {
            col = 0;
            ++row;
        }
        ++iter;
    }

    std::vector<std::pair<float,float>> pts;
    pts.emplace_back(180.0, -90.0);
    pts.emplace_back(180.0, 90.0);
    pts.emplace_back(-180.0, -90.0);
    pts.emplace_back(-180.0, 90.0);
    
    // see what happens at the limits.
    for (auto i : pts) {
        try {
            t1.bilinear_interpolation_impl(i.first, i.second, vals.data());
        } catch (std::domain_error& e) {
            std::cout<<"\nFailed!";
            std::cout<<"\n"<<e.what();
        }
    }

    // check the speed between a grid of floats and a grid of ints.
    std::vector<std::pair<float, float>> test_pts;
    std::vector<float> res1, res2;
    for (int i=0;i<LOOPS_MAX;++i) {
        test_pts.emplace_back(dist_x(gen), dist_y(gen));
    }
    
    grid_skeleton<float, ngpt::grid_dimension::two_dim>
                t1f(180.0, -180.0, -2.5 , -90.0, 90.0, 5.0);
    // let's say we are only interested in presision 10e-2
    int factor = 10000;
    grid_skeleton<int, ngpt::grid_dimension::two_dim>
                t1i(180*factor, -180*factor, -2.5*factor , -90*factor, 90*factor, 5*factor);

    std::size_t failures = 0;
    auto start = std::chrono::steady_clock::now();
    for (int i=0;i<LOOPS_MAX;++i) {
        try {
            auto val = t1f.bilinear_interpolation_impl(
                           test_pts[i].first, test_pts[i].second, vals.data());
            res1.push_back(val);
        } catch (std::domain_error&) {
            res1.push_back(-999.0e0);
            ++failures;
        }
    }
    auto stop = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(stop-start);
    std::cout << "\nFloat Version: " << elapsed.count(); 
    
    start = std::chrono::steady_clock::now();
    for (int i=0;i<LOOPS_MAX;++i) {
        try {
            auto val = t1i.bilinear_interpolation_impl(
                           test_pts[i].first*factor, test_pts[i].second*factor, vals.data());
            res2.push_back(val);
        } catch (std::domain_error&) {
            res2.push_back(-999.0e0);
            ++failures;
        }
    }
    stop = std::chrono::steady_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::microseconds>(stop-start);
    std::cout << "\nInt Version  : " << elapsed.count(); 

    std::cout<<"\nExceptions: "<<failures<<"/"<<LOOPS_MAX;

    // confirm that the results from int/float versions are the same
    float accuracy = 1e-2;
    for (int i=0;i<LOOPS_MAX;++i) {
        if (std::abs(res1[i]-res2[i]) > accuracy) {
            std::cout<<"\nIncompatible results!";
            std::cout<<" point ("<<test_pts[i].first<<", "<<test_pts[i].second<<")"
                     <<" diff = "<<std::abs(res1[i]-res2[i])<<" ("<<res1[i]<<" and "<<res2[i]<<")";
            status = TEST_FAIL;
        }
    }

    std::cout<<std::endl;
    return status;
}
