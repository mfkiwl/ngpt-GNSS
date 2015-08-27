#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>
#include <iterator>

#ifdef __clang__
  #pragma message "[COMPILER] : Non-GCC compiler; droping 'noexcept' specification"
#elif __GNUG__
  #pragma message "[COMPILER] : Using 'noexcept' specification in GCC for " __FILE__
#else
  #pragma message "[COMPILER] : No valid compiler found! " __FILE__
#endif

int foo(std::pair<int,double>* p, int sz)
{
  for (int i=0; i<sz; i++) 
  {
    if ( p[i].first == 5 )
    {
      return i;
    }
  }
  return -1;
}

class A {
public:
    int _a;
    A(int i):_a(i){};
};

typedef std::pair<double, A> CoefObsPair;
typedef std::vector<CoefObsPair> COVec;

class B {
public:
    COVec _cov;
    B(double d, A a):_cov{ {d,a} } {};
    B(double d, int a):_cov{ {d,A(a)} } {};
};

int main()
{
  std::pair<int,double> vec[] =
  {
    {1, 1},
    {2, 2},
    {3, 3},
    {4, 4},
    {5, 5}
  };
  
  COVec _cov = { {1, A(1)} };
  B b (1.0, 1);
  b._cov.emplace_back(1.0, 1);

  std::cout <<"\nPair second = " << foo(vec,5) << "\n";

  COVec vv = {
    {1, A(1)},
    {2, A(2)},
    {3, A(3)},
    {4, A(4)},
    {5, A(5)}
  };

  auto it = std::find_if(vv.begin(), vv.end(), 
  [] (const CoefObsPair& a) ->bool {return a.second._a == 9;});
  if (it == std::end(vv))
    std::cout <<"\nArgument not found!\n";
  else
    std::cout <<"\nArgument found\n";

  return 0;
}
