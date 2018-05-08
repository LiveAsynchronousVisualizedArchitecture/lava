
### Common Problems With Visual Programming

  -  Most environments use custom languages instead of leveraging the enormous ecosystem, maturity, speed and extensive design that has gone into existing languages. LAVA is made using a few thousand lines of C++ spread across 3 header files with no external dependencies
  -  Pure data flow visual programming has been successful in domain specific applications, though generalizing to arbitrary software can can break the tight mapping of the interface to execution needed
  -  Control flow, looping, data structures and persistant state can be difficult to control elegantly
  -  Dense expressions that would be elegant in text can become unweildly when using granular nodes 

### Proposed Solutions to Common Visual Programming Problems
 -  Each node is made using C++, which lets the expression level programming be done normally
 -  LAVA uses multiple node types, most notably message and flow nodes
 -  Individual nodes are written in modern C++ and meant to accomplish high level goals on large chunks of data
 -  Message passing nodes can manage persistent state and dictate the high level program flow
 -  Flow nodes transform data in highly modular pieces with easy visualization and well defined inputs and outputs 
 -  Generator nodes can elegantly bring in network traffic, file IO, etc
 -  Constant nodes are an elegant way to change data going in to nodes in real time. Input data can be created first then the execution can be tested on every change as it happens in real time.  Inputs for parameters then can be controlled dynamically or statically
 
### Fundamental Principles

#### 1. A program is composed of message passing nodes and data flow nodes. This enables many desirable features: 
 -  A clear picture of the high level structure of a program along with a way to plan the program structure in a precise way.
 -  Truly modular pieces that have that clearly defined inputs and output with no side effects.
 -  Detailed information about each node including input and output types, percentage of CPU time, statistics about data size to execution time, etc.

#### 2. Update Nodes Live
 -  Since data is separated from execution in a clear way, a recompile of a shared library can trigger an update while the program is running, meaning a program can be changed live, without restarting. 

#### 3. Freeze Input
 -  Combined with live updating of nodes, this allows rapid development of a section of the program since a compile can automatically update with frozen packets run through immediatly then visualized.

#### 4. Granular Interrupt Handling 
 -  Enables the program to continue running live, even if one node crashes. 
 -  Crashes are shown clearly and don't disrupt workflow.
 -  Running threads will simply skip packets destined for the crashed node.

#### 5. Visual Node Graph Interface
 -  Reduces complexity of how a program fits together as well as giving easy and intuitive feedback
 -  High level information such as node crashes or CPU time spent running each node is integrated and always available
