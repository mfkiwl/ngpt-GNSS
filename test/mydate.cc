#include <iostream>
#include <type_traits>

CHECKOUT THIS -> http://embeddedgurus.com/stack-overflow/2009/06/division-of-integers-by-constants/

namespace koko {

struct seconds {
    constexpr static bool is_foo = true;
    long _sec;
    explicit constexpr seconds(long s) : _sec(s) {};
};

struct milliseconds {
    constexpr static bool is_second_submultiple = true;
    long _msec;
    explicit constexpr milliseconds(long s) : _msec(s) {};
};

struct nanoseconds {
    constexpr static bool is_second_submultiple = true;
    long _nsec;
    explicit constexpr nanoseconds(long s) : _nsec(s) {};
};

template<typename T>
struct issm {};
template<> struct issm<seconds> { typedef long ttype; };

struct hours {
    int _hours;
    explicit constexpr hours(int s) : _hours(s) {};
};

struct minutes {
    int _minutes;
    explicit constexpr minutes(long s) : _minutes(s) {};
};

template<class S>
class time {

public:
    template<typename T,
            /*typename = typename std::enable_if<T::is_foo>::type*/
            typename std::enable_if<sizeof(typename issm<T>::ttype),
                                    typename issm<T>::ttype>::type* = nullptr
            >
    time(hours h, minutes m, T s)
        : _h(h), _m(m), _s(s)
    { std::cout<<"\nUsing template c'tor"; }

    void add_hours( hours h ) { _h += h; }

private:
    hours   _h;
    minutes _m;
    S       _s;
};

}

int main()
{
    koko::hours h (12);
    koko::minutes m (1);
    koko::seconds s(59);

    koko::time<koko::seconds>  t1(h, m, s);
    /* koko::time<int> t2(h, m, 10); -- NICE this won't instantiate -- */

    std::cout << "\n";
    return 0;
}
