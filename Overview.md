
### Overview 

LAVA stands for Live Asynch Visual Architecture

That likely creates more questions than answers. The benefits are more about the tight integration of all these ideas together, so we'll look at something concrete first.  This won't showcase very many of the benefits yet, but it will give some context. 

We can use a program that will load a 3D model as an example.  LAVA is based around connecting both data flow and message passing nodes in a GUI while still writing the program in C++.  Because of this we can use a common C++ library that loads a .obj file and just wrap it in a specific function so that it can be used as a node in the graph.

[LoadObj node implementation](https://github.com/LiveAsynchronousVisualizedArchitecture/lava/blob/master/Fissure/LoadObj/LoadObj.cpp)

That file then gets compiled to a shared library.  A lava shared library can contain one or more nodes.  The lava core copies the shared library to a different directory and finds the nodes in it. This lets you compile over the generated shared library on every iteration while lava takes care of the live reloading.  Here is what it looks like: 

| Shared Library || Node List || Instance Button || Node 
| :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| ![Shared Library](https://github.com/LiveAsynchronousVisualizedArchitecture/lava/blob/master/images/Fissure_LoadObj.dll.png "") | ➡️ | ![Node List](https://github.com/LiveAsynchronousVisualizedArchitecture/lava/blob/master/images/Fissure_node_list.png "") | ➡️ | ![LoadObj Instance Button](https://github.com/LiveAsynchronousVisualizedArchitecture/lava/blob/master/images/Fissure_node_list_load_obj.png "") | ➡️ | ![LoadObj Node](https://github.com/LiveAsynchronousVisualizedArchitecture/lava/blob/master/images/Fissure_obj_node.png) |


If we mouse over the inputs and outputs we can see the descriptions and type information that were given in [the first few lines of the LoadObj node implementation.](https://github.com/LiveAsynchronousVisualizedArchitecture/lava/blob/master/Fissure/LoadObj/LoadObj.cpp#L19) 


| Mouse Over the Input | Mouse Over the Output | 
| :---: | :---: | 
| ![Input (mouse cursor not shown)](https://github.com/LiveAsynchronousVisualizedArchitecture/lava/blob/master/images/Fissure_mouseover_loadobj_in.png "") | ![Output (mouse cursor not shown)](https://github.com/LiveAsynchronousVisualizedArchitecture/lava/blob/master/images/Fissure_mouseover_loadobj_out.png "") |


### Why

- Computer graphics programs like Maya, Nuke, Houdini, Touch Designer have fantastic productivity in their limited domains
- Architectures are flexible, most of the functionality is implemented as plugins 
- Work well because but focus on transformations without state or side effects
- Limited number of data types
- No branching or loops in the graph
- Isolation and iteration 
- Visualization and error handling from the graph interface
- Problems show up quickly and are easy to isolate
- Is there a way to structure arbitrary programs like this? 
- Can an arbitrary program be made (almost) entirely from individual C++ plugins with only a minimal generic core?
- Will have to deal with arbitrary data types, state, side effects and synchronization

### How 

- Load shared libraries that each contain one or more nodes
- Nodes communicate by sending serialized data chunks from their outputs to another node's inputs
- The sending of serialized data from one node to another is done with a queue
- When nodes call a function to ouput chunks, they are actually puttting those chunks in the queue
- Threads (commonly one thread per logical core) run a main loop looking for any chunks in the queue and use the graph to see what node to use to run them
- Some nodes are run when there are no chunks in the queue - this is how a program can start with an empty queue  
- A thread that sees an empty queue will look for a message passing node, or a flow node without input slots and run that node without passing it any chunks as input 
- A node implementation is just a cdecl function with a certain signature
- A node's function is given pointers to its inputs, a pointer to a memory allocation function to use for data that it will output and some other helper data and functions


When C++ is used for almost all the execution and the graph is used for the overall structure, a complex program can be much easier to comprehend and iterate on, with the added benefit that significant concurrency is much easier, since all chunks of data are dealt with asynchronously.   



Lava is about connecting high level nodes written in native languages like modern C++
-  Think about wrapping a small library like an image loader in a single node
-  Just a few single file C++ libraries that are a few dozen kilobytes each 
-  Tools are separate and modular 

Made for general purpose software 
-  Message passing nodes are one of the big ideas
-  -  Most of the algorithmic complexity can be put into data flow nodes that are very modular and easy to test
-  -  Message passing nodes can hold state, control higher level branching and synchronize 
-  -  The parts of the program that are more difficult to debug are minimized and isolated 
-  High level nodes are another big idea
-  -  Instead of treating visual programming as a silver bullet, 
-  -  Program are still written in native languages 
-  -  Structured, planned and debugged visually  
-  -  Libraries meant to handle chunks of data can be wrapped in nodes and used fluidly and easily

Two main node types - Flow nodes and message passing nodes
-  Flow nodes are pure functions that hold no state, meant for transforming data
-  -  Results should only change if input changes or the node changes 
-  -  Can be run over and over - great for fast, automatically hot loaded iterations  
-  -  Easily tested by changing input
-  -  Data for re-running a flow node continuously can be saved easily, since all IO between nodes is serialized

Tools for debugging are powerful, fast and easy to write 
-  simdb makes looking at chunks of data as easy as including a header file and looping through the keys 
-  Visualizations can be written  
-  Look at graphs of array values, statistics like mean median and mode
-  Tools could be made to look at arrays of strings 

