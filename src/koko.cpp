#include <iostream>

//  ---------------------- SOLUTION 1 ------------------------------  //
/*
template<typename T>
class DummyImpl
{
private:
  T mval, mmax;

public:
  constexpr explicit DummyImpl(T x, T max_x) noexcept
 : mval{x}, mmax{max_x}
  {};
  virtual ~DummyImpl() {};

  void bar() const { std::cout << "\nin Base."; }
  T max() const noexcept {return mmax;}
  T val() const noexcept {return mval;}
};

template<typename T, bool B>
class Dummy : DummyImpl<T> {};

template<typename T>
class Dummy<T, true> : DummyImpl<T>
{
public:
  explicit Dummy(T x, T max_x) noexcept : DummyImpl<T>(x, max_x) {};

  T add(T x) const noexcept( !true )
  {
    T ret_val = x + DummyImpl<T>::val();
    if (ret_val < 0 || ret_val > DummyImpl<T>::max()) {
      throw 1;
    }
    return ret_val;
  }
};

template<typename T>
class Dummy<T, false> : DummyImpl<T>
{
public:
  explicit Dummy(T x, T max_x) noexcept : DummyImpl<T>(x, max_x) {};

  T add(T x) const noexcept( !false )
  {
    return x + DummyImpl<T>::val();
  }
};
*/
//  ---------------------- SOLUTION 2 ------------------------------ //
/* DOES NOT WORK !!
template<typename T,  bool RC>
class Dummy
{
private:
  T mval, mmax;

  template<bool I, typename S> struct add_impl {};

  template<typename S> struct add_impl<true, S>
  {
    T operator()(T x) const noexcept( !true )
    {
      T ret_val = x + mval;
      if (ret_val < 0 || ret_val > mmax) {throw 1;}
      return ret_val;
    }
  };

  template<typename S> struct add_impl<false,  S>
  {
    T operator()(T x) const noexcept( !false )
    {
      return x + mval_ref;
    }
  };

public:
  constexpr explicit Dummy(T x, T max_x) noexcept
 : mval{x}, mmax{max_x}
  {};

  void bar() const { std::cout << "\nin Base."; }
  T max() const noexcept {return mmax;}
  T val() const noexcept {return mval;}
  T add(T x) const noexcept( !RC )
  {
    return add_impl<RC, T>()(x);
  }
};
*/

//  ---------------------- SOLUTION 3 ------------------------------ //
/*
#include <type_traits>
//#include <typeinfo>
template<typename T,  bool RC>
class Dummy
{
private:
  using do_range_check = std::integral_constant<bool, RC>;
  T mval, mmax;

public:
  constexpr explicit Dummy(T x, T max_x) noexcept
 : mval{x}, mmax{max_x}
  {};

  T add(T x) const noexcept(do_range_check::value) {
    return add(x, do_range_check{});
   }

private:
  T add(T x, std::false_type) const noexcept(do_range_check::value) {
    //std::cout << "\ndo_range_check{} = " << do_range_check{};
    //std::cout << "\ntypeid = " << typeid(do_range_check{}).name();
    //std::cout << "\ntypeid = " << typeid(std::false_type).name();
    //std::cout << "\ntypeid = " << typeid(do_range_check::value).name();
    return x + mval;
   }

  T add(T x, std::true_type) const noexcept(do_range_check::value) {
    T ret_val = x + mval;
    if (ret_val < 0 || ret_val > mmax) {throw 1;}
    return ret_val;
   }
};
*/

//  ---------------------- SOLUTION 4 ------------------------------ //
/*
template<typename T,  bool RC>
class Dummy
{
private:
  T mval, mmax;

public:
  constexpr explicit Dummy(T x, T max_x) noexcept
 : mval{x}, mmax{max_x}
  {};

  T add(T x) const noexcept( !RC ) {
    T ret_val = x + mval;
    if (RC && (ret_val < 0 || ret_val > mmax)) {
      throw 1;
     }
    return ret_val;
   }
};
*/

//  ---------------------- SOLUTION 5 ------------------------------ //
template<typename T>
struct ThresholdChecker
{
  ThresholdChecker(T value) : mMax(value) {};

  void check(T value) const
  {
    if (value < 0 || mMax < value) {
      throw 1;
     }
  }
private:
  T mMax;
};

template<typename T>
struct NoMaxCheck
{
  ThresholdChecker(T) {};
  void check(T) const noexcept {};
};

template<typename T,  typename CheckPolicy>
class Dummy
{
private:
  T mval;
  CheckPolicy mThresholdChecker;

public:
  explicit Dummy(T x, T max_x) noexcept : mVal(x), mThresholdChecker(max_x) {};

  T add(T x) const noexcept(noexcept(mThresholdChecker.check(x)))
  {
    T ret_val = x + val();
    mThresholdChecker.check(ret_val);
    return ret_val;
  }
};

#include <chrono>
using namespace std::chrono;

constexpr float FLT_MAX { 200.0 };
float random_float()
{
  return static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/FLT_MAX));
}

int main()
{
  /*
  high_resolution_clock::time_point t1 = high_resolution_clock::now();

  for (int j = 0; j < 10; j++) {
    Dummy<float,false> d(0, 1000+j);
    for (int i = 0; i < 1; i++) {
      d.add(random_float());
    }
  }
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();

  std::cout << "\n duration : " << duration;
  //std::cout <<"\nAdding  156.7 gives " << d.add(156.7);
  //std::cout <<"\nAdding 3156.7 gives " << d.add(3156.7)
  */

  Dummy<float,NoMaxCheck<float>/*false*/> df(0, 1000);
  Dummy<float,ThresholdChecker<float>/*true*/> dt(0, 1000);
  std::cout <<  "\nSize of false = " << sizeof(df);
  std::cout <<  "\nSize of true  = " << sizeof(dt);
 
  std::cout <<"\n";
  return 0;
}