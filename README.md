# LAVA - Live Asynchronous Visual Architecture

LAVA is meant to simplify high performance software while allowing every piece to be lock free and asynchronous.

[![Load Obj](https://github.com/LiveAsynchronousVisualizedArchitecture/lava/blob/master/images/Thumb_Demo_LoadObj.gif "")](#load-obj)
[![Camera Rays](https://github.com/LiveAsynchronousVisualizedArchitecture/lava/blob/master/images/Thumb_Demo_CameraRays.gif "")](#camera-rays)
[![Brandisher Elements](https://github.com/LiveAsynchronousVisualizedArchitecture/lava/blob/master/images/Thumb_Demo_BrandisherElements.gif "")](#brandisher-elements)
[![Trace](https://github.com/LiveAsynchronousVisualizedArchitecture/lava/blob/master/images/Thumb_Demo_Trace.gif "")](#trace)
[![Interactive Trace](https://github.com/LiveAsynchronousVisualizedArchitecture/lava/blob/master/images/Thumb_Demo_InteractiveTrace.gif "")](#interactive-trace)
[![Constant Bake](https://github.com/LiveAsynchronousVisualizedArchitecture/lava/blob/master/images/Thumb_Demo_ConstantBake.gif "")](#constant-bake)
[![Shade Rays](https://github.com/LiveAsynchronousVisualizedArchitecture/lava/blob/master/images/Thumb_Demo_ShadeRays.gif "")](#shade-rays)
[![Constant Shade](https://github.com/LiveAsynchronousVisualizedArchitecture/lava/blob/master/images/Thumb_Demo_ConstantShade.gif "")](#constant-shade)

LAVA is designed to **_both_** significantly **_speed up development_** AND as a simple way to create **_signifcant amounts of lock free concurrency_**.  It is written in C++11 and meant to potentially work with any language that can compile a shared library that exposes standard function calls.  The building blocks are single file libraries with no dependencies other than the C++11 standard library. 


### Classic Software Problems
|[*Scalability*](#scal)     |[*Iterations*](#iter)  |[*Modularity*](#mod)    |[*Concurrency*](#concr)       |[*Debugging*](#debug)   |
|       :---:               |      :---:            |   :---:                |    :---:                     |   :---:                |
|[OpenGL Graph](#graphui)   |[Live Reloading](#live)|[Shared Libs](#libs)    |[Execute in Parallel](#async) |[Shared Memory](#shrmem)|
|[Clear Interfaces](#clear) |[Output Baking](#bake) |[Crash Isolation](#crsh)|[Lock Free](#lkfree)          |[Visualization](#viz)   |
|[Flow+Msg Nodes](#flow-msg)|[Visualization](#viz)  |[Serial Data](#serial)  |[Persistant Threads](#thrds)  |[Tbl and Stats](#stats) |

### Classic Software Problems
 - <a id="scal"> __Scalable Complexity__ </a> - High level structure is not strictly enforced (or doesn't exist) and often subverted in some way to accomodate extra data/communication.
  
 - <a id="iter"> __Iterations__ </a> - As program size increases, iterations decrease due to re-compilation time, linking time, and the time to re-run the program when testing.
  
- <a id="mod"> __Modularity__ </a> - Modularity often breaks down due to data dependencies at run time and source dependencies at compile time, making re-use more difficult and increasing the barrier to entry for anyone to a project.

- <a id="concr"> __Concurrency__ </a> - Many techniques and libraries exist, often as heavy dependencies that have narrow use cases where they excel. Concurrency, parallelism and asynchronous design are perpetually difficult to get right and fragmented in their use.

- <a id="debug"> __Debugging__ </a> - Often means using slow builds, multiple runs to narrow down the problem and examining the state individual variables line by line.

### How LAVA Confronts These Problems

#### High Level Structure
- <a id="graphui"> __Graph UI__ </a> - A fluid, openGL accelerated UI for architecting software visually while implementing incrementally.

- <a id="clear"> __Clear Inputs and Outputs__ </a> - Every node has inputs and outputs that show up visually making their scope clear.  Programs can be created from relativly few nodes. simplifying the comprehension of how large projects fit together and how their pieces interact. 

- <a id="flow-msg"> __Data Flow and Message Passing__ </a> - Flow nodes can be used for stateless transformations, making any data separated into a packet be dealt with concurrently.  Message nodes can hold state, use flow nodes and dictate the overall behavior of a program. 
  
#### Iterations, Interactivity and Testing 

- <a id="live">__Live Reloading__</a> -  - Input data can be frozen and a single node can be recompiled, automatically hot reloaded, then automatically run using the now static input.  This enables fast iteration even in a large program by isolating a single piece and its input while continuously viewing its output.

- <a id="bake">__Constant Nodes__</a> - Constant nodes can offer a way to change input in real time for interactive testing with visualization of results.

- <a id="viz">__Visualization__</a> - Visualizations happen with lock free shared memory to external processes and don't interfere with the execution of the main program

#### Modularity
- <a id="libs">__Shared Libraries__</a> - Contain one or more nodes and can change while the program is running. Compilation is isolated to the lib being revised as well as run time checks in debug builds.  

- <a id="crsh">__Interrupts and Exceptions__</a> - The LAVA loop catches low level interrupts so that a crash shows up in the graph as a red halo around the crashed node.

- <a id="serial">__Serialized Node IO__</a> - All communication between nodes is passed as pointer to a single span of contiguous memory. This means that all IO can be saved as a file, written to shared memory for visualization and debugging.  While this can work with any data structure that can be serialized, data structures that always use a single span of memory (like tbl.hpp) are likely to be a good default. 

#### Concurrency, Parallelism and Asynchronous Design

- <a id="async">__Separate Data Executes Concurrently__</a> - Every packet of data can be dealt with concurrently, giving a program lock free asynchronous execution with little effort. Parallelism is dictated by the amount that data can be isolated

- <a id="lkfree">__Lock Free by Default__</a> - Threads take packets from a queue and execute them using their destination node.  Memory for node IO is allocated lock free and owned by the thread using reference counting. Part of each thread's loop is deallocating memory after each node it executes. Even visualization is lock free using simdb.

- <a id="thrds">__Threads Persist and Loop__</a> - Threads are meant to be created initially and loop, finding packets and executing them with their destination node. Each thread's stack can be used as thread local  eliminates overhead such as global memory allocation (which can lock) of thread creation.

- <a id="shrmem">__Shared Memory__</a> - 
- <a id="viz">__Visualization__</a> - 
- <a id="stats">__Tbl Tree View With Statistics__</a> - 

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
 -  Constant nodes will be an elegant way to change data going in to nodes in real time. Input data can be created first then the execution can be tested on every change as it happens in real time.  Inputs for parameters then can be controlled dynamically or statically
 
## Fundamental Principles

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

### Tools

### Fissure

![alt text](https://github.com/LiveAsynchronousVisualizedArchitecture/lava/blob/master/images/craftsman_fissure.jpg "Fissure is the node graph UI.  It can be used to construct a graph of nodes, run the program, visualize outputs, see node errors and view timing information about the nodes.")

Fissure is the node graph UI.  It can be used to construct a graph of nodes, run the program, visualize outputs, see node errors and view timing information about the nodes.

### Visualizer

![alt text](https://github.com/LiveAsynchronousVisualizedArchitecture/lava/blob/master/images/craftsman_visualizer001.jpg "Tables that are in the IdxVerts format (3D geometry with optional normals, vertex colors, uvs, and a color texture map)  will be picked up by the visualizer and displayed with openGL.")

[Tables](README.md#tblhpp) that are in the IdxVerts format (3D geometry with optional normals, vertex colors, uvs, and a color texture map)  will be picked up by the visualizer and displayed with openGL.

### Brandisher

![alt text](https://github.com/LiveAsynchronousVisualizedArchitecture/lava/blob/master/images/craftsman_brandisher001.png "The brandisher is a tool for viewing tables and their sub-tables in shared memory. It can display a graph of the arrays' values as well as their basic statistics.")

The brandisher is a tool for viewing tables and their sub-tables in shared memory. It can display a graph of the arrays' values as well as their basic statistics.  This screen shot shows that while some of the points making up the craftsman model are less than 0 on the Y axis, most are above 0.  We can also see the minimum, maximum, average (mean), most common (mode), and median (middle) values as well as the variance.   This is a useful way to get a high level view on arrays containing too many values to be looked at directly as text. 

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

### Examples

<a id="load-obj"> A constant file path is passed to an obj file loader node. </a>
The LoadObj node shown here is basically a wrapper around the [Tiny Obj Loader](https://github.com/syoyo/tinyobjloader) by [Syoyo Fujita](https://github.com/syoyo)
![alt text](https://github.com/LiveAsynchronousVisualizedArchitecture/lava/blob/master/images/Demo_LoadObj.gif "A constant file path is passed to an obj file loader node.")

<br>

<a id="camera-rays">Ray tracing rays generated and visualized in real time as a memory mapped tbl file (the purple constant node) is changed. </a>
![alt text](https://github.com/LiveAsynchronousVisualizedArchitecture/lava/blob/master/images/Demo_CameraRays.gif "Ray tracing rays generated and visualized in real time as a memory mapped tbl file (the purple constant node) is changed.")

<br>

<a id="brandisher-elements">Here the same tbl is show in two different places. On the right being edited as part of a const node (which just reads a .const file from disk). On the left it is read from shared memory.</a>
![Brandisher Elements](https://github.com/LiveAsynchronousVisualizedArchitecture/lava/blob/master/images/Demo_BrandisherElements.gif "Here the same tbl is show in two different places. On the right being edited as part of a const node (which just reads a .const file from disk). On the left it is read from shared memory.")

<br>

<a id="trace">A trace node (a wrapper around [the Embree ray tracing library from Intel](https://github.com/embree/embree)) is added and generated rays are traced to find where they collide with geometry.</a>
![Trace](https://github.com/LiveAsynchronousVisualizedArchitecture/lava/blob/master/images/Demo_Trace.gif "A trace node (a wrapper around the Embree ray tracing library from Intel) is added and generated rays are traced to find where they collide with geometry.")

<br>

<a id="interactive-trace">The field of view for the generated rays is changed and visualized outputs are updated in real time.</a>
![Interactive Trace](https://github.com/LiveAsynchronousVisualizedArchitecture/lava/blob/master/images/Demo_InteractiveTrace.gif "The field of view for the generated rays is changed and visualized outputs are updated in real time.")

<br>

<a id="constant-bake">An output that is already in shared memory (blue highlight, then stepped once) is middle-click dragged to make a constant node. This cuts the dependency on the rest of the graph while writing out the result to a file on disc.</a>
![Constant Bake](https://github.com/LiveAsynchronousVisualizedArchitecture/lava/blob/master/images/Demo_ConstantBake.gif "")

<br>

<a id="shade-rays">The traced rays are combined with the GGX BRDF to get new ray directions that originate from the model.  Their length when visualized is the result of their PDF (sample weight). </a>
![Shade Rays](https://github.com/LiveAsynchronousVisualizedArchitecture/lava/blob/master/images/Demo_ShadeRays.gif "The traced rays are combined with the GGX BRDF to get new ray directions that originate from the model.  Their length when visualized is the result of their PDF (sample weight).")

<br>

<a id="constant-shade">The results of geometry loading, ray generation and ray tracing are written to a constant file for rapid iteration with no dependencies.</a>
![Constant Shade](https://github.com/LiveAsynchronousVisualizedArchitecture/lava/blob/master/images/Demo_ConstantShade.gif "The results of geometry loading, ray generation and ray tracing are written to a constant file for rapid iteration with no dependencies.")

<br>

<a id="shade-ray-hits"> Rays generated from a camera and traced to find their collisions with a 3D model using the embree library. </a>
![alt text](https://github.com/LiveAsynchronousVisualizedArchitecture/lava/blob/master/images/rays_shadeRayHits001.jpg "Rays generated from a camera and traced to find their collisions with a 3D model using the embree library.")


This is an example of rays generated from a camera and traced to find their collisions with a 3D model using the embree library.
  - An .obj model is loaded 
  - The model is passed to a message node that uses [the Embree ray tracing library from Intel](https://github.com/embree/embree) to sort the geometry into a BVH (bounding volume heirarchy acceleration structure for ray tracing)
  - The camera node generates rays and passes them to the scene node to be traced
  - The message node traces the rays and outputs both the traced rays colliding with geometry and a visualization of the traced rays 
  - A final node takes the traced rays and computes the outgoing ray from the ray hit point and normal
  
  
  ### F.A.Q (Frequently Anticipated Questions)
  
  ### Internals
  
  
