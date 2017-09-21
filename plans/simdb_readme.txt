



simdb.hpp <2k lines and 70KB - A high performance, shared memory, lock free, cross platform, single file, no dependencies, C++11 key-value store (Apache 2)




simdb.hpp - A high performance, shared memory, lock free, cross platform, single file, no dependencies, C++11 key-value store

https://github.com/LiveAsynchronousVisualizedArchitecture/simdb

- Hash based key-value store created to be a fundamental piece of a larger software architecture. 

- High Performance - Real benchmarking needs to be done, but superficial loops seem to run *conservatively* at 500,000 small get() and put() calls per logical core per second. Because it is lock free the performance scales well while using at least a dozen threads. 

- Shared Memory - Uses shared memory maps on Windows, Linux, and OSX without relying on any external dependencies.  This makes it #### exceptionally good at interprocess communication. 

- Lock Free - The user facing functions are thread-safe and lock free with the exception of the constructor (to avoid race conditions between multiple processes creating the memory mapped file at the same time). 

- Cross Platform - Compiles on Visual Studio 2013, ICC 15.0, gcc on Linux, gcc on OS X, and Clang on OS X.

- Single File - simdb.hpp and the C++11 standard library is all you need. No windows SDK or any other dependencies, not even from the parent project. 

- Apache 2.0 License - No need to GPL your whole program to include one file. 

- This has already been used for both debugging and visualization, but *should be treated as alpha software*.  Though there are no known outstanding bugs, there are almost certainly bugs (and small design issues) waiting to be discovered and so will need to be fixed as they arise. 

There is an in-depth explanation in the comments of the simdb.hpp file itself.



simdb.hpp <2k lines and 70KB - A high performance, shared memory, lock free, cross platform, single file, no dependencies, C++11 key-value store (Apache 2)

https://github.com/LiveAsynchronousVisualizedArchitecture/simdb

- Hash based key-value store created to be a fundamental piece of a larger software architecture. 

- High Performance - Real benchmarking needs to be done, but superficial loops seem to run *conservatively* at 500,000 small get() and put() calls per logical core per second. Because it is lock free the performance scales well while using at least a dozen threads. 

- Shared Memory - Uses shared memory maps on Windows, Linux, and OS X without relying on any external dependencies.  This makes it __exceptionally good at interprocess communication__. 

- Lock Free - The user facing functions are thread-safe and lock free with the exception of the constructor (to avoid race conditions between multiple processes creating the memory mapped file at the same time). 

- Cross Platform - Compiles with Visual Studio 2013 and ICC 15.0 on Windows, gcc 5.4 on Linux, gcc on OS X, and clang on OS X.

- Single File - simdb.hpp and the C++11 standard library is all you need. No Windows SDK or any other dependencies, not even from the parent project. 

- Apache 2.0 License - No need to GPL your whole program to include one file. 







