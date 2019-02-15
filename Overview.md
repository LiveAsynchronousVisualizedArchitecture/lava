

# Overview 

LAVA stands for __L__ ive __A__ synch __V__ isual __A__ rchitecture

Because that description probably creates more questions than answers and the benefits are more about the tight integration of all these ideas together, let's look at something concrete.  While this won't showcase very many of the benefits yet, it will give some context. 

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

