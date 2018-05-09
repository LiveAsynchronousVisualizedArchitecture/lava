
### Libraries

LAVA is made out of only a few componenents (with tbl.hpp as technically optional, though extremely valuable).  They are all single header files and have no external dependencies outside of C++11 standard libraries. 

####  LavaFlow.hpp
The core that loads nodes dynamically and runs them with the packets of data they produce.  Each thread will simply call the LavaLoop() function to enter the main loop and start executing nodes with their packets.

####  simdb.hpp
Lock free, shared memory key value store for exceptionally fast, concurrent, inter-process communication  

####  tbl.hpp
 - A fast and flexible data structure that *combines* a vector with a string->number/tbl robinhood hash-map.  
 - The vector can take on any intrinsic numeric type, which is then type checked and range checked during debug mode.   
 - It is always stored in a *single allocation of contiguous memory*, which means that it has *no separate serialized representation*. 
 - It can also *store nested tbls*, creating a simple way to make trees and compound data structures that are full featured, without having to define new structs or classes. 
 - These compound data structures are thus a single allocation, always serialized and can be read clearly in a tool like Brandisher to understand the format without needing extensive documentation.  
 - Due to their single span of memory they can easily be written to files and/or read directly from files using memory mapping.
 - The tbl struct contains a pointer to the main data as well as pointrs for allocation, reallocation and free functions. This allows tbls to carry their memory allocation functions with them, which is valuable for use with shared libraries (so that memory allocation functions in the main program don't get mixed with free functions in the shared library and vice versa).
