# Grids

* Definition & implementation in [grid.hpp](../src/antenna.hpp)

# TickAxisImpl

This class is used to represent a tick axis, starting from tick with value `start_`, ending to tick `stop_` with a step size of `step_`. Note that `stop_` does not have to be greater than `start_`, i.e. the axis can be in either ascending or descending order. The representation of the tick-axis is always from `start_` to `stop_` like:

```

   start_ + step_
          ^
          | 
  index]  |
    [0]  [1]  [2]         npts_-1
  .--+----+----+--...--+----+--..
     |                      |
     v                      v
    start_                 stop_
[left]         -->            [right]
```

The `TickAxisImpl` is a template class, based on two parameters:

* `typename T` representing the type of axis e.g. `float`,`double`, `long`, ...
* `bool RangeCheck` which enables (or disables) range-check

summing up to a decleration:

```cpp
template<typename T, bool RangeCheck> class TickAxisImpl { /*...*/ };
```
