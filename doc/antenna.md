# GNSS Antennas

* Definition in [antenna.hpp](../src/antenna.hpp)
* Implementation in [antenna.cpp](../src/antenna.cpp)

## Implementation

This is how an `antenna` instance is represented (memory map):

```
 N = antenna_model_max_chars
 M = antenna_radome_max_chars
 K = antenna_serial_max_chars

  [0,     N)       antenna model name
  [N+1,   N+M)     antenna radome name
  [N+M+1, N+M+K+1) antenna serial number

  | model name      |   | radome      | serial number     
  v                 v   v             v    
  +---+---+-...-+---+---+---+-...-+---+-----+-----+-...-+-------+
  | 0 | 1 |     |N-1| N |N+1|     |N+M|N+M+1|N+M+2|     |N+M+K+1|
  +---+---+-...-+---+---+---+-...-+---+-----+-----+-...-+-------+
                      ^                                     ^       
                      |                                     |      
            whitespace character                           '\0'
```

From [[1]](#rcvr_ant) the fundamental sizes are:

> Note that [[1]](#rcvr_ant) does not mention antenna serial numbers; all
> relevant info, are extracted from [[2]](#antex14)

```
antenna_model_max_chars  = N = 15
antenna_radome_max_chars = M = 4
antenna_serial_max_chars = K = 20
```
Summing up to a total of
```
antenna_full_max_chars =
                  antenna_model_max_chars
                + 1 /* whitespace */
                + antenna_radome_max_chars
                + antenna_serial_max_chars
                + null-terminating-character
                      = 41 /* characters */
```

## API Reference

### Constructing antennas

A user can construct an `antenna` instance in the following ways:

* using the default constructor : `antenna::antenna()`. This will set all
  characters in the `antenna` to `'\0'`.
* copy/initialize from a [c-string](https://en.wikipedia.org/wiki/C_string_handling) or an
  `std::string`. This will copy, **at max** `N + 1 + M + K` characters. If the
  copied-from string is less than this, the remaining characters are set to `'\0'`.

## Modification & other functions

You can use the `=` operator to assign/modify an `antenna`, using an input
c-string or an `std::string`. The following function can also be used:

* `set_serial_nr(const char*)` to set the serial number of an `antenna`. Note that
  a maximum of `antenna_serial_max_chars` will be copied.

* `compare_serial(const char*)` compares the instances serial number to the one
  provided as input argument.

## Operators

`antenna` instances have overloaded operators for basic usage. These are:

* `operator==` compares two `antenna`s. Note that **only antenna type and radomes**
  are checked **not serial numbers**. If need to strictly compare two `antenna`s
  then use the `is_same()` function (if two antennas have the same serial number
  they are actualy one and the same antenna!).

* `operator!=` is also provided, to negate the `==` operator.

* `operator<` compares two `antenna` instances lexicographically (can be used
  to sort `antenna`s).

# Examples

See the file [test_antenna.cpp](../test/test_antenna.cpp) for basic usage.

# Warnings

The `antenna` class interacts *a lot* with `c-strings`. You should be aware that
all `c-strings` are **null-terminated** arrays of characters; failing to provide
pointers to null-terminated (character) strings can cause 
[UB](https://en.wikipedia.org/wiki/Undefined_behavior).

The user is responsible for assigning **valid** `antenna` names (types, radomes 
and serial numbers in the correct format). It is nearly impossible to check
wether a given antenna name (either as a `c-string` or as an `std::string`) is
valid via a method/function.

# ToDo

- [ ] Revisit the source code
- [ ] Change/Enhance the example
- [ ] Differences between station and satellite antennas
- [ ] Methods for providing `std::strings`, e.g. `antenna_to_string`

# References

<a name="rcvr_ant">[[1]](https://igscb.jpl.nasa.gov/igscb/station/general/rcvr_ant.tab)

<a name="antex14">[[2]](https://igscb.jpl.nasa.gov/igscb/station/general/antex14.txt)
