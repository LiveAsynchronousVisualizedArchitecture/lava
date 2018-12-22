

# Overview 

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

