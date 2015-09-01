#include <iostream>
#include <type_traits>

// Generic struct to perform (or not) range checks.
// Will be later specialized to treat C=true and C=false.
template<typename T, bool C>
struct Range_Checker {};

// Specialized version; DO perform range check.
template<typename T>
struct Range_Checker<T, true>
{
  enum { can_throw = 1 };
  static bool validate(T max_val, T val)
  {
    if (val < 0 || val > max_val) {
      throw 1;
    }
    return true;
  }
};

// Specialized version; DO NOT perform range check.
template<typename T>
struct Range_Checker<T, false>
{
  enum { can_throw = 0 };
  static constexpr bool validate(T max_val, T val) noexcept
  {
    return true;
  }
};

// Dummy class could represent a -ne-dimensional grid
template<typename T, bool RC>
class Dummy
{
private:
  T val, max;
  
public:
  constexpr explicit Dummy(T x, T max_x) noexcept
  : val{x}, max{max_x}
  {};
  
  T add(T x) const noexcept( !Range_Checker<T,RC>::can_throw )
  {
    T ret_val = x + val;
    Range_Checker<T,RC>::validate(max, ret_val);
    return ret_val;
  }

};

int main()
{
  Dummy<float,false> d(0, 1000);

  std::cout <<"\nIndex of  156.7 is " << d.add(156.7);
  std::cout <<"\nIndex of 3156.7 is " << d.add(3156.7);

  std::cout <<"\n";
  return 0;
}