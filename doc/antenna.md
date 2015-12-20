# GNSS Antennas

* Definition in [antenna.hpp](../src/antenna.hpp)
* Implementation in [antenna.cpp](../src/antenna.cpp)

## Implementation

This is how an antenna instance is represented (memory map):

```
 N = antenna_model_max_chars
 M = antenna_radome_max_chars
 K = antenna_serial_max_chars

 [0,     N)       antenna model name
 [N+1,   N+M+1)   antenna radome name
 [N+M+2, N+M+K+2) antenna serial number

 | model name      |   | radome      |     | serial number     |       |
 v                 v   v             v     v                   v       v
 +---+---+-...-+---+---+---+-...-+---+-----+-----+-...-+-------+-------+
 | 0 | 1 |     |N-1| N |N+1|     |N+M|N+M+1|N+M+2|     |N+M+K+1|N+M+K+2|
 +---+---+-...-+---+---+---+-...-+---+-----+-----+-...-+-------+-------+
                     ^                  ^                          ^    
                     |                  |                          |    
                    ' '                 |                          |
                                       ' '                         |
                                                                  '\0'
```

From [[1]][rcvr_ant] the fundamental sizes are:
```
antenna_model_max_chars  = 15
antenna_radome_max_chars = 4
antenna_serial_max_chars = 20
```
Summing up to a total string of
```
  antenna_model_max_chars  + 1  /* whitespace */
+ antenna_radome_max_chars + 1  /* whitespace */
+ antenna_serial_max_chars = 41 /* characters */
```


## Reference
[rcvr_ant]: https://igscb.jpl.nasa.gov/igscb/station/general/rcvr_ant.tab
