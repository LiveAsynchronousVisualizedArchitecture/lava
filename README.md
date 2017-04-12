# lava

LAVA stand for Live Asynchronous Visual Architecture. 

Before describing what each of these terms mean and how they fit together,
 there is something important to emphasize - LAVA is designed to both significantly speed up development 
AND as a simple and easy way to create signifcant amounts of concurrency. 
It is written in C++11 and is meant to work with any language that can compile a shared library that exposes standard function calls.

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
  
6. A separate 




