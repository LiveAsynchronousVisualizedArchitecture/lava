# LAVA

LAVA stands for Live Asynchronous Visual Architecture. 

Before describing what each of these terms mean and how they fit together,
 there is something important to emphasize - LAVA is designed to **_both_** significantly **_speed up development_**
AND as a simple and easy way to create **_signifcant amounts of concurrency_**. 
It is written in C++11 and is meant to work with any language that can compile a shared library that exposes standard function calls.
Many of the fundamental building blocks have been created as single file libraries with no dependencies other than the C++11 standard library. Examples include: 
  - simdb.hpp  -  Lock free, shared memory key value store for exceptionally fast, concurrent, inter-process communication  
  - tbl.hpp    -  A fast and flexible data structure that combines a template vector with a string->number/tbl robinhood hash-map. 
                  It is always stored in contiguous memory, which means that it has no separate serialized representation. 
                  It can also store nested tbls, creating a simple way to make trees and compound data structures that are full featured, without having to define new structs or classes. 

Development is made easier through multiple techniques:
1. A program is composed of message passing nodes and data flow nodes. This enables many desirable features: 
 - A clear picture of the high level structure of a program along with a way to plan the program structure in a precise way.
 - Truely modular pieces that have that clearly defined inputs and output with no side effects.
 - Detailed information about each node including input and output types, percentage of CPU time etc.

2. The ability to update nodes live.
 - Because data is separated from execution in a clear way, a recompile of a shared library can trigger an update while the program is running, meaning a program can be changed live, without restarting. 

3. The ability to freeze the input to any node.
 - Combined with live updating of nodes, this allows rapid development of a section of the program since an compile can automatically update a node and the frozen data can be run through automatically, showing the output. 

4. Per node interrupt handling enables the program to continue running live, even if one node crashes. 
   Mistakes are seen early, shown clearly and don't hurt workflow. 

5. A visual interface for both message passing and data flow nodes reduces the complexity of understanding how a program fits together. 
  
6. A separate visualizer process with an openGL window for visualizing the data passed between nodes. 
   This enables inputs and ouputs at any point in the program to be displayed intuitivly and automatically using lock free shared memory. 




