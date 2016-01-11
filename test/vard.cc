#include <iostream>
#include <tuple>
#include <array>
#include <algorithm>

struct A { int a; bool operator==(A rhs) const { return a==rhs.a; } };
struct B { int b; bool operator==(B rhs) const { return b==rhs.b; } };
struct C { int c; bool operator==(C rhs) const { return c==rhs.c; } };
struct D { int d; bool operator==(D rhs) const { return d==rhs.d; }; friend struct dpolicy_ordered; };

struct dpolicy_jamanfu
{
    static bool match(const D& d1, const D& d2) noexcept
    { return true; }
};

struct dpolicy_strict
{
    static bool match(const D& d1, const D& d2) noexcept
    { return d1==d2; }
};

struct dpolicy_ordered
{
    explicit
    dpolicy_ordered(std::initializer_list<int>&& l)
    noexcept
        : lst{l}
    {}

    std::vector<int> lst;

    bool match(const D& d1, const D& d2)
    noexcept
    { 
        auto i1 = std::find( lst.cbegin(), lst.cend(), d1.d );
        auto i2 = std::find( lst.cbegin(), lst.cend(), d2.d );
        
        if ( i1 == lst.cend() || i2 == lst.cend() )
        {
            return false;
        }

        return std::distance(lst.cbegin(), i1)
                >= std::distance(lst.cbegin(), i2);
    }
};

class X
{

public:
    X(int a=0, int b=0, int c=0, int d=0)
    : _a{a}, _b{b}, _c{c}, _d{d}
    {}

#if __cplusplus > 201103L
    std::tuple<A,B,C,D> tie () { return std::tie(_a,_b,_c,_d); }
#else
    template <typename> friend struct xget;
#endif

    A a() const noexcept {return _a;}
    B b() const noexcept {return _b;}
    C c() const noexcept {return _c;}
    D d() const noexcept {return _d;}

private:
    A _a;
    B _b;
    C _c;
    D _d;
};

template<typename Policy>
bool compare(X x1, X x2, Policy* p=nullptr) noexcept
{
    return x1.a() == x2.a()
        && x1.b() == x2.b()
        && x1.c() == x2.c()
        && Policy::match(x1.d(), x2.d());
}
template<>
bool compare<dpolicy_ordered>(X x1, X x2, dpolicy_ordered* p) noexcept
{
    return x1.a() == x2.a()
        && x1.b() == x2.b()
        && x1.c() == x2.c()
        && p->match(x1.d(), x2.d());
}

#if __cplusplus > 201103L
    template<typename... Args>
    bool match(X x1, X x2)
    { 
        return std::make_tuple(std::get<Args>(x1.tie())...) == 
               std::make_tuple(std::get<Args>(x2.tie())...); 
    }
#else
    template<typename> struct xget {};
    template<> struct xget<A> { static const A& get(const X& x) { return x._a; } };
    template<> struct xget<B> { static const B& get(const X& x) { return x._b; } };
    template<> struct xget<C> { static const C& get(const X& x) { return x._c; } };
    template<> struct xget<D> { static const D& get(const X& x) { return x._d; } };
    template <typename ...Args> bool match(const X& lhs, const X& rhs)
    {
        return std::tie(xget<Args>::get(lhs)...) == std::tie(xget<Args>::get(rhs)...);
    }
#endif

int main()
{
    X x1 (1,2,3,4);
    X x2 (0,1,2,3);
    X x3 (3,3,3,3);

    X x4 (1,2,3,0);
    X x5 (1,2,3,5);

    std::cout << match<A,B,C,D>( x1, x2 ) << "\n" ;
    std::cout << match<A,B,C>( x1, x2 ) << "\n" ;
    std::cout << match<A,B>( x1, x2 ) << "\n" ;
    std::cout << match<A>( x1, x2 ) << "\n" ;
    std::cout << match<C>( x1, x3 ) << "\n";

    std::cout << compare<dpolicy_jamanfu>(x1, x4) << "\n";
    std::cout << compare<dpolicy_strict>(x1, x4) << "\n";
    
    dpolicy_ordered pol {0,1,2,3,4,5,6,7,8,9};

    std::cout << compare<dpolicy_ordered>(x1, x4, &pol) << "\n";
    std::cout << compare<dpolicy_ordered>(x1, x5, &pol) << "\n";

    return 0;
}
