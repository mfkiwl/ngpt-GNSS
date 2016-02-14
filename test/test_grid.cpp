#include <random>
#include <chrono>
#include <iostream>
#include "grid.hpp"

using ngpt::tick_axis;
using ngpt::grid_skeleton;

constexpr int N (3600/25+1);
constexpr int LOOPS_MAX (100000);
constexpr int TEST_PASS (0);
constexpr int TEST_FAIL (1);

int main()
{
    int status = TEST_PASS;
    std::cout<<"\nTesting grid_skeleton<> ## 1";
    std::cout<<"\n=============================================";

    // random number generator (normal distribution)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> dist(0.0, 60.0);
    
    // a tick axis of floats
    //tick_axis<float> t1 (-180.0, 180.0, 2.5);
    grid_skeleton<float, ngpt::grid_dimension::one_dim> t1(180.0, -180.0, -2.5);
    
    // array to hold dummy x- and y-values. x-values are values to interpolate
    // at, whereas y-values are the y-values of the nodes of t1.
    assert( N == t1.size() );
    double yvals[N], xvals[LOOPS_MAX];
    for (int i=0; i<N; ++i) yvals[i] = dist(gen);
    for (int i=0; i<LOOPS_MAX; ++i) xvals[i] = dist(gen);
    // note that y-values could also be assigned as:
    auto nd = t1.begin();
    for (; nd != t1.end(); ++nd) yvals[nd.index()] = dist(gen);

    // hold results of the interpolation for the two different methods
    // (i.e. bounded and unbounded).
    double res1[LOOPS_MAX], res2[LOOPS_MAX];

    // interpolate with the bounded version, A LOT !!
    // --------------------------------------------------------------------- //
    std::size_t failures = 0;
    auto start = std::chrono::steady_clock::now();
    for (int i=0; i<LOOPS_MAX; ++i) {
        try {
            res1[i] = t1.interpolate<double, true>( xvals[i], yvals );
        } catch (std::domain_error&) {
            res1[i] = -999;
            ++failures;
        }
    }
    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end-start);
    std::cout << "\nBounded Version:   " << elapsed.count() << " ms (# of throws: " 
              << failures << "/" << LOOPS_MAX << ")";
    // --------------------------------------------------------------------- //
    
    // interpolate with the un-bounded version, A LOT !!
    // --------------------------------------------------------------------- //
    start = std::chrono::steady_clock::now();
    for (int i=0; i<LOOPS_MAX; ++i) {
        res2[i] = t1.interpolate<double, false>( xvals[i], yvals );
    }
    end = std::chrono::steady_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end-start);
    std::cout << "\nUnbounded Version: " << elapsed.count() << " ms";
    // --------------------------------------------------------------------- //

    // The results should be identical, except where the bounded version has
    // thrown; these last are not compared.
    for (int i=0; i<LOOPS_MAX; ++i) {
        if (res1[i] != -999) {
            if ( std::abs(res1[i]-res2[i]) > 1e-10 ) {
                std::cout<<"\nInconsistent results! "<<res1[i] << "-" << res2[i] << "=" << std::abs(res1[i]-res2[i])
                         << " at " << xvals[i] << "=" << yvals[t1.nearest_neighbor(xvals[i]).index()];
                status = TEST_FAIL;
            }
        }
    }

    // report comparisson results
    if ( !status ) { 
        std::cout<<"\nStatus -> test passed!";
    } else {
        std::cout<<"\nStatus -> test failed!";
    }

    // exit with the test status
    std::cout << std::endl;
    return status;
}
