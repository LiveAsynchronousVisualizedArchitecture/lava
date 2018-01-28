
// -todo: put loop into LavaFlow IMPL 
// -todo: put loop dependent functions into the anonymous namespace
// -todo: take out LavaId implicit cast from u64 - already done earlier
// -todo: break out the loop function
// -todo: work out getting size of out of a node - make room for both reference counts and the size in bytes in the allocation
// -todo: make helper LavaMem struct
// -todo: partition owned memory with a zero reference count, free them and finally erase them from the vector
// -todo: make the lava allocator passed to a node allocate an extra 8 bytes for the reference count 
//       |  make sure that extra data at the beggining is treated atomically
//       |  make sure that memory is allocated aligned to a 64 byte cache line
// -todo: put packet arguments into the InArgs array and use it to decrement references after the the function has ran
// -todo: does the input arg need a slot number? do the slot numbers just need to be used to place it in the array - put in slot for now 
// -todo: make owned memory vector use the thread local allocation
// -todo: make LavaAlloc use LavaHeapAlloc 
// -todo: make message passing nodes a set instead of a vector 
// -todo: figure out why delNode is passed a wrong node id - is it LavaNode::NONE ?  - pointer / iterator was being invalidated by deletion
// -todo: make sel_delete delete from the message passing nodes set - nothing was deleting from the LavaGraph
// -todo: figure out why a deleted node id shows up - is it not being deleted from the msg passing nodes? 
// -todo: put some output in simdb
// -todo: make initializer list run the main init function in tbl - actually need to split the fields initialization from any alloction 
// -todo: split table printing into multiple functions
// -todo: take owned() query out of internal cp() function? - done, but crash still happens, possibly on destroy() during copy
// -todo: make initializer list not use emplace, but copy to indices, since the array elements already exist
// -todo: figure out why flattening the tbl is not working - is the type of the child table not the same on each run? - is flatten not making a child table into a child table type when it is internal? - possible problems with shrink_to_fit()
// -todo: either visualize strings, or put an indexed verts object into the db - probably easier to make another node that outputs an indexed verts table - node that outputs IdxVerts created
// -todo: fix selection again
//       -| figure out all information like the slot and node that's inside, box drag etc click up or down etc, 
//       -| redo connection creation
//       -| redo primary node selection with reordering
//       -| put it all together at the end 
// -todo: make basic command queue - enum for command, priority number - use std::pri_queue - use u32 for command, use two u64s for the arguments  - not neccesary with selection refactoring
// -todo: take out clear node selections
// -todo: take out node state selection on primary selection
// -todo: fix title changing on node deletion - title was changed to the last key pressed 
// -todo: make nanogui message bar at the bottom
// -todo: make status bar show information about the selection 
// -todo: make popup over each node that shows statistics and information like the node number - status bar will take care of this for now
// -todo: fix current node in top right corner - m_curId initialized to LavaNode::NONE instead of 0
// -todo: compile visualizer in release mode - doesn't work the same as debug mode - even debug seemingly calls into eigen at unrelated times while stepping through
// -todo: put in right button mouse states and right mouse clicks
// -todo: make a hook function for LavaFlow to call after every packet
// -todo: make 64 bit atomic ring buffer to know if a Node / slot should be put into the db - just a Set with linear comparisions, not a ring buffer, not hash based
// -todo: make packet callback check if a node-slot combination is in the array of Ids that need to be written - if the node-slot Id is in the vizualize array, then put it into the db
// -todo: separate out slot key creation for deletion
// -todo: fix AtmSet del - compare and swap called with argument order flipped
// -todo: delete a buffer from the visualization DB using the main GUI thread when deleting from the atomic set of visualized slots
// -todo: when turning off a slot, delete the entry from the database 
// -todo: make right clicking on slot visualize that slot with a combination of the text label, node id and slot id  as the db key
// -todo: make play execute a stop() first
// -todo: make play be greyed out while running
// -todo: make play, pause and stop have different colors
// -todo: change status bar on mouse over instead of click
// -todo: try wrapping 'structured exception handling' - just needed to wrap the structured exception handling in a dedicated function
// -todo: fix infinite loop when deleting from the db when the key is not found - needed to end the loop after looping through all the keys - when ending the loop, needed to return empty
// -todo: put background highlights on visualized slots
// -todo: take out Id type alias
// -todo: fix deletion - make delete and backspace delete selected nodes - might need to check the primary selection as well as selection states - sel_nodes now includes the primary selection as well
// -todo: use exceptions to flag node instances 
// -todo: build in a state variable to the LavaInst struct
// -todo: create red background highlights on nodes that generate exceptions
// -todo: build in the atomic store and load functions into the LavaInst struct
// -todo: make exceptions in the shared library functions put the packet back into the queue
// -todo: make nodes highlight on mouse over - white with 0.025 opacity seems to work well to be able to see when a node is moused over
// -todo: put timer into each node instance - try C++11 high resolution clock now() with doubles
// -todo: make status bar show the timing data for each node
// -todo: make time adding atomic
// -todo: make time reset on stop
// -todo: make time totaling function for the LavaGraph
// -todo: make the status bar update continuously even for the last moused over node? - need to keep the last node in the global state - just use the primary selection
// -todo: make command queue for LavaGraph so that changes to the graph can be stored and queued
// -todo: finish using cur() functions
// -todo: fix slot movement - src and destination were flipped in the toggle command
// -todo: make LavaFlow loop always use the right buffer
// -todo: make LavaGraph edit functions use the opposite buffer
// -todo: fix LavaGraph - figure out why there are slots that are [0:0] - sel_clear() was setting current slots to LavaId(0,0)
// -todo: figure out how to create slots from commands when the slots need the Node Id - use a stack of return values
// -todo: change node_add to use commands - how can slots get the sid back from the commands being run? - return stack args from exec() in a vector()?
// -todo: use command queue to batch commands, execute them, and switch the data structures 
// -todo: convert lava graph changes to use the command queue
// -todo: make two graphs and switch back and forth with an atomic bool
//       | -should there be two graphs, one read and one write 
//       | -make sure that the write has a mutex or some sort of locking - don't need a mutex if there is a bool that switches buffers and the commands are sent and executed from a single thread
//       | -do commands only need two arguments? one or two LavaIds with a command enum
// -todo: fix slots not showing up - nodeSlots() needed a const and non-const version, each of which uses a different buffer
// -todo: fix node selection being inverted
// -todo: convert LavaFlow to class with const LavaGraph const& function to access the graph as read only - similiar approach using dual buffers taken
//       | -does there need to be a function to copy the instances and connections? - should this ultimatly be used for drawing the graph? - this is done in the exec() function befoe executing the command queue
//       |  can the graph be condensed into a tbl ? 
// -todo: use a copy of the graph to clear and update the interface buttons - not neccesary due to the LavaGraph dual buffers? 
// -todo: make a function to get a copy of the graph - can check the version number every loop, and if it is higher, get a copy of all the data inside a mutex - not neccesary from the dual buffers
// -todo: make frame queue
// -todo: make LavaFrame struct
// -todo: make slotCount() count the number of bits in the slotMask
// -todo: make an atomic frame number part of LavaNodeInst and a method to fetch_add to it
// -todo: make LavaFlowFunc take a LavaFrame as input 
// -todo: make a putSlot function for LavaFrame
// -todo: find a single message node and run that - should there be a global next message node index or should there be a next message node per thread? - single global atomic is simplest and possibly still very good solution - message nodes are not in an array, they are in a ahash
// -todo: make sure to only run message nodes when there are no more outstanding packets
// -todo: fix probable race condition of updating the cache of indices - make a cache as another A/B member of 
// -todo: fix crash on msg node - didn't switch the flow loop to use the graph's id cache
// -todo: make sure to run a single packet on each increment through the main loop - done with the single nxtPacket call that switches to trying a message node if none are found
// -todo: set a node's state to error if the loading doesn't work
// -todo: figure out a way to query the next message node directly, since an index from the index cache to a query to the msg node could have a change in the middle
// -todo: break out status text creation into separate function
// -todo: clamp status text at 100% of time per node
// -todo: figure out why frame rate seems clamped to 144 - does having two monitors with the same refresh rate, turn vsync for windows back on and vsync glfw? - not sure, but a glfw swap interval of 0 and 1 seem to do the same thing - 24hz desktop means 24fps GUI even with a swap interval of 0
// -todo: test message node to flow node - doesn't crash, but message node is also not highlighted
// -todo: make sure LavaFrame path actually runs the node - is the packet making it out of the message node?
// -todo: make LavaFrame have a source frame number and a dest frame number - not used right now - LavaFrame frame number comes from the first packet, which should also be looking for packets of the same frame number
// -todo: change the decrementing of references to look into the LavaFrame passed in instead of LavaArgs, right now it will not have an effect
// -todo: make slot loop only check if they are taken or not
// -todo: separate out bitset - use std::bitset instead
// -todo: change LavaFrame bitset names - taken out due to bitset
// -todo: type notes for flat_lockfree_map
// -todo: work on flat lock free map
// -todo: make an atomic bitset
// -todo: test atomic bitset
// -todo: use AtomicBitset in LavaFrame
// -todo: make LavaFrame operations atomic - need an atomic bitset 
// -todo: put priority into packets 
// -todo: put a range in a packet - does this imply the generation of multiple smaller packets from one bigger packet that then have to be synced together?
// -todo: figure out a way to emit multiple packets with different ranges - with lava_emit() it should be possible to emit multiple packets that reference the same data and have different ranges - what happens if the first packet is emitted, then consumed, then the reference count is decremented to 0 before the next packet is emitteed? - nothing, because the memory won't be freed until the thread gets out of the node and loops around, so the reference count becoming 0 while the node is still running is not a problem
// -todo: define filesystem namespace by compiler and not OS
// -todo: make priority part of LavaPacket operator< 
// -todo: make a queue for output arguments that uses the thread local heap
// -todo: change project name to Fissure
// -todo: make an assert if more than one thread does a push
// -todo: make a queue that carries its allocation and deallocation functions with it as pointers so that it can be passed to a .dll
//       | -with two buffers and only one thread writing, the structure can be different -make queue use a read buffer and a write buffer - the write buffer will only be used by the thread that owns the queue, so there won't need to be a write lock
// -todo: rework lavaQ to have a cur and an end
// -todo: make read happen inside a compare_exchange loop - added without a loop, pop fails on contention 
// -todo: change project name to Fissure 
// -todo: test LavaQ
// -todo: make a template to run an arbitrary function on the StEnBuf - just copy and paste for now
// -todo: make a union that will hold the st, en and buffer boolean
// -todo: possibly make cur, end, and buffer boolean into a single struct of 31, 31, and 1 bits - buffer and capacity need to be in the same struct - can combining everything in to single atomics be avoided by using an A and B variable for each?
// -todo: change lava_theadQ to lava_outQ? lava_threadOutQ? - LavaQ is fined
// -todo: can the start and buf flag in the queue be put together while leaving the end and capacity separate?  - might need to leave start and end together so that the reading threads are always in sync with the writing thread, though maybe it is fine
// -todo: make LavaFrame slots start from the begining - not neccesary with output queue
// -todo: make deallocation happen just before it is needed
// -todo: change StEnBuf to StBuf
// -todo: make sure that different StBuf structs have different integer respresentations - integer representations are more legible with the buffer switch bit first
// -todo: redo memory allocation sequence so that the buffers are always pointing to allocated memory - should they also be atomic?
// -todo: make memory pointers into atomics
// -todo: make a custom copy function that takes in to account wrapping
// -todo: fix concurrent pop returning unitialized memory 
// -todo: clean up LavaQ
// -todo: make LavaQ::size() take into account wrapping
// -todo: make StBuf::st be incremented by the new capacity so that it wraps around to the same spot yet is a different number - have to increment m_end first? can their wrapped versions be used? - prove that buf.st can't overflow 
// -todo: work around st == en being empty and full - expand happens when buffer has one slot left, instead of no slots left
// -todo: only need to change buf.st on every other buffer flip
// -todo: make at() functions increment and decrement reference counts 
// -todo: put reference count as well as deleted flag into the pointers for memA and memB
// -todo: add reference count for each buffer
// -todo: try StBuf with 6 bits for the current capacity - if the buffer is flipped, it should go up by one - what should be done on shrink? - doesn't seem neccesary 
// -todo: use an array of reference counted buffers? - doesn't seem neccesary
// -todo: think of a way to make sure that the buffer can't be double flipped during a read - can m_end be checked to see if it hasn't increased by the capacity before the read? - if the buffers have been switched twice, that means that m_end must have been incremented by at least the current capacity, to make that happen - maybe an idea for later
// -todo: reset buf.st if it gets bigger than double capacity - not neccesary when not using extra increment tricks
// -todo: clean unused line out of LavaQ again
// -todo: test LavaQ with explicity malloc and free + thread local allocations - free needed to use the stored function pointer in m_free in 3 different places
// -todo: put capacity into StBuf, since the array index is the combination of the constantly incrementing st variable and the capacity of that buffer
//       | -change StBuf to have 6 bits for capacity
//       | -make push / expand put capacity into StBuf
//       | -make pop use the StBuf capacity
//       | -test - fix blank output
//       | -take out m_capA and m_capB
// -todo: profile LavaQ
// -todo: profile LavaQ with only one thread calling pop()
// -todo: template LavaQ
// -todo: test std::queue - about 3 times faster to only push() 100 million integers
// -todo: compile with LavaQ instead of std::queue
// -todo: integrate MakeCube into fissure to try out the new LavaQ
// -todo: make packets be emitted (lava_send() ?) instead of simply returned
// -todo: define a const LavaNodeEnd
// -todo: make template visual studio project file
// -todo: embed visual studio template and node cpp template
// -todo: make a button to create a node directory, project and cpp file
// -todo: fix vc proj xml 
// -todo: replace the node name in the cpp file with |_NAME_|
// -todo: use regex to substitute the node name into the cpp and project files 
// -todo: put |_NAME_| in for cpp file in the project file
// -todo: put |_NAME_| in for the function name in cpp file
// -todo: make slot mask have a const operator[]
// -todo: use std::array for static packet array so that it can be iterated through 
// -todo: clean up comments in LavaFlow.hpp
// -todo: visualize packets being routed - can the current queue simply be read and drawn by the interface? - seems so, though it locks for now
// -todo: expand the margin around flow nodes so that the slots don't obscure the text - need to make the node width dependant on the text bounds in the first place
// -todo: visualize packets at outputs and frames at inputs/nodes? - packets already are on slots, and the 'current node' highlights should represent a frame being run
// -todo: visualize packets being created - halos around slots for all packets in the queue

// todo: calc text size + margins before node drawing so that bounding boxes are correct (for both selection and slot drawing)
// todo: make packets visualize on slots circles stack as concentric circles or as portions/segments of a single circle 
// todo: put total outstanding packet size (in bytes) in the status bar
// todo: make a node that passes cube through
// todo: make assert to check if the mem in nullptr - make a debug function for the end of a node that has its body empty in release mode
// todo: make a node to transform the cube from MakeCube
// todo: visualize both nodes 
// todo: test LavaQ across shared library borders
// todo: make errors in the directory creation give an error in the status bar 
// todo: make list of nodes a side window, right click menu, hot box, etc
// todo: draw a number inside the slot
// todo: popup the slots string name in the status bar
// todo: draw node names after the slots so the text is on top?
// todo: change cur() functions to const and rename to read()
// todo: change opp() functions to non-const only and rename to write()
// todo: figure out a way to have a reader count with lava.graph - is an atomic hash map inevitable? 
// todo: put in read count and write count for both A and B buffers
// todo: make each variable in the graph individually double buffered or even multi-buffered according to readers?
// todo: have exec() spinlock until readers of the opposite buffer drops to 0 - could also just skip the command buffer in the rare case that it catches readers as more than 0
// todo: fix type warnings in simdb
// todo: make button that creates a project for a node - would it need to pop up a modal dialog?
// todo: convert tbl.hpp to no longer be a template - characters "u8", "iu8", "f64", for the type of array - can any heirarchy of initializer_lists be brought down to an array of the same types?
// todo: make popup text box that avoids the bounding box of the moused over node? - put graph of node times in the box? put graph of covariance data of data in, time spent, data out, and time ?   
// todo: make segmented vertical bar that shows packets building up on certain node
// todo: make right click or space bar open up a text box that can contain the build command, stats and/or hotbox
// todo: make shared libraries loaded after the GUI
// todo: make shared libraries only try to load one per frame
// todo: make nodes a little fatter or slots offset a little more from the edge
// todo: integrate type into LavaMem instead of as part of the LavaMem struct so that it can be queried externally
// todo: try to unify nanogui into a single file?
// todo: try to unify nfd into a single file ? 
// todo: make LavaHeapFree use a thread local variable for the errors instead of a return value, so that it's signature will match with free

// todo: make input slots start at 0 - does there need to be a separation between input and out slots or does there need to be an offset so that the input frame starts at 0 
// todo: convert tbl to use arrays of the data types smaller than 64 bits
// todo: profile
// todo: print child tables in table printing functions
// todo: make lava memory allocation aligned to 64 byte boundaries
// todo: come up with locking system so that message nodes have their own threads that are only run when a looping thread visits them - how should memory allocation be done? passing the thread's allocator in the exact same way?
// todo: prototype API for message nodes
//       | do message nodes need some extra way to hold their state? will there ever be more than a single instance of a message node?
//       | initially just make them thread safe or make them lock with mutexes
//       | do messages need some sort of 8 byte number to be able to do occasionally do without heap or simdb allocated values?
// todo: make a thread number UI input box
// todo: somehow draw slot names and types when slots are moused over
// todo: transition to better json support to write formatted json/.lava files?
// todo: put in error checking for connecting dest to dest or src to src?
// todo: make two nodes execute in order
// todo: make a node to read text from a file name 
// todo: make a node to split text into lines and scatter the result
// todo: make message node diameter dependant on text bounds
// todo: make flow node size dependant on text bounds
// todo: make unconnected slots not overlap
// todo: make one node snap to another node
// todo: make two snapped nodes group together and be dragged together
// todo: use scroll wheel and nanovg scale transforms to zoom in and out - will need to scale mouse pointer position as well to 'canvas' coordinates
// todo: make multiple slots avoid each other - might need to have discreet sections around a node for a slot to sit in
// todo: don't select a slot if it is under an existing node
// todo: make Lava data structures use the Lava thread local allocator
// todo: change NODE_ERROR to NODE_NONE 
// todo: keep the time in microseconds of the execution of every node instance in a LavaNodeInst struct
//       | use the execution time to draw the saturation of the node color a long with a background indicator to see the nodes with the most execution time
// todo: use combination of frame, node id and slot as key to simbdb - visualization can be done with a different key for now and node communication is through thread local memory
//       |  how does that get in to the node, if all the data is in the packet struct? - through the output Args
//       |  put the index information into the output array and use that 
//       |  leave room for the hash in the output struct? - not now
//       |  the full key is needed because robin hood hashing could shift the other keys' indices around - the index into the key value slots in the db can't be used, but the starting block index can since that won't change until the data goes away
//       |  use a union of bytes that is filled with the frame, slot, list index?
//       |  use malloc addresses initially

// idea: build guard pages into Lava heap allocations that can be set to non read or write - use these to crash if memory is read or written out of bounds - maybe do this only in debug mode
// idea: make a table creation and editing GUI
// idea: make a dialog to set a compilation command for each node - think about debug and release modes, different OSs - set the current working directory to the node directory path
// idea: when turning off a slot, take it out, delete it, and put it back under a name with 'OFF: ' as a prefix
// idea: keep track of covariance matrix for time vs size of data 
//       | keep track of multiple moments and detect if a node has a quadratic / O(n^2) time
//       | keep track of the covariance of input data, time, and other parameters to show how they affect the running time of that node?
//       | also keep track of the parameters and input size to see how they affect the data output size - does this complete the data needed to estimate how long calculations will take with a breakdown of where all the time will be spent? 
// idea: make an optional main message node that takes command line input 
// idea: make an execute count on each Node - accumulate on every frame to draw which nodes are executing
// idea: give message nodes an order than can be edited
// idea: make LavaGraph into a template and use it for the visual graph as well?
// idea: load shared libs asynchronously and show a progress bar somewhere
// idea: make command queue - use a simdb file or use the heap
// idea: try compiling nanogui into one file - depends on eigen, stb and glfw
// idea: separate finding node the pointer is inside from the action to take
// idea: make nodes snap to a grid
// idea: combine src and dest slots when writing json and just use a boolean to separate them 
// idea: have a panel or window that shows information about the selected node and the shared library it represents
// idea: make connections thicker when there is more data and brighter when there are more packets
// idea: build in focus as information separate from selection
// idea: draw message node slots as sliding angles
// idea: make connection delete and create trigger when 1 or more in/dest slots are selected and 1 out/src slot is connected
// idea: make a single file include all nanoui into one compilation unit
// idea: make msg loop that would deal with selections, clicks and drags? 
// idea: take out redudant node position + bnd information 
// idea: look into quantized gradients of nanovg
// idea: switch over to using json.hpp
// idea: make load file take the state in
// idea: make an io file
// idea: make a recently opened menu
// idea: drag json file into window to open file
// idea: draw arrows (triangles) to show direction with connections
// idea: make connection a set
// idea: slow down cursor over nodes
// idea: speed up cursor while dragging
// idea: highlight connections and slots when a node is selected
// idea: half highlight when mouse over of node, slot or connection
// idea: make snapped/grouped nodes separate with right mouse button
// idea: make mouse slow down over slots more than it slows down over nodes
// idea: make click and drag for connections
// idea: make selected indication a border effect and not a color change
// idea: make selection a vector for multi-selection - if the vector capacity is 3x the size, use reserve to shrink it down to 1.5x the size?
// idea: make connections have different shapes? draw three thin lines for a scatter connection?
// idea: make nodes different shapes? - make data input into vertical columns?
// idea: look into openGL input latency technique
// idea: convert general data structures of nodes, slots, and connections to use tbl?
// idea: make drawing order of slots dictated by node order
// idea: make connection class that keeps two connection arrays, each sorted by src or dest for fast searching
// idea: keep covariance statistics of input size, output size and time taken

// q: do nodes just need to be tagged for visualization so that they put their outputs in the viz db? - should this just be a function hook so that the LavaGraph doesn't have to deal with it? 
//    | put it in the packet queue first, then run the function callback on the LavaMem from the same frame
//    | how should it be identified?
// q: does each node need it's own memory to create a full frame?
// q: does each node slot need it's own priority queue to put frames together? 
// q: how is it possible to determine when to run a node with a full frame?
// q: should there be a section of memory for packets to be copied to? 
//    |  when a packet is pulled from the queue it could be copied to slot memory for the node
//    |  if a thread completes a frame, then it runs the node with that frame
//    |  this would possibly imply multiple frame buffers in a ring
// q: what to do about a scenario where multiple packets for a single frame are being generated, but there are non-optional arguments that will keep them from running
// q: is there any value in a queue for each node?
// q: is there any value in a multi-stage queue that takes packets out and organizes them into frames to create a frame queue?
//    | there would need to be a packet queue, a packet combining stage, then a frame queue
//    | can it be guaranteed that frames will run sequentially on a node basis? - if the queue is sorted by frame first, then node, then slot, without accounting for multiple packets to one slot, they should be roughly in order and a frame should exist in the queue roughly sequentially
//    | does there need to be a limitation that says nodes that splits can't feed into a node with framed slots?
//    | structurally does this mean that nodes would have attributes instead of one type? 
//    |  |  main issues are gather and split
//    |  |  if a node wants to gather packets but not output a single packet, would it be both a gather and a split?

// Major Features: 
//   | -Catch low level exceptions on each node
//   | -Automatic reloading of shared libs
//   | Freezing data while playing
//   | Reloading of shared libs while running 
//   | Packet syncing through use of frames
//   | Live editing of input tables 
//   | Constant input tables to configure a node? - can constant input tables just be a live table that gets frozen into the binary? would it be memory mapped and a specially tagged pointer passed around?
//   | Node graph visualizations, tests and notes treated specially with the ability to be hidden
//   | Packet queueing based on cache and memory heirarchies - look in a queue for the physical core, then whatever shares the L2 cache, L3 cache, and finally the same NUMA node - special queue structure needed that will automatically use this heirarchy by being specifically structured around it - need to weigh importance of having the absolute correct ordering versus using the memory heirarchy as effectivly as possible - maybe the memory heirarchy is the most important, but would out of order frames cause problems? possibly only if message passing nodes did not make sure to process the frames they recieve in order - would this end up being a tree structure of queues


#include "FissureStatic.cpp"

// glew might include windows.h
#define  WIN32_LEAN_AND_MEAN
#define  NOMINMAX
#include "glew_2.0.0.h"
#include "glfw3.h"
#include "nanovg.h"

#define NANOVG_GL3_IMPLEMENTATION   // Use GL2 implementation.
//#include "nanovg_gl.h"  // nanogui includes this already

#define ENTRY_DECLARATION int main(void)
#ifdef _MSC_VER
  // not needed due to having glfw and glew compiled into single files
  //#pragma comment(lib, "glfw3dll.lib")
  //#pragma comment(lib, "glew32.lib")

  #pragma comment(lib, "opengl32.lib")

  #if defined(_DEBUG)
    #define USE_CONSOLE                                 // turning this off will use the windows subsystem in the linker and change the entry point to WinMain() so that no command line/console will appear
  #endif

  #ifndef USE_CONSOLE
    #pragma comment( linker, "/subsystem:windows" )
    #undef ENTRY_DECLARATION
    #define ENTRY_DECLARATION int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
  #endif
#endif

#include <cstdio>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <thread>
#include <regex>
#include <filesystem>
#include "nfd.h"
#include "jzon.h"
#include "vec.hpp"
#include "../no_rt_util.h"
#include "../Transform.h"
#include "FissureDecl.h"

namespace fs = std::experimental::filesystem;

using vec_ids = std::vector<LavaId>;

static FisData    fd;
static simdb   fisdb;

namespace{

float               lerp(float p, float lo, float hi)
{
  return lo*(1-p) + hi*p;
}
float            linNorm(float n, float lo, float hi)
{
  return (n-lo) / (hi-lo);
}
float              remap(float n, float lo, float hi, float toLo, float toHi)
{
  float p = linNorm(n, lo, hi);
  return lerp(p, toLo, toHi);
}
bool                isIn(float x, float y, Bnd const& b)
{
  return x>b.xmn && x<b.xmx && y>b.ymn && y<b.ymx;
}
bool                isIn(Bnd const& a, Bnd const& b)
{
  return b.xmn<a.xmn && b.xmx>a.xmx && b.ymn<a.ymn && b.ymx>a.ymx;
}
int              isBlack(NVGcolor col)
{
	if( col.r == 0.0f && col.g == 0.0f && col.b == 0.0f && col.a == 0.0f )
	{
		return 1;
	}
	return 0;
}
static char*    cpToUTF8(int cp, char* str)
{
	int n = 0;
	if (cp < 0x80) n = 1;
	else if (cp < 0x800) n = 2;
	else if (cp < 0x10000) n = 3;
	else if (cp < 0x200000) n = 4;
	else if (cp < 0x4000000) n = 5;
	else if (cp <= 0x7fffffff) n = 6;
	str[n] = '\0';
	switch (n) {
	case 6: str[5] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x4000000;
	case 5: str[4] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x200000;
	case 4: str[3] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x10000;
	case 3: str[2] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x800;
	case 2: str[1] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0xc0;
	case 1: str[0] = cp;
	}
	return str;
}
bool              hasInf(v2   v)
{
  TO(2,i) if(v[i]==INFf || v[i]==-INFf) return true;
  return false;
}
bool              hasNaN(v2   v)
{
  TO(2,i) if(v[i]==SIG_NANf || v[i]==-SIG_NANf) return true;
  return false;
}
v2      lineCircleIntsct(v2   P, v2 dir, v2 crcl, f32 r)  // only works for circles to the sides
{
  if(dir.x==0) return {INFf, INFf};

  v2       st = (P - crcl) / r;
  f32     mlt = abs(st.x) / dir.x;                 // mlt = multiplier - the multiplier to get dir.x to equal st.x 
  f32       C = (st + dir*mlt).y;
  if(C > r) return v2(INFf, INFf);
  f32       m = dir.y / dir.x;
  f32       a = SQR(m) + 1;
  f32       b = 2.f * m * C;
  f32       c = SQR(C) - 1.f;
  f32      q2 = SQR(b) - 4.f*a*c;
  if(q2 < 0) return v2(INFf, INFf);
  f32       x = ((-b + sqrt(q2)) / 2.f*a);
  f32       y = sin(acos(x));
  v2  intrsct = v2( sign(dir.x)*x, sign(dir.y)*y)*r + crcl;

  return intrsct;
}
f32        normalToAngle(v2   N)
{
  return atan2(N.y, N.x);
}
v2         angleToNormal(f32 angle)
{
  return { cos(angle), sin(angle) };
}
f64        timeToSeconds(u64 t)
{
  return t / 1000000000.0;
}

str       makeStatusText(u64 nid, f64 totalTime, vec_ndptrs const& nds, u64 nIdx)
{
  using namespace std;

  f64 seconds  =  timeToSeconds(fd.lgrph.node(nid).time);
  f64 percent  =  totalTime>0?  (seconds/totalTime)*100  :  0;
  percent      =  max(100.0, percent);
  seconds      =  (int)(seconds*1000) / 1000.0;    // should clamp the seconds to 3 decimal places 
  auto status  =  toString("Node [",nid,"]  ",nds[nIdx]->txt," | ",seconds," seconds  %",percent);

  return status;
}

void         draw_radial(NVGcontext* vg, NVGcolor clr, f32 x, f32 y, f32 rad)
{
  auto  hlf = rad/2; 

  nvgFillColor(vg, nvgRGBA(0,255,255,255));
  nvgBeginPath(vg);
    nvgCircle(vg, x, y, rad);
    auto radial = nvgRadialGradient(vg,x,y,0,hlf,clr,nvgRGBA(0,0,0,0));
  nvgFillPaint(vg, radial);
  nvgFill(vg);
}

// state manipulation
u64                nxtId()
{
  return fd.nxtId++;
}
void     stopFlowThreads()
{
  fd.flow.stop();
  for(auto& t : fd.flowThreads){
    if(t.joinable()){
      t.join();
    }
  }
  fd.flowThreads.clear();
  fd.flowThreads.shrink_to_fit();

  fd.lgrph.clearTime();
}
void    startFlowThreads(u64 num=1)
{
  stopFlowThreads();
  fd.flow.start();

  TO(num,i){
    fd.flowThreads.emplace_back([](){
      //printf("\n LavaInst size: %d \n", (i32)sizeof(LavaInst) );
      //printf("\n LavaNode size: %d \n", (i32)sizeof(LavaNode) );
      //
      //printf("\n running thread \n");
      //
      //printf("\n union size: %d \n", (i32)sizeof(LavaOut::key) );
      ////printf("\n union size: %d \n", (i32)sizeof(LavaOut::key.bytes) );
      //printf("\n union size: %d \n", (i32)sizeof(LavaOut::key.slot) );
      //printf("\n union size: %d \n", (i32)sizeof(LavaOut::key.listIdx) );
      //printf("\n union size: %d \n", (i32)sizeof(LavaOut::key.frame) );
      //printf("\n packet size: %d \n", (i32)sizeof(LavaPacket) );

      //fd.flow.loop();
      LavaLoop(fd.flow);
    });
  }
}

v2               in_cntr(Node const& n, f32 r)
{
  //return v2(n.P.x + NODE_SZ.x/2, n.P.y-r);
  return v2(n.P.x + n.b.w()/2, n.P.y-r);
}
v2              out_cntr(Node const& n, f32 r)
{
  //return v2(n.P.x + NODE_SZ.x/2, n.P.y + NODE_SZ.y + r);
  return v2(n.P.x + n.b.w()/2, n.P.y + n.b.h() + r);
}

void            slot_add(bool isDest)
{
  //LavaFlowSlot ls;
  //ls.id      = s.nid;
  //ls.in      = s.in;
  //ls.state   = (LavaFlowSlot::State)(s.state);

  LavaCommand::Arg arg;
  arg.slotDest = isDest;
  fd.lgrph.put(LavaCommand::ADD_SLOT, arg);
}
void           slot_draw(NVGcontext* vg, Slot const& s, Slot::State drawState, f32 slot_rad=10.f)
{
  nvgStrokeColor(vg, nvgRGBAf(0,0,0,1.f));
  nvgStrokeWidth(vg, 3.f);

  v2     out = s.P;
  v2       N = s.N;

  NVGcolor fillClr;
  if(s.in){
    switch(drawState){
    case Slot::SELECTED:    fillClr = nvgRGBAf(1.f,   1.f,   .5f,  1.f); break;
    case Slot::HIGHLIGHTED: fillClr = nvgRGBAf( .36f,  .9f, 1.f,   1.f); break;
    case Slot::NORMAL:
    default:                fillClr = nvgRGBAf( .18f,  .6f,  .75f, 1.f); break;
    }
  }else{
    switch(drawState){
    case Slot::SELECTED:    fillClr = nvgRGBAf(1.f,   1.f,   .5f,   1.f); break;
    case Slot::HIGHLIGHTED: fillClr = nvgRGBAf( .36f, 1.f,   .36f,  1.f); break;
    case Slot::NORMAL:      
    default:                fillClr = nvgRGBAf( .18f,  .75f, .18f,  1.f); break;
    }
  }
  nvgFillColor(vg, fillClr);

  nvgBeginPath(vg);
    nvgCircle(vg, out.x, out.y, slot_rad);  //io_rad);
    nvgFill(vg);
  nvgStroke(vg);

  // slot triangle drawing
  f32  triRad = slot_rad - 2.f;
  auto inrClr = fillClr;
  inrClr.r += 0.2f;
  inrClr.g += 0.2f;
  inrClr.b += 0.2f;
  nvgFillColor(vg, inrClr);

  nvgBeginPath(vg);
    nvgResetTransform(vg);
    nvgTranslate(vg, out.x, out.y);             // translate comes before rotate here because nanovg 'premultiplies' transformations instead of multiplying them in for some reason. this reverses the order of transformations and can be seen in the source for nanovg
    nvgRotate(vg, normalToAngle(N) + (s.in? PIf/2.f : -PIf/2) );

    nvgMoveTo(vg, -0.707f*triRad, -0.707f*triRad);
    nvgLineTo(vg,  0.707f*triRad, -0.707f*triRad);
    nvgLineTo(vg,  0, triRad);
  nvgClosePath(vg);
  nvgFill(vg);
  nvgResetTransform(vg);
}
Slot*           slot_get(LavaId id)
{
  auto& slots = fd.graph.slots;

  auto si = slots.find(id);
  if(si == slots.end()) 
    return nullptr;

  return &si->second;
}

u64        node_nxtOrder()
{
  auto& nodes = fd.graph.nds;
  auto& ordrs = fd.graph.ordr;

  u64 order = 1;
  if(ordrs.size() > 0)
    order = ordrs.rbegin()->order + 1;

  return order;
}
auto        node_getPtrs() -> vec_ndptrs
{
  auto& nodes = fd.graph.nds;
  auto     sz = fd.graph.nds.size();  // nds.nsz();
  u64       i = 0;
  vec_ndptrs nds; 
  nds.reserve(sz);
  for(auto& ido : fd.graph.ordr){                     // ido is id order - an IdOrdr struct
    if(nodes.find(ido.id) != end(nodes)){
      nds.push_back( &fd.graph.nds[ido.id] );
      ++i;
    }
  }

  return nds;

  //vec_ndptrs nds(sz,nullptr);
  //
  //nds[i++] = &fd.graph.nds[ido.id];

  //for(auto& kv : fd.graph.nds){
  //  nds[i++] = &kv.second;
  //}
}
Node&   node_moveToFront(u64 idx)
{
  using namespace std;

  Node& n = fd.graph.nds[idx];
  auto nxt = node_nxtOrder();

  fd.graph.ordr.erase( {idx, n.order} );
  n.order = nxt;
  fd.graph.ordr.insert( {idx, n.order} );

  return n;
}

auto            node_add(str node_name, Node n) -> uint64_t
{
  // this effectivly adds a node to the ui graph, adds a add_node command to the LavaGraph, adds slot commands, then on exec adds the slots to the ui graph - far from ideal
  using namespace std;

  auto          pi = fd.flow.nameToPtr.find( node_name );                               // pi is pointer iterator
  uint64_t instIdx = LavaNode::NODE_ERROR;
  LavaNode*     ln = nullptr;
  if( pi != end(fd.flow.nameToPtr) )
  {
    ln = pi->second;
    if(ln)
    {
      instIdx = nxtId();

      FisData::IdOrder ido;                                                          //ido is id order
      ido.id    = instIdx;
      ido.order = node_nxtOrder();
      fd.graph.ordr.insert(ido);
      
      n.txt   = "New: " +  node_name;
      n.id    = instIdx;
      n.order = ido.order;
      fd.graph.nds[instIdx] = move(n);

      LavaCommand::Arg A,B;
      A.ndptr = ln;
      B.val   = instIdx;
      fd.lgrph.put(LavaCommand::ADD_NODE, A, B);

      auto out_types = ln->out_types;                    // do these first so that the output slots start at 0
      for(; out_types  &&  *out_types; ++out_types){ 
        slot_add(false);                                 // this will put a slot command into the LavaGraph command queue - the graph_apply() function will actually end up creating the slots in the ui graph
      }

      auto in_types = ln->in_types;
      for(; in_types  &&  *in_types; ++in_types){
        slot_add(true);
      }
    }

  }

  return instIdx;
}
Bnd             node_bnd(NVGcontext* vg, Node const&  n)
{
  f32 x=n.P.x, y=n.P.y, w=n.b.w(), h=n.b.h();
  Bnd b;
  b = { x, y, x+w, y+h };

  return b;

  //if(n.type==Node::MSG){
  //  b = { x, y, x+w, x+w };
  //}else
}
void           node_draw(NVGcontext* vg,      // drw_node is draw node
                            int preicon,
                          Node const& n,
                         float      rnd,      // rnd is corner rounding
                         bool  forceSel=false,
                         f32     border=3.5f,
                         bool highlight=false)
{
  using namespace std;

  const float   rthk = 8.f;    // rw is rail thickness

  nvgResetTransform(vg);
  nvgGlobalAlpha(vg, 1.f);
  
  float wMargin=50.f, hMargin=20.f;
	
  NVGpaint bg;
	char icon[8];
  float tw=0, iw=0, x=n.P.x, y=n.P.y, h=n.b.h(); // w=n.b.w(),

  f32 txtsz = fd.graph.textSize;
  nvgFontSize(vg, txtsz);
  nvgFontFace(vg, "sans-bold");
  tw = nvgTextBounds(vg, 0,0, n.txt.c_str(), NULL, NULL);
  float w  = max<float>(n.b.w(), tw + wMargin);

	float rad = lerp(rnd, 0.f, h/2.f);                                  // rad is corner radius
  f32 cntrX = x + w/2.f;
  f32 cntrY = y + h/2.f; 
  f32    rr = rad;                                                    // rr is rail radius
  bool  sel = forceSel || n.sel;

  switch(n.type)
  {
  case Node::Type::FLOW: {
    SECTION(draw flow node)
    {
      SECTION(grey border)
      {
	      nvgBeginPath(vg);
         nvgRoundedRect(vg, x,y,w,h, rad);
        nvgFillColor(vg, nvgRGBA(0,0,0,128));
	      nvgFill(vg);
      }
      SECTION(shaded color inside)
      {
        //auto    col = n.sel? fd.ui.nd_selclr  : fd.ui.nd_color;
        auto    col = sel? fd.ui.nd_selclr  : fd.ui.nd_color;
        int    grad = (int)lerp(rnd, 0, 48);
        auto topClr = nvgRGBA(255,255,255,isBlack(col)?16:grad);
        auto botClr = nvgRGBA(0,0,0,isBlack(col)?16:grad);
	      bg = nvgLinearGradient(vg, x,y,x,y+h, topClr, botClr);
	      nvgBeginPath(vg);
	        nvgRoundedRect(vg, x+border,y+border, w-(border*2),h-(border*2), rad-border);
          col.a = 0.8f;
	        if(!isBlack(col)){
		        nvgFillColor(vg, col);
		        nvgFill(vg);
	        }
	      nvgFillPaint(vg, bg);
	      nvgFill(vg);
      }
      //b = {x,y, x+w, y+h};
    }
  } break;
  case Node::Type::MSG: {
    SECTION(draw message node)
    {
      f32 msgRad = w/2.f;

      nvgStrokeColor(vg, fd.ui.lineClr);
      nvgStrokeWidth(vg, border);

      SECTION(linear gradient from upper left)
      {
        nvgBeginPath(vg);
        nvgCircle(vg, cntrX, cntrY, msgRad);
        auto lin = nvgLinearGradient(vg, 
          cntrX, cntrY-msgRad, x, y+msgRad,
          sel? fd.ui.msgnd_selclr : fd.ui.msgnd_gradst,
          sel? fd.ui.msgnd_selclr : fd.ui.msgnd_graden );
        nvgFillPaint(vg, lin);
        nvgFill(vg);
      }
      SECTION(radial gradient)
      {
        nvgBeginPath(vg);
        nvgCircle(vg, cntrX, cntrY, msgRad);
        auto radial = nvgRadialGradient(vg,
          cntrX, cntrY, msgRad*.5f, msgRad,
          fd.ui.msgnd_gradst,
          fd.ui.msgnd_graden  );
        nvgFillPaint(vg, radial);
        nvgFill(vg);

        if(highlight){
          nvgBeginPath(vg);
          nvgCircle(vg, cntrX, cntrY, msgRad);
          auto radial = nvgRadialGradient(vg,
            cntrX, cntrY, msgRad*.5f, msgRad,
            fd.ui.msgnd_gradst,
            fd.ui.nd_hilgt );
            //fd.ui.msgnd_graden  );
          nvgFillPaint(vg, radial);
          nvgFill(vg);
        }
      }
      SECTION(circle outline)
      {
        nvgBeginPath(vg);
          nvgCircle(vg, cntrX, cntrY, msgRad);
        nvgStroke(vg);
      }

    }
  } break;
  }

  SECTION(text)
  {
   // f32 txtsz = fd.graph.textSize;
	  //nvgFontSize(vg, txtsz);
	  //nvgFontFace(vg, "sans-bold");
	  //tw = nvgTextBounds(vg, 0,0, n.txt.c_str(), NULL, NULL);
	  if(preicon != 0){
		  //nvgFontSize(vg, h*1.3f);
      nvgFontSize(vg, h);
		  nvgFontFace(vg, "icons");
		  iw = nvgTextBounds(vg, 0,0, cpToUTF8(preicon,icon), NULL, NULL);
		  //iw += h*0.15f;
	  }

    f32 txtX = x+w*0.5f - tw*0.5f + iw*0.25f;   // + fd.ui.slot_rad;
    f32 txtY = y + h*0.5f;
	  nvgFontSize(vg, txtsz);
	  nvgFontFace(vg, "sans-bold");
	  nvgTextAlign(vg, NVG_ALIGN_MIDDLE);  // NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
	  nvgFillColor(vg, nvgRGBA(0,0,0,160));
    nvgText(vg, txtX, txtY-1, n.txt.c_str(), NULL);
	  nvgFillColor(vg, nvgRGBA(255,255,255,255));
    nvgText(vg, txtX, txtY, n.txt.c_str(), NULL);
  }
  SECTION(debug circle sides and bnd)
  {
    //nvgStrokeColor(vg, nvgRGBAf(1,1,1,1) );
    //nvgStrokeWidth(vg, 1.f);
    //
    //nvgBeginPath(vg);
    //  nvgRect(vg, b.xmn, b.ymn, b.w(), b.h());
    //nvgStroke(vg);
    //
    //nvgBeginPath(vg);
    // nvgMoveTo(vg, n.P.x+rad,n.P.y);
    // //nvgLineTo(vg, n.P.x+rad,n.P.y + NODE_SZ.y);
    // nvgLineTo(vg, n.P.x+rad,n.P.y + h);
    //nvgStroke(vg);
    //
    //nvgBeginPath(vg);
    //  //nvgMoveTo(vg, n.P.x+NODE_SZ.x-rad, n.P.y);
    //  //nvgLineTo(vg, n.P.x+NODE_SZ.x-rad, n.P.y + NODE_SZ.y);
    //  nvgMoveTo(vg, n.P.x + w - rad, n.P.y);
    //  nvgLineTo(vg, n.P.x + w - rad, n.P.y + h);
    //nvgStroke(vg);
    //
    //nvgBeginPath(vg);
    //  nvgCircle(vg, n.P.x+rad, n.P.y+rad, rad);
    //nvgStroke(vg);
    //
    //nvgBeginPath(vg);
    //  //nvgCircle(vg, n.P.x+NODE_SZ.x-rad, n.P.y+NODE_SZ.y-rad, rad);
    //  nvgCircle(vg, n.P.x + w - rad, n.P.y + h - rad, rad);
    //nvgStroke(vg);
  }

  //return;
}
v2           node_border(Node const& n, v2 dir, v2* out_nrml=nullptr)
{
  v2       wh = n.b.wh();
  v2      hlf = wh / 2;
  v2       nP = n.P;
  v2 borderPt = {0,0};
  v2     ndir = norm(dir);
  v2    ncntr = nP + hlf;      // n.b.mx/2.f; // hlf; // n.mx/2.f; // NODE_HALF_SZ;

  switch(n.type)
  {
  case Node::Type::FLOW: {
    f32  rad  =  hlf.y;
    v2  pdir  =  ndir;
    v2    ds  =  sign(pdir);                                  // ds is direction sign
    f32   ax  =  abs(pdir.x);
    if( ax > hlf.x ){
      pdir /= ax/hlf.x;                                       // can this never be 0, since ax is positive, hlf.x is positive, and ax is greater than hlf.x ? 
    }else{
      f32 ay = abs(pdir.y);
      pdir /= ay==0.f?  1.f  :  ay/hlf.y;
    }

    v2  circCntr = (pdir.x<0)? nP+v2(rad,rad)  :  nP + wh - v2(rad,rad);
    v2   intrsct = lineCircleIntsct(ncntr, pdir, circCntr, rad);
    bool     hit = !hasInf(intrsct)  &&  (intrsct.x < nP.x+rad || intrsct.x > nP.x + wh.x - rad); 
    if(hit){ pdir = intrsct - ncntr; }

    borderPt = ncntr + pdir;

    if(out_nrml){
      if(hit) *out_nrml = norm(borderPt - circCntr);
      else    *out_nrml = { 0.f, sign(ndir).y };
    }
  } break;
  case Node::Type::MSG: 
  default: 
  {
    f32  rad = wh.x/2;
    borderPt = ncntr + ndir*rad; 
    if(out_nrml){ *out_nrml = ndir; }
  } break;
  }

  return borderPt;
}
auto          node_slots(u64 nid) -> decltype(fd.graph.slots.begin())
{
  using namespace std;
  //return lower_bound(ALL(fd.graph.slots), Id(nid), [](auto a,auto b){ return a.first < b; } );
  return lower_bound(ALL(fd.graph.slots), nid, [](auto a,auto b){ return a.first.nid < b; } );
}
auto          node_slots(vec_ndptrs const& nds) -> vec_ids
{
  using namespace std;

  auto& slots = fd.graph.slots;
  vec_ids sidxs;                                            // sidxs is slot indexes
  for(auto np : nds){                                       // np is node pointer and nds is nodes
    //auto si = lower_bound(ALL(slots), Id(np->id), [](auto a,auto b){ return a.first < b; } );          // si is slot iterator
    
    auto si = node_slots(np->id);    
    for(; si != end(slots)  &&  si->first.nid==np->id; ++si){
      Slot& s = si->second;
      sidxs.push_back(si->first);     
    }
  }
  return sidxs;                                        // RVO
}
void         node_delete()
{
  
}
void    node_draw_radial(Node const& n, NVGcontext* vg, NVGcolor clr)
{
  auto    w = n.b.w();
  auto    h = n.b.h();
  auto   cx = n.P.x + w/2;
  auto   cy = n.P.y + h/2;

  draw_radial(vg, clr, cx, cy, w*1.25f);
}

void           cnct_draw(NVGcontext* vg, v2 srcP, v2 destP, v2 srcN, v2 destN, f32 minCenterDist=INFf)
{ 
  using namespace std;
  
  f32  halfx = lerp(.5f, srcP.x, destP.x);
  f32  halfy = lerp(.5f, srcP.y, destP.y);
  f32   dist = len(srcP - destP);
  v2  outNxt = srcP  + srcN  * min(minCenterDist, (dist/3));         // divide by 3 because there are 3 sections to the bezier
  v2   inNxt = destP + destN * min(minCenterDist, (dist/3));

  nvgBeginPath(vg);
    nvgMoveTo(vg,   srcP.x, srcP.y);
    nvgBezierTo(vg, outNxt.x,outNxt.y, inNxt.x,inNxt.y, destP.x,destP.y);
    nvgStrokeWidth(vg, 3.f);
    nvgStrokeColor(vg, nvgRGBAf(.7f, 1.f, .9f, .5f));
  nvgStroke(vg);
}

auto           sel_nodes() -> vec_ndptrs
{
  vec_ndptrs nds;                                      // nids is node ids
                                                       //for(auto& on : m_nodes){                           // on is order and node - order is on.first    node is on.second
  for(auto& on : fd.graph.nds){                        // on is order and node - order is on.first    node is on.second
    if(on.second.sel) nds.push_back(&on.second);
  }

  auto& nodes = fd.graph.nds;
  if(fd.sel.pri != LavaNode::NODE_ERROR && 
    fd.sel.pri != LavaId::NODE_NONE)
  { 
    if(nodes.find(fd.sel.pri) != end(nodes))
      nds.push_back( &fd.graph.nds[fd.sel.pri] );
  }

  return nds;                                          // counting on RVO (return value optimization) here
}
u64           sel_delete()
{
  using namespace std;

  u64    cnt = 0;
  auto   nds = sel_nodes();           // accumulate nodes
  auto   ids = node_slots(nds);       // accumulate dest slots  // accumulate slots
  
  for(auto id : ids){                 // delete cncts with dest slots
    auto s = slot_get(id);
    if(s){
      if(s->in) fd.lgrph.delDestCnct(id);  //){ ++cnt; }
      else      fd.lgrph.delSrcCncts(id);
    }
  }

  // delete slots
  for(auto id : ids){                  // are LavaGraph slots deleted when deleting their node?
    fd.graph.slots.erase(id);
  }

  // delete nodes
  for(auto n : nds){  // does deleting from the graph.nds unordered map invalidate the pointers? - standard says no - how is memory reclaimed? - rehash()
    //m_ids.erase(n->id);
    auto  nid = n->id;
    auto ordr = n->order;
    fd.graph.ordr.erase({nid, ordr});
    fd.graph.nds.erase(nid);
    fd.lgrph.delNode(nid);
  }
  fd.graph.nds.reserve( fd.graph.nds.size() * 2 );

  return cnt;

  //for(auto sidx : sidxs){ fd.graph.slots.erase(sidx); }
  //
  //auto sidxs = node_slots(nds);       // accumulate dest slots  // accumulate slots
  //
  //for(auto sidx : sidxs){             // delete cncts with dest slots
  //
  //auto s = slot_get(sidx);
  //
  //if(s->in) fd.lgrph.delDestCnct(sidx);  //){ ++cnt; }
  //else      fd.lgrph.delSrcCncts(sidx);
}
void           sel_clear()
{
  fd.sel.slotOutSel = LavaId();
  fd.sel.slotInSel  = LavaId();
  fd.sel.pri        = LavaNode::NODE_ERROR;
  fd.sel.sec        = LavaNode::NODE_ERROR;

  for(auto& kv : fd.graph.nds){
    kv.second.sel = false;
  }

  for(auto& kv : fd.graph.slots){
    kv.second.state = Slot::NORMAL;
  }
}
void      sel_clearSlots()
{
  fd.sel.slotInSel.sidx  = LavaId::SLOT_NONE; 
  fd.sel.slotOutSel.sidx = LavaId::SLOT_NONE;

  for(auto& kv : fd.graph.slots){
    kv.second.state = Slot::NORMAL;
  }
}

void         graph_clear()
{
  fd.graph.nds.clear();
  fd.graph.slots.clear();
  fd.graph.ordr.clear();

  fd.lgrph.clear();
}
void         graph_apply(LavaGraph::ArgVec args)
{
  using namespace std;
  //auto ni = fd.graph.nds.find(a.id.nid);

  for(auto& a : args){
    if(a.id.sidx == LavaId::SLOT_NONE && a.id.nid != LavaId::NODE_NONE)
    {
      //LavaInst li = fd.lgrph.node(a.id.nid);
      //
      //FisData::IdOrder ido;                                                          //ido is id order
      //ido.id    = a.id.nid;
      //ido.order = node_nxtOrder();
      //fd.graph.ordr.insert(ido);
      //
      //Node n = move(fd.graph.nds[a.id.nid]);
      //
      //if(n.txt=="") { n.txt = "New: " + str(li.node->name); }
      //n.id    = a.id.nid;
      //n.order = ido.order;
      //
      //fd.graph.nds[a.id.nid] = move(n);

      //n.txt   = "New: " +  node_name;
      //n.id    = instIdx;
      //n.order = ido.order;
      //fd.graph.nds[instIdx] = move(n);
    }else if(a.id.sidx != LavaId::SLOT_NONE){
      LavaFlowSlot* ls = fd.lgrph.slot(a.id);
      if(ls){
        Slot s(a.id.nid,ls->in);
        //LavaId sid = a.id;
        fd.graph.slots.insert({a.id, s});
      }
    }
  }
}

// serialize to and from json - put in FisTfm.hpp file?
void    normalizeIndices()
{
  using namespace std;
  
  auto   nmap = fd.lgrph.normalizeIndices();                              // nmap is normalization map - this contains a map of previous indices to new indices
  //auto& nodes = fd.graph.nds;

  decltype(fd.graph.nds) nxtNds;
  for(auto& kv : fd.graph.nds){
    auto nxtId = nmap[kv.first];
    nxtNds[nxtId] = move(kv.second);
  }
  fd.graph.nds = move(nxtNds);

  decltype(fd.graph.slots) nxtSlots;
  for(auto& kv : fd.graph.slots){
    LavaId    nxtId = kv.first;
    nxtId.nid       = nmap[nxtId.nid];
    //auto      nxtId = nmap[kv.first.nid];
    nxtSlots.insert({ nxtId, move(kv.second) });
  }
  fd.graph.slots = move(nxtSlots);

  decltype(fd.graph.ordr) nxtOrdr;
  for(auto& o : fd.graph.ordr){
    auto nxtId = nmap[o.id];
    nxtOrdr.insert( {nxtId, o.order} );
  }
  fd.graph.ordr = move(nxtOrdr);


  //for(auto const& kv : nmap){
  //}
}
str           graphToStr(LavaGraph const& lg)
{
  using namespace std;

  Jzon::Node jnodes = Jzon::object();
  SECTION(nodes)
  {
    auto     nps = node_getPtrs();     // fg.nds;
    auto    lnds = lg.nodes();         // lnds is Lava Nodes
    auto      sz = lnds.size();

    //auto  sz  = lg.nsz();
    //TO(sz,i) nd_func.add(lnds[i].nd->name);

    Jzon::Node  nd_func = Jzon::array();
    TO(sz,i) nd_func.add( lg.node(nps[i]->id).node->name );

    Jzon::Node    nd_id = Jzon::array();
    TO(sz,i) nd_id.add(nps[i]->id);

    Jzon::Node   nd_txt = Jzon::array();
    TO(sz,i) nd_txt.add(nps[i]->txt);

    Jzon::Node     nd_x = Jzon::array();
    TO(sz,i) nd_x.add(nps[i]->P.x);

    Jzon::Node     nd_y = Jzon::array();
    TO(sz,i) nd_y.add(nps[i]->P.y);

    Jzon::Node nd_type = Jzon::array();
    TO(sz,i) nd_type.add(nps[i]->type);

    jnodes.add("function", nd_func);
    jnodes.add("id",         nd_id);
    jnodes.add("txt",       nd_txt);
    jnodes.add("x",           nd_x);
    jnodes.add("y",           nd_y);
    jnodes.add("type",     nd_type);
  }
  //Jzon::Node jslots = Jzon::object();
  //SECTION(slots)
  //{
  //  Jzon::Node srcId   = Jzon::array();
  //  Jzon::Node srcIdx  = Jzon::array();
  //  Jzon::Node destId  = Jzon::array();
  //  Jzon::Node destIdx = Jzon::array();
  //
  //  //for(auto kv : g.slots()){
  //  //for(auto kv : lg.slots())
  //  for(auto const& kv : fd.graph.slots)
  //  {
  //    //GraphDB::Id  sid = kv.first;
  //    LavaId sid = kv.first;
  //    auto const& s = kv.second;
  //    if(s.in){
  //      destId.add(sid.nid);
  //      destIdx.add(sid.sidx);
  //    }else{
  //      srcId.add(sid.nid);
  //      srcIdx.add(sid.sidx);
  //    }
  //  }
  //
  //  jslots.add("destId",   destId);
  //  jslots.add("destIdx", destIdx);
  //  jslots.add("srcId",     srcId);
  //  jslots.add("srcIdx",   srcIdx);
  //}
  Jzon::Node jcncts = Jzon::object();
  SECTION(connections)
  {
    //auto sz = g.cnctsz();
    auto sz = lg.cnctsz();

    Jzon::Node srcId   = Jzon::array();
    Jzon::Node srcIdx  = Jzon::array();
    Jzon::Node destId  = Jzon::array();
    Jzon::Node destIdx = Jzon::array();

    //for(auto kv : g.cncts()){
    for(auto const& kv : lg.cncts()){
      destId.add(kv.first.nid);
      destIdx.add(kv.first.sidx);
      srcId.add(kv.second.nid);
      srcIdx.add(kv.second.sidx);
    }

    jcncts.add("destId",   destId);
    jcncts.add("destIdx", destIdx);
    jcncts.add("srcId",     srcId);
    jcncts.add("srcIdx",   srcIdx);
  }

  Jzon::Node graph = Jzon::object();
  graph.add("nodes", jnodes);
  //graph.add("slots", jslots);
  graph.add("connections", jcncts);

  //Jzon::Format format;
  //format.newline    = true;
  //format.indentSize = 1;
  //format.spacing    = true;
  //format.useTabs    = false;

  Jzon::Writer w;
  //w.setFormat(format);  // breaks reading for some reason
  str s;
  w.writeString(graph, s);

  return s;
}
void          strToGraph(str const& s)
{
  using namespace std;

  //GraphDB g;
  //fd.lgrph.clear();
  //LavaGraph lg;

  graph_clear();

  Jzon::Parser prs;
  auto graph = prs.parseString(s);

  auto nd_func    = graph.get("nodes").get("function");
  auto nd_id      = graph.get("nodes").get("id");
  auto nd_txt     = graph.get("nodes").get("txt");
  auto nd_x       = graph.get("nodes").get("x");
  auto nd_y       = graph.get("nodes").get("y");
  auto nd_type    = graph.get("nodes").get("type");
  auto ordr       = graph.get("nodes").get("order");
  auto destId     = graph.get("connections").get("destId");
  auto destIdx    = graph.get("connections").get("destIdx");
  auto srcId      = graph.get("connections").get("srcId");
  auto srcIdx     = graph.get("connections").get("srcIdx");
  auto sltDestId  = graph.get("slots").get("destId");
  auto sltDestIdx = graph.get("slots").get("destIdx");
  auto sltSrcId   = graph.get("slots").get("srcId");
  auto sltSrcIdx  = graph.get("slots").get("srcIdx");

  auto cnt = nd_id.getCount();
  TO(cnt,i){        
    Node n;
    n.id   = nd_id.get(i).toInt();
    n.txt  = nd_txt.get(i).toString();
    n.P.x  = nd_x.get(i).toFloat();
    n.P.y  = nd_y.get(i).toFloat();
    n.type = (Node::Type)nd_type.get(i).toInt();

    node_add( nd_func.get(i).toString(), n);
  }

  //TO(sltSrcId.getCount(),i){
  //  Id sltId;
  //  sltId.nid  = sltSrcId.get(i).toInt();
  //  sltId.sidx = sltSrcIdx.get(i).toInt();
  //  Slot s(sltId.nid,false);
  //
  //  //g.addSlot(s, sltId.sidx);
  //  slot_add(s);
  //}
  //TO(sltDestId.getCount(),i){
  //  Id sltId;
  //  sltId.nid  = sltDestId.get(i).toInt();
  //  sltId.sidx = sltDestIdx.get(i).toInt();
  //  Slot s(sltId.nid,true);
  //  
  //  //g.addSlot(s, sltId.sidx);
  //  slot_add(s);
  //}

  auto cnct_cnt = destId.getCount();
  TO(cnct_cnt,i){
    //GraphDB::Id src, dest;
    LavaId src, dest;
    src.nid   = srcId.get(i).toInt();
    src.sidx  = srcIdx.get(i).toInt();
    dest.nid  = destId.get(i).toInt();
    dest.sidx = destIdx.get(i).toInt();

    //g.addCnct(src, dest);
    //g.toggleCnct(src, dest);
    fd.lgrph.toggleCnct(src, dest);
  }

  //fd.lgrph.setNextNodeId( fd.lgrph.maxId() );

  //return move(fd.lgrph);
}
bool            saveFile(LavaGraph const& lg, str path)
{
  stopFlowThreads();

  str fileStr = graphToStr(lg);

  FILE* f = fopen(path.c_str(), "w");
  if(!f) return false;

  size_t writeSz = fwrite(fileStr.c_str(), 1, fileStr.size(), f);
  if(writeSz != fileStr.size()) return false;

  int closeRet = fclose(f);
  if(closeRet == EOF) return false;

  //startFlowThreads(1);

  return true;
}
bool            loadFile(str path, LavaGraph* out_g)
{
  stopFlowThreads();

  FILE* f = fopen(path.c_str(), "r");
  if(!f) return false;
  fseek(f, 0, SEEK_END);
  str s;
  s.resize( ftell(f) );
  fseek(f, 0, SEEK_SET);

  size_t redRet = fread( (void*)s.data(), 1, s.size(), f);
  if(redRet != s.size()) return false; 

  if(fclose(f) == EOF) return false;

   //*out_g = strToGraph(s);
  strToGraph(s);

  //startFlowThreads(1);

  return true;
}
bool    reloadSharedLibs()
{
  bool newlibs = RefreshFlowLibs(fd.flow);

  if(!newlibs) return false;

  fd.ui.ndBtns.clear();                                          // delete interface buttons from the nanogui window

  // redo interface node buttons
  for(auto& kv : fd.flow.flow){
    LavaNode*     fn = kv.second;                                // fn is flow node
    auto       ndBtn = new Button(fd.ui.keyWin, fn->name);
    ndBtn->setCallback([fn](){ 
      node_add(fn->name, Node(fn->name, (Node::Type)((u64)fn->node_type), {100,100}) );
    });
  }
  fd.ui.screen.performLayout();

  return true;
}

void               keyCallback(GLFWwindow* win,    int key, int scancode, int action, int modbits)
{
  if(action==GLFW_RELEASE) return;

  //char sngl[256]; // = {'\0', '\0'};
  //memset(sngl, 0, 256);
  //sngl[0] = key;
  //glfwSetWindowTitle(win, sngl);

  switch(key){
  case 'J':{
  }break;
  case 'K':{
  }break;
  case 'L':{
    reloadSharedLibs();
  }break;
  case 'Y':{
  }break;
  case GLFW_KEY_BACKSPACE:
  case GLFW_KEY_DELETE: {
    sel_delete();
  }break;
  default:
    ;
  }

  //if(key==GLFW_KEY_BACKSPACE || key==GLFW_KEY_DELETE){
  //  //fd.grph.delSelected();
  //  sel_delete();
  //}

  fd.ui.screen.keyCallbackEvent(key, scancode, action, modbits);
}
void          mouseBtnCallback(GLFWwindow* window, int button, int action, int mods)
{
  if(button==GLFW_MOUSE_BUTTON_LEFT){
    if(action==GLFW_PRESS) fd.mouse.lftDn = true;
    else if(action==GLFW_RELEASE) fd.mouse.lftDn = false;
  }

  if(button==GLFW_MOUSE_BUTTON_RIGHT){
    if(action==GLFW_PRESS){ fd.mouse.rtDn = true; }
    else if(action==GLFW_RELEASE){ fd.mouse.rtDn = false; }
  }

  fd.ui.screen.mouseButtonCallbackEvent(button, action, mods);
}
void             errorCallback(int e, const char *d) {
  printf("Error %d: %s\n", e, d);
  fflush(stdout);
}
void            scrollCallback(GLFWwindow* window, double xofst, double yofst)
{
  using namespace std;
  fd.ui.screen.scrollCallbackEvent(xofst, yofst);
}
void         cursorPosCallback(GLFWwindow* window, double x, double y)
{
  const static float _2PI = 2.f* PIf; //  pi<float>();
  fd.ui.screen.cursorPosCallbackEvent(x,y);
}
void              charCallback(GLFWwindow* window, unsigned int codepoint)
{
  fd.ui.screen.charCallbackEvent(codepoint);
}
void              dropCallback(GLFWwindow* window, int count, const char** filenames)
{
  FisData* fd = (FisData*)glfwGetWindowUserPointer(window);

  fd->ui.screen.dropCallbackEvent(count, filenames);
}
void   framebufferSizeCallback(GLFWwindow* window, int w, int h)
{
  fd.ui.screen.resizeCallbackEvent(w, h);
}

str                   genDbKey(LavaId     sid)            // genDbKey is generate database key, sid is slot Id
{
  auto ni = fd.graph.nds.find(sid.nid); // todo: this is called from the lava looping threads and would need to be thread safe - it is also only used for getting text labels, which may make things easier
  if(ni == end(fd.graph.nds)) return ""; 

  auto label  =  toString("[",sid.nid,":",sid.sidx,"] ",ni->second.txt);

  return label;
}
void        lavaPacketCallback(LavaPacket pkt)
{
  LavaId id(pkt.src_node, pkt.src_slot);
  if( fd.vizIds.has(id.asInt) ){
    auto label  =  genDbKey(id);
    auto    lm  =  LavaMem::fromDataAddr(pkt.msg.val.value);
    bool    ok  =  fisdb.put(label.data(), label.size(), lm.data(), lm.sizeBytes() );
  }
}

void              debug_coords(v2 a)
{
  char  winTitle[TITLE_MAX_LEN];  // does glfw copy this string or just use the pointer? looks like it converts to a different character format which copies it / transforms it

  sprintf(winTitle, "%.2f  %.2f", a.x, a.y);
  glfwSetWindowTitle(fd.win, winTitle);
}

} // end namespace

//void print_flf_map(flf_map mp)
//{
//  flf_map::Header* hd = mp.header();
//
//  Println("Header | typechar1: ", (char)hd->typeChar1, 
//    " typechar2: ",         (char)hd->typeChar2, 
//    " sizeBytes: ",    hd->sizeBytes, 
//    " size: ",         hd->size, 
//    " valSizeBytes: ", hd->valSizeBytes );
//
//    u64    cap = mp.capacity();
//    
//    auto slots = mp.slotPtr();
//    Print("Slots: ");
//    TO(cap,i){
//      auto idx = slots[i];
//      Print(idx.readers, ",");
//      if(idx.val_idx==flf_map::DELETED)
//        Print("DELETED ");
//      else if(idx.val_idx==flf_map::EMPTY)
//        Print("EMPTY ");
//      else 
//        Print(idx.val_idx, " ");
//    }
//    Println("\n");
//
//    u32* lstSt = mp.listStart(cap);
//    Print("List: ");
//    TO(cap,i){ 
//      if(lstSt[i]==flf_map::LIST_END)
//        Print("LIST_END");
//      else 
//        Print(lstSt[i], " "); 
//    }
//}

void PrintAB(LavaQ<int>& q, str label="")
{
  //TO(q.capA(),i) sA += toString(q.atA(i)," ");
  //while(i < q.capA()){
  //
  //TO(q.capB(),i) sB += toString(q.atB(i)," ");
  //while(i < q.capB()){

  str sA = "A:  ";
  int i=0;
  auto  buf = q.m_buf;
  auto capA =  buf.useA? LavaQ<int>::Capacity(buf.cap) : LavaQ<int>::Capacity(buf.cap-1);
  auto capB = !buf.useA? LavaQ<int>::Capacity(buf.cap) : LavaQ<int>::Capacity(buf.cap-1);

  if(q.m_memA.addr() != nullptr)
    while(i < capA ){
      sA += toString(q.atA(i)," ");
      ++i;
    }

  str sB = "\nB:  ";
  i=0;
  if(q.m_memB.addr() != nullptr)
    while(i < capB){
      sB += toString(q.atB(i)," ");
      ++i;
    }

  Println(label,":\n",sA,"\n",sB,"\n");
}

ENTRY_DECLARATION // main or winmain
{
  using namespace std;
  using namespace fs;
  
  SECTION(LavaQ test)
  {
    //Println("sizeof(LavaQ::StEnBuf): ", sizeof(LavaQ<int>::StBuf));
    //LavaQ<int>::StBuf a, b;
    //a.st   = 0;
    //a.useA = 1;
    //b.st   = 2000;
    //b.useA = 1;
    //Println(a.asInt,"   ",b.asInt);

    //LavaHeapInit();

    //Println("\n\n");
    ////LavaQ q(malloc, free);
    //queue<int> stdQ;
    //LavaQ<int> q(LavaHeapAlloc, LavaHeapFree);
    //bool running = true; 
    //vector<thread> qthrds;
    //TO(1,i)
    //{
    //  qthrds.emplace_back([i, &q, &running](){
    //    while( running )
    //    {
    //      int val;
    //      bool ok = q.pop(val);
    //      if(ok){
    //        //PrintAB(q, toString("thread ",i) );
    //        //Println(i,": ",val,"\n");
    //        //assert(val > 0);
    //      }
    //      //this_thread::sleep_for( 0ms );
    //    }
    //  });
    //}
    //TO(10,i){
    //  q.push(i);

    //  //stdQ.push(i);

    //  //PrintAB(q);
    //  //Println();
    //}

    //Println("left over size: ", q.size());

    //PrintAB(q, "Main Thread");

    //while( q.size() > 0 )
    //  this_thread::sleep_for( 0ms );

    //running = false;
    //for(auto& t : qthrds){
    //  t.join();
    //}

    //int val = 0;
    //while( q.size() > 0 ){
    //  bool ok = q.pop( val );
    //  Print(val," ");
    //}

    //Println("\n\n");
  }

	NVGcontext* vg = NULL;
  SECTION(initialization)
  {
    SECTION(init glfw)
    {
      //glfwSetErrorCallback(errorCallback);
      if( !glfwInit() ){
        fprintf(stdout, "[GFLW] failed to init!\n");
        exit(1);
      }

      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
      glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
      glfwWindowHint(GLFW_SAMPLES, 16);
      //glfwWindowHint(GLFW_DECORATED, GL_FALSE);

      fd.win = glfwCreateWindow(1024, 768, "Fissure", NULL, NULL);        // assert(win!=nullptr);
      glfwMakeContextCurrent(fd.win);

      glfwSetKeyCallback(fd.win,                keyCallback);
      glfwSetMouseButtonCallback(fd.win,   mouseBtnCallback);
      glfwSetCharCallback(fd.win,              charCallback);          // in glfw charCallback is for typing letters and is different than the keyCallback for keys like backspace 
      glfwSetKeyCallback(fd.win,                keyCallback);
      glfwSetScrollCallback(fd.win,          scrollCallback);
      glfwSetCursorPosCallback(fd.win,    cursorPosCallback);
      glfwSetMouseButtonCallback(fd.win,   mouseBtnCallback);
      glfwSetDropCallback(fd.win,              dropCallback);
      glfwSetFramebufferSizeCallback(fd.win, framebufferSizeCallback);

      glfwSetWindowUserPointer(fd.win, &fd.ui.screen);

      #ifdef _WIN32
        //GLFWimage images[2];
        //images[0] = LoadIcon("lava.jpg");
        //images[1] = LoadIcon("lava.jpg");
        //glfwSetWindowIcon(win, 2, images);
      #endif

      glfwSwapInterval(0); // turning this to 1 clamps the programs to the vertical sync rate and slows down the interactivity
    }
    SECTION(init glew)
    {
      //glewExperimental = 1;
      if(glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to setup GLEW\n");
        exit(1);
      }
    }
    SECTION(nanogui)
    {
      fd.ui.screen.initialize(fd.win, false);

      fd.ui.keyLay   = new BoxLayout(Orientation::Horizontal, Alignment::Fill, 0,10);      //fd.ui.keyLay   = new BoxLayout(Orientation::Vertical, Alignment::Fill, 0,10);
      fd.ui.keyWin   = new Window(&fd.ui.screen,    "");
      auto spcr1     = new  Label(fd.ui.keyWin,     "");
      auto spcr2     = new  Label(fd.ui.keyWin,     "");
      auto loadBtn   = new Button(fd.ui.keyWin,      "Load");
      auto saveBtn   = new Button(fd.ui.keyWin,      "Save");
      auto playBtn   = new Button(fd.ui.keyWin,    "Play >");
      auto pauseBtn  = new Button(fd.ui.keyWin,  "Pause ||");
      auto stopBtn   = new Button(fd.ui.keyWin,  "Stop |_|");
      auto nodeBtn   = new Button(fd.ui.keyWin,  "Create Node");
      auto nodeTxt   = new TextBox(fd.ui.keyWin,  "");

      nodeTxt->setEditable(true);
      nodeTxt->setFixedWidth(250);
      nodeTxt->setAlignment(TextBox::Alignment::Left);
      fd.ui.keyWin->setLayout(fd.ui.keyLay);

      playBtn->setBackgroundColor(  Color(e3f(.15f, .2f,  .15f)) ); 
      pauseBtn->setBackgroundColor( Color(e3f(.2f,  .2f,  .15f)) ); 
      stopBtn->setBackgroundColor(  Color(e3f(.19f, .16f, .17f)) ); 

      loadBtn->setCallback([](){ 
        //stopFlowThreads();

        nfdchar_t *inPath = NULL;
        nfdresult_t result = NFD_OpenDialog("lava", NULL, &inPath );

        //printf("\n\nfile dialog: %d %s \n\n", result, inPath);

        if(inPath){
          //bool ok = loadFile(inPath, &fd.grph);
          bool ok = loadFile(inPath, &fd.lgrph);
          if(ok) printf("\nFile loaded from %s\n", inPath);
          else   printf("\nLoad did not read successfully from %s\n", inPath);
        }
      });
      saveBtn->setCallback([](){
        //fd.grph.normalizeIndices();

        nfdchar_t *outPath = NULL;
        nfdresult_t result = NFD_SaveDialog("lava", NULL, &outPath );
        //printf("\n\nfile dialog: %d %s \n\n", result, outPath);
        if(outPath){
          //stopFlowThreads();
          normalizeIndices(); // todo: put this into the save function
          bool ok = saveFile(fd.lgrph, outPath);
          if(ok) printf("\nFile Written to %s\n", outPath);
          else   printf("\nSave did not write successfully to %s\n", outPath);
        }
      });
      playBtn->setCallback([playBtn](){
        playBtn->setEnabled(false);
        startFlowThreads(1);
      });
      pauseBtn->setCallback([](){
        stopFlowThreads();
      });
      stopBtn->setCallback([playBtn](){
        stopFlowThreads();
        playBtn->setEnabled(true);
      });
      nodeBtn->setCallback([nodeTxt]()
      {
        Println("nodeTxt: ", nodeTxt->value());

        regex whiteSpace("[ |\t]+");
        regex whiteSpaceTrail("[ |\t]+$");

        str nodeName = nodeTxt->value();
        nodeName = regex_replace( nodeName, whiteSpaceTrail, ""); 
        nodeName = regex_replace( nodeName, whiteSpace, "_"); 

        nodeTxt->setValue( nodeName );

        str     nodeDir = "../" + nodeName;
        create_directory(nodeDir);
        str      cppPth = nodeDir+"/"+nodeName+".cpp";
        str   vcprojPth = nodeDir+"/"+nodeName+".vcxproj";
        //str      cppPth = "../"+nodeName+"/"+nodeName+".cpp";
        //str   vcprojPth = "../"+nodeName+"/"+nodeName+".vcxproj";
        
        FILE*    cppHndl = fopen(cppPth.c_str(),    "wb");
        if(!cppHndl) return;

        FILE* vcprojHndl = fopen(vcprojPth.c_str(), "wb");
        if(!vcprojHndl) return;

        str    cppFile = regex_replace( Template_cpp,     regex("\\|_NAME_\\|"), nodeName);
        str vcprojFile = regex_replace( Template_vcxproj, regex("\\|_NAME_\\|"), nodeName);

        fwrite(cppFile.c_str(),     1, cppFile.length(),        cppHndl);                   // don't want to write the '\0' null character at the end of the string into the file
        fwrite(vcprojFile.c_str(),  1, vcprojFile.length(),  vcprojHndl);

        fclose(cppHndl);
        fclose(vcprojHndl);
      });

      fd.ui.keyWin->setLayout(fd.ui.keyLay);

      Theme* thm = fd.ui.keyWin->theme();
      thm->mButtonFontSize      = 16;
      thm->mTransparent         = e4f( .0f,  .0f,  .0f,    .0f );
      thm->mWindowFillUnfocused = e4f( .2f,  .2f,  .225f,  .3f );
      thm->mWindowFillFocused   = e4f( .3f,  .28f, .275f,  .3f );

      fd.ui.statusWin = new  Window(&fd.ui.screen,    "");
      fd.ui.statusTxt = new TextBox(fd.ui.statusWin,  "");
      fd.ui.statusTxt->setEditable(false);
      fd.ui.statusTxt->setDefaultValue("");
      fd.ui.statusTxt->setValue("");
      fd.ui.statusLay = new BoxLayout(Orientation::Horizontal, Alignment::Fill, 0,0);

      fd.ui.statusWin->setLayout(fd.ui.statusLay);

      fd.ui.screen.setVisible(true);
      fd.ui.screen.performLayout();
    }
    SECTION(init nanovg and font)
    {
      vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
	    if(vg == NULL) {
        printf("Could not init nanovg.\n");
		    return -1;
	    }
      int font = nvgCreateFontMem(vg, "sans-bold", Roboto_Bold_ttf, Roboto_Bold_ttf_len, 0);
      if(font == -1) {
		    printf("Could not add font bold.\n");
		    return -1;
	    }
    }
    SECTION(test data init)
    {
      //printf("Arg    size: %lld \n\n", sizeof(LavaVal));
      //printf("Msg    size: %lld \n\n", sizeof(LavaMsg));
      //printf("Packet size: %lld \n\n", sizeof(LavaPacket));
      //
      //fd.ui.slot_rad = 15.f;

      reloadSharedLibs();

      node_add("MakeCube",  Node("six",  Node::Type::MSG,  {200.f,200.f}) );

      //auto   inst0 = node_add("FileToString", Node("one",   Node::Type::FLOW, {400.f,300.f}) );
      //auto   inst1 = node_add("FileToString", Node("two",   Node::Type::FLOW, {200.f,500.f}) );
      //auto   inst2 = node_add("FileToString", Node("three", Node::Type::FLOW, {700.f,500.f}) );
      //auto   inst3 = node_add("FileToString", Node("four",  Node::Type::FLOW, {700.f,700.f}) );
      //auto   inst4 = node_add("FilePathMsg",  Node("five",  Node::Type::MSG,  {200.f,200.f}) );
      //auto   inst5 = node_add("MakeCube",  Node("six",  Node::Type::MSG,  {200.f,200.f}) );
      
      //LavaId s0 = slot_add( Slot(inst0, false)  );
      //LavaId s1 = slot_add( Slot(inst1,  true)  );
      //LavaId s2 = slot_add( Slot(inst2,  true)  );
      //LavaId s3 = slot_add( Slot(inst3,  true)  );
      //LavaId s4 = slot_add( Slot(inst0, false)  );
      //LavaId s5 = slot_add( Slot(inst4, false)  );

      //fd.lgrph.toggleCnct(s5, s1);
      //fd.lgrph.toggleCnct(s0, s1);
      //fd.lgrph.toggleCnct(s0, s2);
      //fd.lgrph.toggleCnct(s0, s3);
    }
    SECTION(lava and db)
    {
      new (&fisdb) simdb("Fissure", 256, 2<<10);
      new (&fd.vizIds) AtmSet( LavaId().asInt );
      fd.flow.packetCallback = lavaPacketCallback;
      LavaInit();
    }

    SECTION(test flat lock free map)
    {
      // Println();
      // flf_map  defaultMap;
      // flf_map  tstMap(4);
      // Println("Idx: ", sizeof(flf_map::Idx) 
      //          );
      //Print("List: ");
      //u32* lst = tstMap.listStart( tstMap.capacity() );
      //TO(tstMap.capacity(),i) Print(lst[i]," ");
      //Println();
      //Println("size: ", tstMap.size(), " capacity: ", tstMap.capacity() );
      //Println();
      //Println("put: ", tstMap.put(85, 101) );
      //Println("put: ", tstMap.put(20, 40) );
      //Println("put: ", tstMap.put(5, 7) );
      ////Println("put: ", tstMap.put(11, 13) );
      ////Println("put: ", tstMap.put(17, 19) );
      //Println();
      //print_flf_map(tstMap);
      //Println("get() as bool: ",  (bool)tstMap.get(85), "   ", tstMap.get(85).ok);
      //Println("get() as Value: ", (u64)tstMap.get(85),  "   ", tstMap.get(85).value);
      //Println();
      //print_flf_map(tstMap);
    }
  }

  glfwSetTime(0);
  SECTION(main loop)
  {
    auto&      g = fd.lgrph;
    v2      pntr = {0,0};
    f64 cx, cy, t, dt, avgFps=60, prevt=0, cpuTime=0;
    f32 pxRatio;
    int fbWidth, fbHeight;

    while(!glfwWindowShouldClose(fd.win))
    {
      auto&      ms = fd.mouse;
      bool    rtClk = (ms.rtDn  && !ms.prevRtDn);  // todo: take this out
      auto      nds = node_getPtrs();
      auto       sz = nds.size();
      bool isInNode = false;
      LavaId    nid;
      u64      nIdx;

      SECTION(time)
      {
        t     = glfwGetTime();
		    dt    = t - prevt;
		    prevt = t;
      }
      SECTION(input)
      {
        glfwPollEvents();                                             // PollEvents must be done after zeroing out the deltas
  	    glfwGetCursorPos(fd.win, &cx, &cy);

        fd.ui.prevPntr = pntr;
        pntr=Vec2((float)cx, (float)cy);

		    glfwGetWindowSize(fd.win, &fd.ui.w, &fd.ui.h);
		    glfwGetFramebufferSize(fd.win, &fbWidth, &fbHeight);

        pxRatio = (float)fbWidth / (float)fd.ui.w;          // Calculate pixel ration for hi-dpi devices.
      }
      SECTION(gl frame setup)
      {
		    glViewport(0, 0, fbWidth, fbHeight);
			  glClearColor(.075f, .075f, .075f, 1.0f);
		    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
      }
      SECTION(get lava graph visualization data)
      {        
        fd.graph.curNode  =  fd.flow.m_curId.nid;          // todo: make atomic, although this may just work since it is only reading 8 bytes

        auto& slts = fd.graph.packetSlots;
        slts.clear();
        fd.flow.m_qLck.lock();
          if(fd.flow.q.size() > 0){
            auto& pckt = fd.flow.q.top();
            slts.emplace_back( pckt.dest_node, pckt.dest_slot );
            slts.emplace_back( pckt.src_node, pckt.src_slot );
          }
        fd.flow.m_qLck.unlock();

        sort( ALL(slts) );
      }
      SECTION(selection)
      {
        // figure out all the information that needs to be known before figuring what to do with it
        bool lftClkDn  =   ms.lftDn && !ms.prevLftDn;      // lftClkDn is left click down
        bool lftClkUp  =  !ms.lftDn &&  ms.prevLftDn;      // lftClkDn is left click up
        bool rtClkDn   =   ms.rtDn  && !ms.prevRtDn;       // rtClkDn is right click down

        LavaId    sid;                                     // sid is slot id
        bool isInSlot = false;
        SECTION(slot inside check: if inside a slot, early exit on the first found) // todo: does this need to loop through in the node ordr ? 
        {
          for(auto& kv : fd.graph.slots){
            Slot&    s  =  kv.second;
            isInSlot    =  len(pntr - s.P) < fd.ui.slot_rad;

            if(isInSlot){ 
              sid = kv.first;
              break;
            }
          }
        }

        if(!isInSlot) SECTION(node inside check: if inside a node in node ordr from top to bottom)
        {
          FROM(sz,i)                                                // loop backwards so that the top nodes are dealt with first
          {
            Node*     n  =  nds[i];
            isInNode     =  isIn(pntr.x,pntr.y, n->b);
            if(isInNode){
              nid  = n->id;
              nIdx = i;
              break;
            }
          }
        }

        bool    slotClk  =  isInSlot && lftClkDn;
        bool    nodeClk  =  isInNode && lftClkDn;
        bool  slotRtClk  =  isInSlot &&  rtClkDn;
        // end per loop data

        SECTION(mouse ui state)
        {
          if(!ms.lftDn){ 
            ms.dragging = false;
            ms.drgbox   = false;
          }
          ms.dragging  |=  lftClkDn;                             // only change from false to true - already true will stay true
          ms.drgbox    |=  !slotClk && !nodeClk && lftClkDn;     // only change from false to true - already true will stay true

          if(!ms.dragging){ ms.drgP = pntr; }                    // if not dragging, the drag pointer and pointer are the same

          if(ms.drgbox){
            ms.drgbnd = Bnd( min(ms.drgP.x, pntr.x),
              min(ms.drgP.y, pntr.y),
              max(ms.drgP.x, pntr.x),
              max(ms.drgP.y, pntr.y) );
          }else{
            ms.drgbnd = Bnd();
          }
        }
        SECTION(select src and dest slots)
        {          
          if(slotClk){
            Slot* s = slot_get(sid);
            if(s->in) fd.sel.slotInSel  = sid;
            else      fd.sel.slotOutSel = sid;
            s->state = Slot::SELECTED;
          }
        }
        SECTION(node primary select and move to top)
        {
          if(nodeClk)
          {
            Node* n     =  &(node_moveToFront(nid.nid));
            nds         =  node_getPtrs();                    // move to the front will invalidate some pointers in the nds array so it needs to be remade
            fd.sel.pri  =  n->id;
            ms.drgP     =  pntr;
          }
        }
        SECTION(select from drg box)
        {
          if(ms.drgbox) TO(sz,i)
          {
            Node& n       =  *(nds[i]);
            bool inside   =  isIn(n.b, ms.drgbnd);
            n.sel        |=  inside;
            //anyInside    |=  inside;
          }
        }
        SECTION(connection creation)
        {
          if(fd.sel.slotInSel.sidx  != LavaId::SLOT_NONE && 
             fd.sel.slotOutSel.sidx != LavaId::SLOT_NONE)
          {
            LavaCommand::Arg dest;
            LavaCommand::Arg  src;
            dest.id = fd.sel.slotInSel;
            src.id  = fd.sel.slotOutSel;
            fd.lgrph.put(LavaCommand::TGL_CNCT, dest, src);
            fd.sel.slotOutSel = fd.sel.slotInSel = LavaId(0,0);

            sel_clearSlots();
          }
        }
        SECTION(selection clearing)
        {
           if(lftClkDn && !slotClk && !nodeClk){
             sel_clear();
             fd.ui.statusTxt->setValue("");
           }
        }
        SECTION(slot output simdb writing)
        {
          if(slotRtClk){
            if( fd.vizIds.has(sid.asInt) ){
              fd.vizIds.del(sid.asInt);
              fisdb.del( genDbKey(sid) );
            }else{
              fd.vizIds.put( sid.asInt );
            }
          }
        }
        SECTION(nanogui status bar)
        {
          f64 totalTime = timeToSeconds(fd.lgrph.totalTime());

          if(nid.nid == LavaId::NODE_NONE){
            fd.ui.statusTxt->setValue("");
          }else if(slotRtClk){
            fd.ui.statusTxt->setValue( toString(" right click on slot ") );
          }else if(isInSlot){
            auto status  =  toString("Slot [",sid.nid,":",sid.sidx,"]");
            fd.ui.statusTxt->setValue( status );
          }else if(isInNode){
            //f64 seconds  =  timeToSeconds(fd.lgrph.node(nid.nid).time);
            //f64 percent  =  totalTime>0?  (seconds/totalTime)*100  :  0;
            //auto status  =  toString("Node [",nid.nid,"]  ",nds[nIdx]->txt," | ",seconds," seconds  %",percent);

            auto status = makeStatusText(nid.nid, totalTime, nds, nIdx);
            fd.ui.statusTxt->setValue( status );
          }else if(fd.sel.pri != LavaNode::NODE_ERROR){
            //f64 seconds  =  timeToSeconds(fd.lgrph.node(fd.sel.pri).time);
            //f64 percent  =  totalTime>0?  (seconds/totalTime)*100  :  0;
            //auto status  =  toString("Node [",fd.sel.pri,"]  ",fd.graph.nds[fd.sel.pri].txt," | ",seconds," seconds  %",percent);

            auto status = makeStatusText(nid.nid, totalTime, nds, nIdx);
            fd.ui.statusTxt->setValue( status );
          }else{
            fd.ui.statusTxt->setValue("");
          }
        }
      }
      SECTION(movement)
      {
        SECTION(node movement)
        {
          TO(sz,i)
          {
            Node&     n = *(nds[i]);
            bool selctd = n.id==fd.sel.pri || n.sel;

            if(ms.dragging && fd.sel.pri!=LavaNode::NODE_ERROR && selctd ){           // if a node is primary selected (left mouse down on a node) or the selected flag is set
              n.P +=  pntr - fd.ui.prevPntr;
            }

            n.b = node_bnd(vg, n);
          }
        }
        SECTION(slot movement)
        {
          for(auto& kv : fd.graph.slots)
          {
            LavaId    nid = kv.first;
            Slot&       s = kv.second;
            Node const& n = fd.graph.nds[nid.nid];
            v2 wh = n.b.wh();
            v2 nP = n.P + wh/2; //NODE_SZ/2; // n.b.mx; // w()/2; // NODE_SZ/2;
            v2 nrml;

            if(s.in)
            {                                                 // dest / in / blue slots
              LavaFlowSlot* src = g.srcSlot(nid);
              if(src){
                auto srcIter = fd.graph.slots.find(src->id);
                if(srcIter != fd.graph.slots.end() ){
                  auto  srcNdP = fd.graph.nds[src->id.nid].P;
                  s.P = node_border(n, srcNdP - nP, &nrml);
                  s.N = nrml;
                }
              }else{
                s.P = node_border(n, {0,-1.f}, &nrml);
                s.N = {0,-1.f};
              }
            }else
            {
              auto ci = fd.lgrph.destSlots(kv.first);
              if(ci==fd.lgrph.destCnctEnd()){
                s.P = node_border(n, v2(0,1.f), &nrml);
                s.N = nrml;
              }else{
                v2  destP={0,0}, destN={0,0};
                int   cnt = 0;
                for(; ci != fd.lgrph.destCnctEnd() && ci->first==nid; ++ci)
                {
                  if(!fd.lgrph.slot(ci->second)){ cnt -= 1; continue; }   // todo: does this need to subtract 1 from count?
                          
                  auto si = fd.graph.slots.find(ci->second);
                  if(si != fd.graph.slots.end()){
                    auto curP  =  si->second.P;
                    destP     +=  curP; 
                    destN     +=  norm(curP - nP);
                    ++cnt;
                  }
                }
                destP /= (f32)cnt;
                destN /= (f32)cnt;
                s.N = norm(destN);
                s.P = node_border(n, s.N);
              }
            }
          }
        }
      }
      SECTION(drawing)
      {
        SECTION(nanovg drawing - |node graph|)
        {
          nvgBeginFrame(vg, fd.ui.w, fd.ui.h, pxRatio);
          SECTION(draw background grid)
          {
            nvgStrokeWidth(vg, 1.f);
            nvgStrokeColor(vg, nvgRGBAf( .12f, .12f, .12f, 1.f));

            u32 lineCntX = (fd.ui.w / 100) + 1;
            f32 lineIncX  = fd.ui.w / (f32)lineCntX;
            f32    curX  = 0; 
            TO(lineCntX,i){
              nvgBeginPath(vg);
                nvgMoveTo(vg, curX, 0);
                nvgLineTo(vg, curX, (f32)fd.ui.h);
              nvgStroke(vg);

              curX += lineIncX;
            }

            u32 lineCntY = (fd.ui.h / 100) + 1;
            f32 lineIncY  = fd.ui.h / (f32)lineCntY;
            f32    curY  = 0; 
            TO(lineCntY,i){
              nvgBeginPath(vg);
              nvgMoveTo(vg, 0, curY);
              nvgLineTo(vg, (f32)fd.ui.w, curY);
              nvgStroke(vg);

              curY += lineIncY;
            }
          }
          SECTION(draw current node highlights)
          {
            auto nid = fd.graph.curNode;
            if(nid != LavaNode::NONE  &&
               fd.graph.nds.count(nid) > 0)
            {
              auto const& n = fd.graph.nds[nid];
              node_draw_radial(n, vg, nvgRGBA(0,255,128,48));
            }
          }
          SECTION(draw highlights behind visualized slots)
          {
            TO(fd.vizIds.sz,i)
            {
              LavaId id;
              id.asInt  = fd.vizIds.load(i);
              Slot*   s = nullptr;
              if(id.asInt == fd.vizIds.null_val){ continue; }
              s = slot_get(id);
              if(!s){ continue; }

              draw_radial(vg, nvgRGBA(0,128,228,255), 
                s->P.x, s->P.y, fd.ui.slot_rad*5); 
            }
          }
          SECTION(draw outstanding packets as circle segments around slots)
          {
            nvgStrokeColor(vg, nvgRGBAf(0,0,0,1.f));
            nvgStrokeWidth(vg, 1.f);
            nvgFillColor(vg, nvgRGBAf(1.f,1.f,0,.75f) );

            auto const& slots = fd.graph.slots;
            for(auto lid : fd.graph.packetSlots)
            {
              auto sIter = node_slots(lid.nid);
              for(; sIter!=end(slots) && sIter->first.nid==lid.nid; ++sIter)
              {
                auto sIdx = sIter->first;                        // todo: needs to be redone
                if(lid.sidx != sIdx.sidx){ continue; }
                  
                Slot const& s = sIter->second;

                // draw a circle larger than the slot circle, which will show up as a sort of halo when the slot is drawn over it in the next section
                nvgBeginPath(vg);
                  nvgCircle(vg, s.P.x, s.P.y, fd.ui.slot_rad+5.f);
                  nvgFill(vg);
                  nvgStroke(vg);
                nvgStroke(vg);
              }
            }
          }
          SECTION(draw connections)
          {
            auto di = g.srcCnctsMap().begin();                                    // di is destination iterator
            auto en = g.srcCnctsMap().end();
            for(auto di = g.srcCnctsMap().begin(); di != en; )
            {
              auto     srcIdx = di->first;
              auto    destIdx = di->second;
              u64  cnt = 0; v2 srcP(0,0); v2 srcN(0,0);
              auto const& si = fd.graph.slots.find(srcIdx);
              if(si != fd.graph.slots.end()){
                cnt  = g.destCnctCount(srcIdx);
                srcP = si->second.P;
                srcN = si->second.N;
              }

              if(cnt==1)
              {
                Slot const& dest = fd.graph.slots.find(destIdx)->second;
                f32 w = fd.graph.nds[destIdx.nid].b.w();

                cnct_draw(vg, srcP, dest.P, srcN, dest.N, w/2);
                ++di;
              }
              else
              {
                v2 avgP=srcP; v2 avgN={0,0}; u32 cnt=0;
                auto avgIter=di;
                for(; avgIter!=en && avgIter->first==srcIdx; ++avgIter )
                {    
                  if(! slot_get(avgIter->second)){ continue; }

                  Slot const& dest = *(slot_get(avgIter->second));
                  avgP += dest.P;
                  avgP += srcP;
                  ++cnt;
                }
                avgP    /= (f32)(cnt*2+1);             // can't forget the first position for averaging - the src position - the avgP is weighted 1:1 with the srcP and all the destination positions combined
                v2 midN  = norm(srcP - avgP);
                f32   w  = fd.graph.nds[destIdx.nid].b.w();

                cnct_draw(vg, srcP, avgP, srcN, midN, w/2);

                for(auto dhIter=di; di!=en && di->first == srcIdx; ++di){        // dhIter is draw half iterator - this is where the the connections are drawn from the average position of all slots 
                  const v2 hlfsz = fd.ui.slot_rad/2.f;

                  Slot* dest = slot_get(di->second);

                  if(dest) cnct_draw(vg, avgP, dest->P, -1.f*midN, dest->N, w/2);
                }

                nvgBeginPath(vg);
                  nvgCircle(vg, avgP.x, avgP.y, 6.f); 
                  nvgFillColor(vg, nvgRGBAf(.18f, .18f, .18f, 1.f) );
                  nvgStrokeWidth(vg, 3.f);
                  nvgStrokeColor(vg, nvgRGBAf(0,0,0,1.f) );
                nvgStroke(vg);
                nvgFill(vg);
              }
            }
          }
          SECTION(draw nodes and slots)
          {
            auto const& slots = fd.graph.slots;
            TO(sz,i)
            {
              Node&     n = *(nds[i]);
              bool selctd = n.id==fd.sel.pri || n.sel;

              LavaInst::State state = fd.lgrph.getState(n.id);
              if(state == LavaInst::RUN_ERROR){
                node_draw_radial(n, vg, nvgRGBA(255,48,0,255));
              }

              bool highlight = isInNode && nIdx==i;
              node_draw(vg, 0, n, 1.f, selctd, fd.ui.nd_border, highlight);

              SECTION(draw node slots)
              {
                nvgStrokeColor(vg, nvgRGBAf(0,0,0,1.f));
                nvgStrokeWidth(vg, fd.ui.slot_border);
                
                auto sIter = node_slots(n.id);
                for(; sIter!=end(slots) && sIter->first.nid==n.id; ++sIter)
                {
                  auto     sIdx = sIter->first;                    // todo: needs to be redone
                  //Slot const& s = *(grph.slot(sIdx));
                  Slot const& s = sIter->second;
                  bool   inSlot = len(pntr - s.P) < fd.ui.slot_rad; //io_rad;

                  Slot::State drawState = Slot::NORMAL;
                  if(s.state==Slot::SELECTED) drawState = Slot::SELECTED;
                  else if(inSlot)             drawState = Slot::HIGHLIGHTED;
                  slot_draw(vg, s, drawState, fd.ui.slot_rad);
                }
              }
            }
          }
          SECTION(draw selection box)
          {
            if(fd.mouse.lftDn && fd.sel.pri==LavaNode::NODE_ERROR)
            {
              nvgBeginPath(vg);
                float x,y,w,h;
                x = min(ms.drgP.x, pntr.x); 
                y = min(ms.drgP.y, pntr.y); 
                w = abs(ms.drgP.x - pntr.x);
                h = abs(ms.drgP.y - pntr.y);
                nvgRect(vg, x,y, w,h);
                nvgStrokeWidth(vg, 1.f);
                nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, .75f));
              nvgStroke(vg);
            }
          }
          SECTION(draw fps - frames per second counter)
          {
            avgFps *= 0.9;
            avgFps += (1.0/dt)*0.1;
            int fps = (int)avgFps;

            char fpsStr[TITLE_MAX_LEN];
            sprintf(fpsStr, "%d", fps);

            f32 tb = nvgTextBounds(vg, 10,0, fpsStr, NULL, NULL);
            nvgFontSize(vg, 12.0f);
            nvgFontFace(vg, "sans-bold");
            nvgTextAlign(vg,  NVG_ALIGN_LEFT);  // NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
            nvgFillColor(vg, nvgRGBA(255,255,255,255));
            nvgText(vg,  fd.ui.w - tb , 10, fpsStr, NULL);
          }
          nvgEndFrame(vg);
        }
        SECTION(nanogui)
        {
          fd.ui.screen.performLayout();

          e2i statusSz = fd.ui.statusWin->size();
          fd.ui.statusWin->setPosition( e2i(0, fd.ui.h - statusSz[1]) );
          fd.ui.statusWin->setSize( e2i(fd.ui.w, statusSz[1]) );
          fd.ui.statusTxt->setWidth( fd.ui.w - fd.ui.statusLay->margin() );

          fd.ui.screen.drawContents();
          fd.ui.screen.drawWidgets();
        }
      }

      fd.mouse.prevRtDn  = fd.mouse.rtDn;
      fd.mouse.prevLftDn = fd.mouse.lftDn;

      glfwSwapBuffers(fd.win);
      glfwPollEvents();
      LavaGraph::ArgVec av = fd.lgrph.exec();
      graph_apply(move(av));
      //fd.flow.udpateMsgIdxCache();
    }
  }
  SECTION(shutdown)
  {
    stopFlowThreads();
    nanogui::shutdown();
    glfwTerminate();
  }

  return 0;
}












//TO(sz,i)
//Node&     n = *(nds[i]);
//
//auto sIter = node_slots(n.id);
//
//Slot const& s = *(grph.slot(sIdx));
//
//bool   inSlot = len(pntr - s.P) < fd.ui.slot_rad; //io_rad;
//
//Slot::State drawState = Slot::NORMAL;
//if(s.state==Slot::SELECTED) drawState = Slot::SELECTED;
//else if(inSlot)             drawState = Slot::HIGHLIGHTED;
//slot_draw(vg, s, drawState, fd.ui.slot_rad);

//decltype(std::unordered_map<int,int>::begin()) 
//Println("sizeof( std::unordered_map<int,int>::iterator ) ): ",  sizeof( std::unordered_map<int,int>::iterator ) );

//SECTION(test atomic bitset)
//{
//  //AtomicBitset ab;
//  //Println("  slot 0: ", (bool)ab[0], " : ", (char*)ab.toStr().bitstr );
//  //ab[0] = 1;
//  //Println("  slot 0: ", (bool)ab[0], " : ", (char*)ab.toStr().bitstr );
//  //ab[0] = 1;
//  //Println("  slot 7: ", (bool)ab[7], " : ", (char*)ab.toStr().bitstr );
//  //ab[7] = 1;
//  //Println("  slot 7: ", (bool)ab[7], " : ", (char*)ab.toStr().bitstr );
//  //ab[7] = 1;
//  //
//  //Println("  slot 0: ", ab[0], " : ", ab.toStr().bitstr);
//  //ab[0] = 0;
//  //Println("  slot 0: ", ab[0], " : ", ab.toStr().bitstr);
//}

//SECTION(test slot mask)
//{
//  //Println("size of frame: ", sizeof(LavaFrame));
//  //LavaFrame tstFrame;
//  //Println("  slot 0: ", tstFrame.slotMask[0], " : ", tstFrame.slotMask);
//  //tstFrame.slotMask[0] = 1;
//  //Println("  slot 0: ", tstFrame.slotMask[0], " : ", tstFrame.slotMask);
//  //tstFrame.slotMask[0] = 0;
//  //Println("  slot 0: ", tstFrame.slotMask[0], " : ", tstFrame.slotMask);
//  //
//  ////tstFrame.slotMask[16] = 1;
//  ////Println("  slot 16: ", tstFrame.slotMask[16], " : ", tstFrame.slotMask);
//  ////tstFrame.slotMask[0] = 1;
//  ////Println("  slot 16: ", tstFrame.slotMask[16], " : ", tstFrame.slotMask);
//  ////tstFrame.slotMask[63] = 1;
//  ////Println("  slot 63: ", tstFrame.slotMask[63], " : ", tstFrame.slotMask);
//  //
//  //Println("\n");
//  //// tstFrame.slotMask = 0;
//  //Println("slot cout: ",tstFrame.slotCount(),"  slot ",0,": ", tstFrame.slotMask[0], " : ", tstFrame.slotMask);
//  //TO(LavaFrame::PACKET_SLOTS,i){
//  //  tstFrame.slotMask[i] = 1;
//  //  Println("slot cout: ",tstFrame.slotCount(),"  slot ",i,": ", tstFrame.slotMask[i], " : ", tstFrame.slotMask);
//  //  //tstFrame.slotMask[i,0);
//  //}
//  //Println();
//}

//using Id      = LavaId;
//
//extern "C" const char const* Template_cpp;
//extern "C" const char const* Template_vcxproj;
//
//auto spcr3     = new  Label(fd.ui.keyWin,     "");
//
//nodeTxt->setWidth(800);
//
//fwrite(Template_cpp,     1, strlen(Template_cpp)+1,        cppHndl);
//fwrite(Template_vcxproj, 1, strlen(Template_vcxproj)+1, vcprojHndl);

//Print("A:  ");
//TO(q.capA(),i) Print(q.atA(i)," ");
//Println("");
//
//Print("B:  ");
//TO(q.capB(),i) Print(q.atB(i)," ");
//Println("");

//LavaId          slot_add(Slot s)
//{
//LavaId sid = fd.lgrph.addSlot(ls);
//
//fd.graph.slots.insert( {sid, s} );
//
//ls.id = sid;
//return sid;
//}

//void            node_add(str node_name, Node n)
//void            node_add(str node_name, Node n)
//{
//  using namespace std;
//
//  auto          pi = fd.flow.nameToPtr.find( node_name );                               // pi is pointer iterator
//  uint64_t instIdx = LavaNode::NODE_ERROR;
//  LavaNode*     ln = nullptr;
//  if( pi != end(fd.flow.nameToPtr) ){
//    ln      = pi->second;
//    LavaCommand::Arg nodeArg;
//    nodeArg.ndptr = ln;
//    fd.lgrph.put(LavaCommand::ADD_NODE, nodeArg);
//  }
//
//  if(ln)
//  {
//    auto out_types = ln->out_types;                    // do these first so that the output slots start at 0
//    for(; out_types  &&  *out_types; ++out_types){ 
//      slot_add(false);                                 // this will put a slot command into the LavaGraph command queue - the graph_apply() function will actually end up creating the slots in the ui graph
//    }
//
//    auto in_types = ln->in_types;
//    for(; in_types  &&  *in_types; ++in_types){
//      slot_add(true);
//    }
//  }
//}

//instIdx = fd.lgrph.addNode(ln, true);
//
//if(instIdx != LavaNode::NODE_ERROR)
//
//slot_add( Slot(instIdx, true) );
//
//slot_add( Slot(instIdx, false) );

//FisData::IdOrder ido;                                                          //ido is id order
//ido.id    = instIdx;
//ido.order = node_nxtOrder();
//fd.graph.ordr.insert(ido);
//
//n.txt   = "New: " +  node_name;
//n.id    = instIdx;
//n.order = ido.order;
//fd.graph.nds[instIdx] = move(n);

//instIdx = fd.lgrph.addNode(ln, true);
//
//if(instIdx != LavaNode::NODE_ERROR)
//
//slot_add( Slot(instIdx, true) );
//
//slot_add( Slot(instIdx, false) );
//
//FisData::IdOrder ido;                                                          //ido is id order
//ido.id    = instIdx;
//ido.order = node_nxtOrder();
//fd.graph.ordr.insert(ido);
//
//n.txt   = "New: " +  node_name;
//n.id    = instIdx;
//n.order = ido.order;
//fd.graph.nds[instIdx] = move(n);
//
//}
//return instIdx;

//fd.graph.curNode = fd.flow.q.top().dest_node;      // todo: race condition
//
//LavaId nxtPckt = fd.flow.getNxtPacketId();
//if(nxtPckt.nid != LavaId::NODE_NONE)
//  fd.graph.curNode = nxtPckt.nid;

//TO(sz,j){ nds[j]->sel = false; }                // why do this?   // never O^2 due to no longer being inside an outer loop of nodes  todo: move this out of the outer loop due to being O(n^2) - actually not O(n^2) due to the break
//n->sel = true;
//break;                                          // without breaking from the loop, a node could be moved down and hit again

//if(!inAnySlt && lftClkUp){
//  fd.sel.slotOutSel = fd.sel.slotInSel = LavaId(0,0);
//  sel_clearSlots();
//}
//
//fd.lgrph.toggleCnct(fd.sel.slotOutSel, fd.sel.slotInSel);

//auto    w = fd.ui.slot_rad, h = w;
//auto hlfw = w/2, hlfh = hlfw;
//auto   cx = s->P.x;
//auto   cy = s->P.y;
//nvgFillColor(vg, nvgRGBA(0,255,255,255));
//nvgBeginPath(vg);
//  nvgCircle(vg, cx, cy, w*4.25f );
//  auto radial = nvgRadialGradient(vg,
//    cx, cy, 0, hlfw*4.25f,
//    nvgRGBA(0,128,228,255),
//    nvgRGBA(0,0,0,0)  );
//  nvgFillPaint(vg, radial);
//nvgFill(vg);

//f64 seconds  =  fd.lgrph.node(fd.sel.pri).time / 1000000000.0;
//f64 seconds  =  fd.lgrph.node(nid.nid).time / 1000000000.0;

//for(auto& kv : grph.slots())
//Id     nid = kv.first;            // s.nid;
//Node const& n = grph.node(nid.id);
//
//u64       nid = kv.first;              // s.nid;
//LavaSlot&  ls = fd.lgrph.slots();

//Slot* src = grph.srcSlot(kv.first);
//Slot* src = &s;     // srcSlot(kv.first);
//Slot* src = nullptr;
//auto srcNdId = g.srcSlot(nid)->nid;

//auto srcNdP = grph.node(src->nid).P + wh/2;  //NODE_SZ/2; // n.b.mx; // NODE_SZ/2;
//auto srcNdP = n.P + wh/2;
//
//auto srcIter = fd.graph.slots.find(src->id);

//v2 curP = fd.lgrph.slot(ci->second)->P;
//v2 curP = fd.graph.slot.find(ci->second)->P;
//fd.lgrph.srcSlot(ci->second);

//auto ci = grph.destSlots(kv.first);
//if(ci==grph.destCnctEnd()){
//  s.P = node_border(n, v2(0,1.f), &nrml);
//  s.N = nrml;
//}else{
//  v2  destP={0,0}, destN={0,0};
//  int   cnt = 0;
//  for(; ci != grph.destCnctEnd() && ci->first==nid; ++cnt, ++ci)
//  {
//    if(!grph.slot(ci->second)){ cnt -= 1; continue; }   // todo: does this need to subtract 1 from count?
//            
//    v2 curP = grph.slot(ci->second)->P;
//    destP  += curP; 
//    destN  += norm(curP - nP);
//  }
//  destP /= (f32)cnt;
//  destN /= (f32)cnt;
//  s.N = norm(destN);
//  s.P = node_border(n, s.N);
//}

//n.sel? fd.ui.msgnd_selclr : fd.ui.msgnd_gradst,
//n.sel? fd.ui.msgnd_selclr : fd.ui.msgnd_graden );
//
//b = { x, y, x+w, y+w };

//auto hlfw = w / 2;
//auto hlfh = h / 2;

//auto    w = n.b.w();
//auto hlfw = w / 2;
//auto    h = n.b.h();
//auto hlfh = h / 2;
//auto   cx = n.P.x + hlfw;
//auto   cy = n.P.y + hlfh;
//draw_radial(vg, nvgRGBA(0,255,128,48), cx, cy, w*1.25f);

//nvgFillColor(vg, nvgRGBA(0,255,255,255));
//nvgBeginPath(vg);
//  nvgCircle(vg, cx, cy, n.b.w()*1.25f );
//  auto radial = nvgRadialGradient(vg,
//    cx, cy, 0, hlfw*1.25f,
//    nvgRGBA(0,255,128,48),
//    nvgRGBA(0,0,0,0)  );
//nvgFillPaint(vg, radial);
//nvgFill(vg);

//auto    w = rad h = w;
// hlfh = hlfw;
//auto   cx = s->P.x;
//auto   cy = s->P.y;

//
//nds.push_back(LavaId(fd.sel.pri));

//}else if(slotClk){
//}else if(nodeClk){

//auto    h = fd.ui.slot_rad;
//auto hlfw = w / 2;
//auto hlfh = h / 2;

//auto sIter = fd.graph.slots.find(n.id);                             // sIter is slot iterator
//auto sIter = lower_bound( ALL(slots), n.id);                      // sIter is slot iterator

//LavaId sid(pkt.src_node, pkt.src_slot);
//
//auto    ni = fd.graph.nds.find(pkt.src_node); // todo: this is called from the lava looping threads and would need to be thread safe - it is also only used for getting text labels, which may make things easier
//if(ni == end(fd.graph.nds)) return; 
//
//auto label  =  toString("[",pkt.src_node,":",pkt.src_slot,"] ",ni->second.txt);

//
//startFlowThreads(1);

//sprintf(winTitle, "%.4f  %.4f", px, py);
//glfwSetWindowTitle(win, winTitle);

//
//fd.ui.statusWin->setPosition( e2i(statusSz[0], fd.ui.h - statusSz[1]) );

//auto entryBtn  = new Button(fd.ui.keyWin,    "Set Entry Node");
//
//auto msgBtn    = new Button(fd.ui.keyWin,    "Message Node");
//auto flowBtn   = new Button(fd.ui.keyWin,    "Flow Node");

//entryBtn->setCallback([](){        
//  u64 selId = Node::Type::NODE_ERROR;
//  auto  nds = node_getPtrs();
//  for(auto n : nds) if(n->sel  &&  n->type==Node::Type::MSG){
//    selId = n->id;
//    break;
//  }
//  if(selId==Node::Type::NODE_ERROR){ printf("\nno message nodes selected\n"); }
//
//  printf("\ntodo: primary selection node to be the entry point - selected: %d \n", (i32)selId );
//
//  //TO(nds.size(),i){
//  //  if(nds[i]->sel && nds[i]->type == Node::MSG)
//  //}
//  //
//  //if(fd.sel.pri < 0) printf("no nodes selected\n\n");
//});

//msgBtn->setCallback([](){
//  node_add("FileToString", Node("message node", Node::Type::MSG) );
//});
//flowBtn->setCallback([](){
//  node_add("FileToString", Node("new node", Node::Type::FLOW) );
//});

//SECTION(selection)
//{
//  bool lftClkDn =  ms.lftDn && !ms.prevLftDn;    // lftClkDn is left click down
//  bool lftClkUp = !ms.lftDn &&  ms.prevLftDn;    // lftClkDn is left click up
//  bool clearSelections = true;
//  //Bnd  drgbnd;
//  ms.prevDrgbox = ms.drgbox;
//  SECTION(select from selection box)
//  {
//    bool anyInside = false; 
//    if(lftClkUp || ms.drgbox){    // put this above the box settings?
//      TO(sz,i){
//        Node& n       =  *(nds[i]);
//        bool inside   =  isIn(n.b, ms.drgbnd);
//        n.sel        |=  inside;
//        anyInside    |=  inside;
//      }
//      //if(drgbnd.hasLen() && fd.mouse.lftDn)
//      if(ms.drgbnd.area()>1.f && fd.mouse.lftDn)
//        clearSelections = false;
//    }
//
//    if(!anyInside && lftClkUp){
//      clearSelections = true;
//    }
//
//    if(!fd.mouse.lftDn){
//      fd.sel.pri = LavaNode::NODE_ERROR;
//    }
//  }
//  SECTION(selection box create)
//  {
//    //if(ms.drgbox || drgbnd.hasLen()){    // && lftClkUp
//    if(ms.drgbox || ms.drgbnd.area() > 1.f){
//      clearSelections=false;
//    }
//
//    if(!ms.lftDn){ ms.drgP=pntr; ms.drgbox=false; }
//
//    if(ms.drgbox)
//      ms.drgbnd = Bnd( min(ms.drgP.x, pntr.x),
//        min(ms.drgP.y, pntr.y),
//        max(ms.drgP.x, pntr.x),
//        max(ms.drgP.y, pntr.y) );
//    else
//      ms.drgbnd = Bnd();
//  }
//
//  bool inAnySlt = false;
//  SECTION(slot selection and connection creation)
//  {
//    LavaId  inClk(LavaId::NODE_NONE, LavaId::SLOT_NONE);
//    LavaId outClk(LavaId::NODE_NONE, LavaId::SLOT_NONE);
//    //for(auto& kv : grph.slots())
//    for(auto& kv : fd.graph.slots)
//    {
//      LavaId sid = kv.first;                       // sid is slot id
//      Slot&    s = kv.second;
//      if(lftClkDn)
//      {  
//        bool inSlt = len(pntr - s.P) < fd.ui.slot_rad;
//        if(inSlt){
//          outClk  = sid;
//          s.state = Slot::SELECTED;
//          if(s.in) fd.sel.slotInSel  = sid;
//          else     fd.sel.slotOutSel = sid;
//          clearSelections = false;
//          inAnySlt = true;
//        }
//      }else if(lftClkUp){
//        bool inSlt = len(pntr - s.P) < fd.ui.slot_rad;
//        if(inSlt){
//          clearSelections = false;
//          inAnySlt = true;
//        }
//      }
//    }
//
//    if(!inAnySlt && lftClkUp){
//      fd.sel.slotOutSel = fd.sel.slotInSel = LavaId(0,0);
//      sel_clearSlots();
//    }
//
//    if(fd.sel.slotInSel.sidx  != LavaId::SLOT_NONE && 
//      fd.sel.slotOutSel.sidx != LavaId::SLOT_NONE)
//    {
//      fd.lgrph.toggleCnct(fd.sel.slotOutSel, fd.sel.slotInSel);
//      fd.sel.slotOutSel = fd.sel.slotInSel = LavaId(0,0);
//
//      sel_clearSlots();
//    }
//  }
//
//  if(!inAnySlt) SECTION(node selection)
//  {
//    bool inAny = false;
//    FROM(sz,i)                                                // loop backwards so that the top nodes are dealt with first
//    {
//      Node*     n = nds[i];
//      bool inNode = isIn(pntr.x,pntr.y, n->b);
//      inAny      |= inNode;
//
//      SECTION(primary selection and group selection effects)
//      {
//        if(inNode)
//        {
//          if(lftClkDn && (fd.sel.pri==LavaNode::NODE_ERROR || fd.sel.pri!=n->id) )
//          {
//            n    =  &(node_moveToFront(n->id));
//            nds  =  node_getPtrs();                  // move to the front will invalidate some pointers in the nds array so it needs to be remade
//            fd.sel.pri = n->id;
//            ms.drgP    = pntr;
//
//            TO(sz,j){ nds[j]->sel = false; }        // todo: move this out of the outer loop due to being O(n^2) - actually not O(n^2) due to the break
//            n->sel = true;
//            break;                                  // without breaking from the loop, a node could be moved down and hit again
//          }
//          else if(lftClkUp)
//          {
//            TO(sz,j){ nds[j]->sel = false; }        // todo: move this out of the outer loop due to being O(n^2) - actually not O(n^2) due to the break
//            n->sel          = true;
//            clearSelections = false;
//            break;                
//          }
//        }
//      }
//    }
//
//    if(!inAny)
//    {
//      if(fd.mouse.lftDn){ 
//        clearSelections = false;
//      }
//
//      if(lftClkDn && fd.sel.pri==LavaNode::NODE_ERROR ){ ms.drgbox=true; }
//      if(fd.mouse.rtDn && !fd.mouse.prevRtDn){ fd.sel.sec = LavaNode::NODE_ERROR; }
//    }else{
//      clearSelections = false;
//    }
//  }
//
//  if(clearSelections && lftClkUp){ 
//    sel_clear();
//  }
//}

//  LavaId  inClk(LavaId::NODE_NONE, LavaId::SLOT_NONE);
//  LavaId outClk(LavaId::NODE_NONE, LavaId::SLOT_NONE);
//  for(auto& kv : fd.graph.slots)
//  {
//    LavaId sid = kv.first;                       // sid is slot id
//    Slot&    s = kv.second;
//    if(lftClkDn)
//    {  
//      bool inSlt = len(pntr - s.P) < fd.ui.slot_rad;
//      if(inSlt){
//        outClk  = sid;
//        s.state = Slot::SELECTED;
//        if(s.in) fd.sel.slotInSel  = sid;
//        else     fd.sel.slotOutSel = sid;
//      }
//    }else if(lftClkUp){
//      bool inSlt = len(pntr - s.P) < fd.ui.slot_rad;
//    }
//  }
//}

//LavaId sidClk;
//
//LavaId nidClk;
//bool  nodeClk = false;
//
//bool dragging = false;
//
//SECTION(drag pointer)
//{
//  if(!ms.lftDn) ms.drgP = pntr;
//}
//if(!isInSlot && !isInNode)

//bool clearSelections = true;
//ms.prevDrgbox = ms.drgbox;
//SECTION(select from selection box)
//{
//  bool anyInside = false; 
//  if(lftClkUp || ms.drgbox){    // put this above the box settings?
//    TO(sz,i){
//      Node& n       =  *(nds[i]);
//      bool inside   =  isIn(n.b, ms.drgbnd);
//      n.sel        |=  inside;
//      anyInside    |=  inside;
//    }
//    //if(drgbnd.hasLen() && fd.mouse.lftDn)
//    if(ms.drgbnd.area()>1.f && fd.mouse.lftDn)
//      clearSelections = false;
//  }
//
//  if(!anyInside && lftClkUp){
//    clearSelections = true;
//  }
//
//  if(!fd.mouse.lftDn){
//    fd.sel.pri = LavaNode::NODE_ERROR;
//  }
//}

//SECTION(selection box create)
//{
//  //if(ms.drgbox || drgbnd.hasLen()){    // && lftClkUp
//  if(ms.drgbox || ms.drgbnd.area() > 1.f){
//    clearSelections=false;
//  }
//
//  if(!ms.lftDn){ ms.drgP=pntr; ms.drgbox=false; }
//
//  if(ms.drgbox)
//    ms.drgbnd = Bnd( min(ms.drgP.x, pntr.x),
//                  min(ms.drgP.y, pntr.y),
//                  max(ms.drgP.x, pntr.x),
//                  max(ms.drgP.y, pntr.y) );
//  else
//    ms.drgbnd = Bnd();
//}

//bool inAnySlt = false;
//SECTION(slot selection and connection creation)
//{
//  LavaId  inClk(LavaId::NODE_NONE, LavaId::SLOT_NONE);
//  LavaId outClk(LavaId::NODE_NONE, LavaId::SLOT_NONE);
//  for(auto& kv : fd.graph.slots)
//  {
//    LavaId sid = kv.first;                       // sid is slot id
//    Slot&    s = kv.second;
//    if(lftClkDn)
//    {  
//      bool inSlt = len(pntr - s.P) < fd.ui.slot_rad;
//      if(inSlt){
//        outClk  = sid;
//        s.state = Slot::SELECTED;
//        if(s.in) fd.sel.slotInSel  = sid;
//        else     fd.sel.slotOutSel = sid;
//        clearSelections = false;
//        inAnySlt = true;
//      }
//    }else if(lftClkUp){
//      bool inSlt = len(pntr - s.P) < fd.ui.slot_rad;
//      if(inSlt){
//        clearSelections = false;
//        inAnySlt = true;
//      }
//    }
//  }
//
//  if(!inAnySlt && lftClkUp){
//    fd.sel.slotOutSel = fd.sel.slotInSel = LavaId(0,0);
//    sel_clearSlots();
//  }
//  
//  if(fd.sel.slotInSel.sidx  != LavaId::SLOT_NONE && 
//     fd.sel.slotOutSel.sidx != LavaId::SLOT_NONE)
//  {
//    fd.lgrph.toggleCnct(fd.sel.slotOutSel, fd.sel.slotInSel);
//    fd.sel.slotOutSel = fd.sel.slotInSel = LavaId(0,0);
//
//    sel_clearSlots();
//  }
//}

//if(!inAnySlt) SECTION(node selection)
//{
//  bool inAny = false;
//  FROM(sz,i)                                                // loop backwards so that the top nodes are dealt with first
//  {
//    Node*     n = nds[i];
//    bool inNode = isIn(pntr.x,pntr.y, n->b);
//    inAny      |= inNode;
//
//    SECTION(primary selection and group selection effects)
//    {
//      if(inNode)
//      {
//        if(lftClkDn && (fd.sel.pri==LavaNode::NODE_ERROR || fd.sel.pri!=n->id) )
//        {
//          n    =  &(node_moveToFront(n->id));
//          nds  =  node_getPtrs();                  // move to the front will invalidate some pointers in the nds array so it needs to be remade
//          fd.sel.pri = n->id;
//          ms.drgP    = pntr;
//
//          TO(sz,j){ nds[j]->sel = false; }        // todo: move this out of the outer loop due to being O(n^2) - actually not O(n^2) due to the break
//          n->sel = true;
//          break;                                  // without breaking from the loop, a node could be moved down and hit again
//        }
//        else if(lftClkUp)
//        {
//          TO(sz,j){ nds[j]->sel = false; }        // todo: move this out of the outer loop due to being O(n^2) - actually not O(n^2) due to the break
//          n->sel          = true;
//          clearSelections = false;
//          break;                
//        }
//      }
//    }
//  }
//
//  if(!inAny)
//  {
//    if(fd.mouse.lftDn){ 
//      clearSelections = false;
//    }
//
//    if(lftClkDn && fd.sel.pri==LavaNode::NODE_ERROR ){ ms.drgbox=true; }
//    if(fd.mouse.rtDn && !fd.mouse.prevRtDn){ fd.sel.sec = LavaNode::NODE_ERROR; }
//  }else{
//    clearSelections = false;
//  }
//}
//
//if(clearSelections && lftClkUp){ 
//  sel_clear();
//}

//
//if(lftClkDn && (fd.sel.pri==LavaNode::NODE_ERROR || fd.sel.pri!=n->id) )

//Bnd  drgbnd;
//
//for(auto& kv : grph.slots())

//
//TO(paths.size(),i) printf("\n %llu : %s \n", i, paths[i].c_str() );

//auto       paths  =  GetRefreshPaths();
//auto   livePaths  =  GetLivePaths(paths);
//
//// coordinate live paths to handles
//auto liveHandles  =  GetLiveHandles(fd.flow.libs, livePaths);
//
//// free the handles
//auto   freeCount  =  FreeLibs(liveHandles); 
//
//// delete the now unloaded live shared library files
//auto    delCount  =  RemovePaths(livePaths);
//
//// copy the refresh paths' files
//auto   copyCount  =  CopyPathsToLive(paths); 
//
//// load the handles
//auto loadedHndls  =  LoadLibs(livePaths);
//
//// put loaded handles into LavaFlow struct
//TO(livePaths.size(), i){
//  auto h = loadedHndls[i];
//  if(h){
//    fd.flow.libs[livePaths[i]] = h;
//  }
//}
//
//// extract the flow node lists from the handles
//auto flowNdLists = GetFlowNodeLists(loadedHndls);
//
//// extract the flow nodes from the lists and put them into the multi-map
//TO(livePaths.size(),i)
//{
//  LavaNode* ndList = flowNdLists[i];
//  if(ndList){
//    auto const& p = livePaths[i]; 
//    fd.flow.flow.erase(p);                              // delete the current node list for the livePath
//    for(; ndList->func!=nullptr; ++ndList){             // insert each of the LavaFlowNodes in the ndList into the multi-map
//      fd.flow.nameToPtr.erase(ndList->name);
//      fd.flow.nameToPtr.insert( {ndList->name, ndList} );
//      fd.flow.flow.insert( {p, ndList} );
//    }
//  }
//}

//for(auto& t : fd.flowThreads)
//{
//}

//fd.flow.start();
//fd.flowThreads.emplace_back([](){
//  //printf("\n running thread \n");
//  //
//  //printf("\n union size: %d \n", (i32)sizeof(LavaOut::key) );
//  ////printf("\n union size: %d \n", (i32)sizeof(LavaOut::key.bytes) );
//  //printf("\n union size: %d \n", (i32)sizeof(LavaOut::key.slot) );
//  //printf("\n union size: %d \n", (i32)sizeof(LavaOut::key.listIdx) );
//  //printf("\n union size: %d \n", (i32)sizeof(LavaOut::key.frame) );
//  //printf("\n packet size: %d \n", (i32)sizeof(LavaPacket) );
//
//  fd.flow.loop();
//});

//fd.flow.stop();                                   // this will make the 'running' boolean variable false, which will make the the while(running) loop stop, and the threads will end
//for(auto& t : fd.flowThreads){
//  t.join();
//}
//fd.flow.stop(); 

//
//LavaGraph     strToGraph(str const& s)

//bool            saveFile(GraphDB const& g, str path)
//bool            loadFile(str path, GraphDB* out_g)

//io_rad = IORAD;
//
// nodes
//Node&     n0 = fd.grph.addNode( Node("one", Node::FLOW, {400.f,300.f}) );
//Node&     n1 = fd.grph.addNode( Node("two",   Node::FLOW, {200.f,500.f}) );
//Node& n2 = fd.grph.addNode( Node("three", Node::FLOW, {700.f,500.f}) );
//Node& n3 = fd.grph.addNode( Node("four",  Node::FLOW, {700.f,700.f}) );
//Node& n4 = fd.grph.addNode( Node("five",  Node::MSG,  {200.f,200.f}) );
//
//n4.b.ymx = n4.b.xmx;
//
// slots
//GraphDB::Id ls0 = fd.grph.addSlot( Slot(n0.id,false) );
//LavaFlowSlot s0 = slot_add(inst0, Slot(inst0,true) );
//GraphDB::Id ls1 = fd.grph.addSlot( Slot(n1.id,true) );
//LavaFlowSlot s1 = slot_add(inst1, Slot(inst1,false) );
//Id s2 = fd.grph.addSlot( Slot(n2.id,  true) );
//Id s3 = fd.grph.addSlot( Slot(n3.id,  true) );
//Id s4 = fd.grph.addSlot( Slot(n0.id, false) );
//Id s5 = fd.grph.addSlot( Slot(n4.id, false) );
//
//fd.grph.toggleCnct(ls0, ls1);
//fd.grph.toggleCnct(s0, s2);
//fd.grph.toggleCnct(s0, s3);
//
//auto  pi = fd.lf.flow.find("FileToString");                                  // pi is pointer iterator
//if(pi != end(fd.lf.flow))
//  return fd.lgrph.addNode(pi->second, true);
//else
//  return LavaFlowNode::NODE_ERROR;

//fd.grph.addNode( Node(fn->name, Node::FLOW, {100,100}), true);
//node_add("FileToString", Node( str("New: ") + fn->name) );
//node_add("FileToString");

//// replace their nodes
//TO(loadedHndls.size(),i) if(loadedHndls[i]!=0)
//{
//  fd.lf.flow.find
//}
//
// coordinate the node structures with node Ids
//
//TO(pths.size(),i) 
//
//for(auto& p : pths) printf("\n %s \n

//if(premult)
// glClearColor(0,0,0,0);
//else

//auto      nodeSlots(vec_nptrs const& nds) -> vec_ids
//auto            slot_all() -> vec_ids
//{
//  using namespace std;
//
//  auto& slots = fd.graph.slots;
//  auto& nodes = fd.graph.nds;
//
//  vec_ids sidxs;                                            // sidxs is slot indexes
//  for(auto& kv : nodes)
//  {                                                         // np is node pointer and nds is nodes
//    auto nid = kv.first;
//    auto  si = lower_bound(ALL(slots), nid, [](auto a,auto b){ return a.first < b; } );          // si is slot iterator
//    if(si != end(slots)  &&  si->first.nid == nid){
//      Slot& s = si->second;
//      sidxs.push_back(si->first);        
//    }
//  }
//  return sidxs;                                        // RVO
//}
//
//str           graphToStr(GraphDB const& g)
//
//str           graphToStr(FisData::Graph const& fg, LavaGraph const& lg)
//GraphDB       strToGraph(str const& s)

//GraphDB* grphPtr = &fd.grph;
//flowBtn->setCallback([grphPtr](){
//  grphPtr->addNode( Node("new node", Node::FLOW) );
//});
//msgBtn->setCallback([grphPtr](){
//  grphPtr->addNode( Node("message node", Node::MSG) );
//});

//
//auto node_add(str node_name, Node n=Node("",Node::FLOW,v2(0,0)) ) -> uint64_t;

//grph.toggleCnct(fd.sel.slotOutSel, fd.sel.slotInSel);
//
//grph.clearSlotSels();
// todo: put back sel_clearSlots()

//auto di = grph.srcCnctsMap().begin();                                    // di is destination iterator
//auto en = grph.srcCnctsMap().end();
//for(auto di = grph.srcCnctsMap().begin(); di != en; )
//
//Slot const& src = *(grph.slot(srcIdx));
//Slot const& src = g.slot(srcIdx);

//
//n    =  &(grph.moveToFront(n->id));

//Slot const& dest = *(grph.slot(destIdx));
//f32 w = grph.node(destIdx.id).b.w();
//
//Slot const& dest = *(g.slot.find(destIdx));
//
//continue;
//draw_cnct(vg, src.P, dest.P, src.N, dest.N, NODE_SZ.x/2);

// ++cnt
//if(!grph.slot(avgIter->second)){ continue; }
//
//Slot const& dest = *(grph.slot(avgIter->second));

//void        node_idslots(u64 nid)
//
//auto si = lower_bound(ALL(fd.graph.slots), Id(nid), [](auto a,auto b){ return a.first < b; } );

//for(; sIter!=grph.slotEnd() && sIter->first.id==n.id; ++sIter)
// .nodeSlots(n.id);

//auto sIter = grph.nodeSlots(n.id);            // sIter is slot iterator
//for(; sIter!=grph.slotEnd() && sIter->first.id==n.id; ++sIter)
//{
//  auto     sIdx = sIter->first;                    // todo: needs to be redone
//  Slot const& s = *(grph.slot(sIdx));
//  bool   inSlot = len(pntr - s.P) < fd.ui.slot_rad; //io_rad;
//
//  Slot::State drawState = Slot::NORMAL;
//  if(s.state==Slot::SELECTED) drawState = Slot::SELECTED;
//  else if(inSlot)             drawState = Slot::HIGHLIGHTED;
//  slot_draw(vg, s, drawState, fd.ui.slot_rad);
//}

//GraphDB& grph = fd.grph;
//LavaFlow& grph = fd.lgrph;

//clear_selections(&grph, &fd);
// todo: make sel_clear()

// !lftDn && prevLftDn){ 
//clear_selections(&grph, &fd);
//clear_selections(&grph, &fd);

//void           sel_clear(GraphDB* inout_grph, FisData* inout_fd)
//{
//  inout_grph->clearSels();
//  inout_fd->sel.slotOutSel = Id(0,0);
//  inout_fd->sel.slotInSel  = Id(0,0);
//  inout_fd->sel.pri        =  -1;
//  inout_fd->sel.sec        =  -1;
//
//  for(auto& kv : fd.graph.nds){
//    kv.second.sel = false;
//  }
//}

//
//LavaId          slot_add(u64 nidx, Slot s)

//ls.id = nidx;
//
//return ls;
//
//ls.state = (LavaFlowSlot::State)((u64)s.state);

//_s = graphToStr();
//glfwSetWindowTitle(win, _s.c_str() );
//FILE* f = fopen("nanovg_test.lava", "w");
//fwrite(_s.c_str(), 1, _s.size(), f);
//fclose(f);

//FILE* f = fopen("nanovg_test.lava", "r");
//fseek(f, 0, SEEK_END);
//_s.resize( ftell(f) );
//fseek(f, 0, SEEK_SET);
//fread( (void*)_s.data(), 1, _s.size(), f);
//fclose(f);
//strToGraph(_s);

//#ifdef _WIN32
//  auto     paths = GetRefreshPaths();
//  auto livePaths = GetLivePaths(paths);
//
//  // coordinate live paths to handles
//  auto liveHandles  =  GetLiveHandles(fd.lf.libs, livePaths);
//
//  // free the handles
//  auto   freeCount  =  FreeLibs(liveHandles); 
//
//  // delete the now unloaded live shared library files
//  auto    delCount  =  RemovePaths(livePaths);
//
//  // copy the refresh paths' files
//  auto   copyCount  =  CopyPathsToLive(paths); 
//
//  // load the handles
//  auto loadedHndls  =  LoadLibs(livePaths);
//
//  // put loaded handles into LavaFlow struct
//  TO(livePaths.size(), i){
//    auto h = loadedHndls[i];
//    if(h){
//      fd.lf.libs[livePaths[i]] = h;
//    }
//  }
//
//  // extract the flow node lists from the handles
//  auto flowNdLists = GetFlowNodeLists(loadedHndls);
//
//  // extract the flow nodes from the lists and put them into the multi-map
//  TO(livePaths.size(),i)
//  {
//    LavaFlowNode* list = flowNdLists[i];
//    if(list){
//      auto const& p = livePaths[i]; 
//      fd.lf.flow.erase(p);                            // delete the current node list for the livePath
//      for(; list->func!=nullptr; ++list){             // insert each of the LavaFlowNodes in the list into the multi-map
//        fd.lf.flow.insert( {p, list} );
//      }
//    }
//  }
//
//  // delete interface buttons from the nanogui window
//  fd.ui.ndBtns.clear();
//  
//  // redo interface node buttons
//  for(auto& kv : fd.lf.flow){
//    LavaFlowNode* fn = kv.second;                      // fn is flow node
//    auto       ndBtn = new Button(fd.ui.keyWin, fn->name);
//    ndBtn->setCallback([fn](){ 
//      fd.grph.addNode( Node(fn->name, Node::FLOW, {100,100}), true);
//    });
//  }
//  fd.ui.screen.performLayout();
//
//  //// replace their nodes
//  //TO(loadedHndls.size(),i) if(loadedHndls[i]!=0)
//  //{
//  //  fd.lf.flow.find
//  //}
//
//  // coordinate the node structures with node Ids
//
//  //TO(pths.size(),i) 
//
//  TO(paths.size(),i) printf("\n %llu : %s \n", i, paths[i].c_str() );
//  //for(auto& p : pths) printf("\n %s \n
//#endif

//fd.lf.nids.clear();
//TO(fd.lf.nids.size(),i){ 
//  fd.lf.nids
//}

//Node  n = node(id);
//auto prevOrder = n.order;     
//
// addNode will get the next order number
// n.order = nxtOrder();
//
//if(n->type==Node::NODE_ERROR) return errorNode();
//
//m_nodes.erase(prevOrder);     // todo: use a delNode here instead
//m_ids.erase(id);
//
//return addNode(n, false);
//
//fd.graph.ordr.erase( {idx, n.order} );
//fd.graph.ordr.erase( {idx, n.order} );
//n.order = fd.graph.ordr.rbegin()->order + 1;

//f32   w  = grph.node(destIdx.nid).b.w();
//draw_cnct(vg, src.P, avgP, src.N, midN, NODE_SZ.x/2);
//
//Slot const& dest = *(grph.slot(di->second));
//LavaFlowSlot const& dest =  *(fd.lgrph.slot(di->second));     // *(grph.slot(di->second));
//LavaFlowSlot const& dest =
//
//draw_cnct(vg, avgP, dest.P, -1.f*midN, dest.N, NODE_SZ.x/2);

//using Id = GraphDB::Id;
//using Id = LavaGraph::Id;

//if(lftClkUp){
//  //if(fd.sel.pri == n->id) n->sel = true;
//  fd.sel.pri = -1;
//}
//
//n   = &(grph.moveToFront(n->id));
//nds = grph.nodes();                   // move to the front will invalidate some pointers in the nds array so it needs to be remade

//n->sel     = true;
//if(!n->sel){
//  TO(sz,j){ nds[j]->sel = false; }      // todo: move this out of the outer loop due to being O(n^2) - actually not O(n^2) due to the break
//  n->sel = true;
//}

//if(!n->sel){
//fd.sel.pri = -1;
//fd.sel.pri = n->id;
//}
//clearSelections = false;

//
//fd.sel.pri = -1;

//if(n->sel){
//  fd.sel.pri      = -1;
//  clearSelections = false;
//  break;
//}

//if(!n->sel){
//  TO(sz,j){ nds[j]->sel = false; }      // todo: move this out of the outer loop due to being O(n^2) - actually not O(n^2) due to the break
//  n->sel = true;
//  fd.sel.pri = -1;
//  //fd.sel.pri = n->id;
//}
//clearSelections = false;

//else if(lftClkUp){
//  fd.sel.pri = -1;
//}
//
//if(!lftClkUp)
//  clearSelections=false;
//else
//
//if(lftClkUp){ fd.sel.pri = -1; }
//}else if(lftClkUp){         // if(lftClkDn && lftClkUp){
//  //auto tmp = fd.sel.pri = -1;
//  //clear_selections(&grph, &fd);

//
//if(fd.sel.pri >= 0){ fd.graph.nds[fd.sel.pri].sel = true; }

//auto    nds = grph.nodes();
//auto     sz = grph.nsz();
//
//auto     sz = fd.graph.nds.size();  // nds.nsz();
//u64       i = 0;
//vec_ndptrs nds(sz,nullptr);
//for(auto& kv : fd.graph.nds){
//  nds[i++] = &kv.second;
//}

//auto            node_add(str node_name, Node n=Node("",Node::FLOW,v2(0,0)) ) -> uint64_t
//
//auto      pi = fd.lf.flow.find( node_name );                                  // pi is pointer iterator
//auto instIdx = LavaFlowNode::NODE_ERROR;
//if( pi != end(fd.lf.flow) )
//  return fd.lgrph.addNode(pi->second, true);
//
//n.txt = node_name;
//n.txt = "new node";

//else
//  return LavaFlowNode::NODE_ERROR;

//auto clr = fd.ui.nd_color;    // NODE_CLR;
//if(selctd){ clr = fd.ui.nd_selclr; }  //nvgRGBf(.5f,.4f,.1f); }
//
//n.b = node_draw(vg, 0, n, clr, 1.f, fd.ui.nd_border);

//f32 w=n.b.w(), h=n.b.h();
//v2       wh = {w,h};
//v2      hlf = { n.b.xmx/2, n.b.ymx/2 };
//v2      hlf = NODE_SZ/2;

//v2   hlf  =  NODE_HALF_SZ;
//
//v2  circCntr = (pdir.x<0)? nP+v2(rad,rad)  :  nP + NODE_SZ - v2(rad,rad);  // NODE_SZ
//bool     hit = !hasInf(intrsct)  &&  (intrsct.x < nP.x+rad || intrsct.x > nP.x + NODE_SZ.x - rad); 

//
//f32  rad = NODE_SZ.x/2;  // n.b.xmx/2.f;   // NODE_SZ.x/2; // n.b.xmx/2.f;   // NODE_SZ.x/2;

//switch(s.state){
//switch(s.state){

//f32 msgRad = NODE_SZ.x / 2;
//f32 msgRad = n.b.w() / 2.f;

//nvgCircle(vg, x+w/2,y+h/2, msgRad);
//
//nvgCircle(vg, x+w/2,y+h/2, msgRad);

//b = {cntrX-msgRad/1.2f, cntrY-msgRad/1.2f, cntrX+msgRad/1.2f, cntrY+msgRad/1.2f};
//f32 mn = cntrX-msgRad, mx = cntrX+msgRad;
//b = {mn, mn, mx, mx};
//b = n.b;

//nvgText(vg, x+w*0.5f-tw*0.5f+iw*0.25f,y+h*0.5f-1, n.txt.c_str(), NULL);
//nvgText(vg, x+w*0.5f-tw*0.5f+iw*0.25f,y+h*0.5f,n.txt.c_str(), NULL);

//#ifdef _WIN32
//  //LoadSharedLibraries();
//  //HMODULE lib = LoadLibrary(TEXT("lava_ReadFile.dll"));
//  if(lib)
//  {
//    auto getNds = (GetLavaFlowNodes_t)GetProcAddress(lib, TEXT("GetLavaFlowNodes") );
//    LavaFlowNode* nds = getNds();
//    if(nds) printf("%s    %s \n\n", nds[0].name, nds[0].out_types[0] );
//
//    auto flowFunc = nds[0].func;
//
//    if(flowFunc) printf("flow func %llu : \n\n", flowFunc(nullptr, nullptr) );
//    else         printf("flow func is nullptr \n\n");
//
//    //printf("%s    %s    %s", nds[0].name, nds[0].in_types[0], nds[0].out_types[0] );
//    //while(nds && nds->name)
//      //node_add( (nds++)->name );
//  }else{ printf("zero"); }
//  //}else{ printf("zero", lib); }
//#endif

//#ifdef _WIN32
//      HMODULE lib = LoadLibrary(TEXT("ReadFile.dll"));
//      if(lib){
//        auto   getNds = (GetLavaNodes_t)GetProcAddress(lib, TEXT("GetNodes") );
//        LavaNode* nds = getNds();
//        printf("%s    %s    %s", nds[0].name, nds[0].in_types[0], nds[0].out_types[0] );
//        //while(nds && nds->name)
//        //node_add( (nds++)->name );
//      }else{ printf("zero"); }
//      //}else{ printf("zero", lib); }
//#endif

//sprintf(sngl, "sizeof LavaData %d", sizeof(LavaData) );
//glfwSetWindowTitle(win, sngl);    

//
//printf("sizeof LavaData %lld \n", sizeof(LavaData) );

//FisData* fd = (FisData*)glfwGetWindowUserPointer(window);
//fd->ui.screen.charCallbackEvent(codepoint);

//nvgRGBAf( .15f, .15f,  .15f,   .95f ),
//nvgRGBAf( .2f, .2f,    .2f,   1.f)  );
//
//nvgRGBAf( .3f,   .3f,   .3f,  0.5f), 
//nvgRGBAf( .15f,  .15f,  .15f,  .45f) );

//f32 cntrX = (n.b.xmn+n.b.xmx)/2;
//f32 cntrY = (n.b.ymn+n.b.ymx)/2; 
//
//NVGcolor col,
//
//float tw=0, iw=0, x=n.P.x, y=n.P.y, w=NODE_SZ.x, h=NODE_SZ.y;
//float cntrX=x+w/2, cntrY=y+h/2, rr=rad;      // rr is rail radius

//fd.grph.addCnct(Id(1,1), Id(1,1));
//fd.grph.addCnct(Id(1,1), Id(2,1));
//fd.grph.addCnct(Id(1,1), Id(3,1));

//bool lftClk = (fd.mouse.lftDn && !fd.mouse.prevLftDn);
//bool    clk = lftClk || rtClk;

//nvgRGBAf( .15f, .15f,  .15f,   .95f ),
//nvgRGBAf( .2f, .2f,    .2f,   1.f)  );

//bool              insUnq(cnct_tbl* cnct, int a, int b)   // insUnq is insert unique - this inserts into a multi-set only if the combination of int key and int val does not already exist
//{
//  using namespace std;
//  
//  auto rnge = cnct->equal_range(a);
//  if(rnge.first != rnge.second){
//    for(auto i=rnge.first; i!=rnge.second; ++i){
//      if( i->second == b ) return false;
//    }
//  }
//  cnct->insert( {a, b} );
//
//  return true;
//}

//bool                 drgNd = false;
//v2                    drgP;
//v2                 drgofst;
//bool                drgbox = false;

//bool                  rtDn = false;    // right mouse button down
//bool                 lftDn = false;    // left mouse button down
//bool              prevRtDn = false;    // right mouse button down
//bool             prevLftDn = false;    // left mouse button down

//v2                prevPntr;
//

//char              winTitle[TITLE_MAX_LEN];
//
//int                premult = 0;
//
//Bnd                   nbnd;
//
//float              ndOfstX;
//float              ndOfstY;
//
//float                  ndx = 512.f;
//float                  ndy = 512.f;
//
//Id               slotInSel;
//Id              slotOutSel;
//int                 pri = -1;
//int                 sec = -1;
//f32                 io_rad;

//
//bool highlight=false, bool selected=false)

//px=(float)cx; py=(float)cy;
//prevX=px; prevY=py; 

//#ifdef _WIN32
//  HMODULE lib = LoadLibrary(TEXT("TfmTestLib.dll"));
//  if(lib){
//    auto   getNds = (GetLavaNodes_t)GetProcAddress(lib, TEXT("GetNodes") );
//    LavaNode* nds = getNds();
//    sprintf(sngl, "%s    %s    %s", nds[0].name, nds[0].in_types[0], nds[0].out_types[0] );
//    while(nds && nds->name)
//      node_add( (nds++)->name );
//  }else{ sprintf(sngl, "zero", lib); }
//
//  glfwSetWindowTitle(win, sngl);
//#endif

//
//int font = nvgCreateFont(vg, "sans-bold", "Roboto-Bold.ttf");

//using namespace glm;
//
//FisData* fd = (FisData*)glfwGetWindowUserPointer(window);

//auto sz = nodes.size();
//
//TO(sz,i) nd_txt.add(g.node(i).txt);
//TO(sz,i) nd_txt.add(nodes[i].txt);
//
//TO(sz,i) nd_x.add(g.node(i).P.x);
//
//TO(sz,i) nd_y.add(g.node(i).P.y);
//
//TO(sz,i) nd_type.add(g.node(i).type);
//
//Jzon::Node ordr = Jzon::array();
//TO(sz,i) ordr.add(nd_ordr[i]);
//
//nds.add("order",  ordr);

//void            node_add(str txt, Node::Type type=Node::FLOW)
//{
//  //v2 P = v2(512,512);
//  Node n(txt, type);
//  n.P = v2(512,512);
//  //nodes.push_back(n);
//
//  //nodes.push_back( {P,txt} );
//  //nodes.back().type = type;
//
//  //sels.push_back(false);
//  //nbnds.emplace_back();
//
//  //nd_ordr.push_back( (int)(nodes.size()-1) );
//}

// draw rail
//void            drw_rail(NVGcontext* vg, v2 P, v2 nP)                     // drw_rail is draw_rail
//{
//  using namespace std;
//  
//  const f32    hPi = PIf/2;      // hPi is half pi 
//  const int border = 2;
//  const f32   rthk = 8.f;        // rthk is rail thickness
//
//  f32 rnd = 1.f;
//  f32 tw=0, iw=0, x=nP.x, y=nP.y, w=NODE_SZ.x, h=NODE_SZ.y;
//  v2   hlf = NODE_HALF_SZ;
//  f32 vMid = nP.y +  NODE_SZ.y/2;
//  f32  rad = hlf.y; 
//  f32   rr = NODE_SZ.y/2;          // rr is rail radius
//  f32 rlen = NODE_SZ.x/2;          // rlen is rail length
//  
//  v2 ndCntr = {nP.x + hlf.x, nP.y + hlf.y};
//
//  bool leftSide = P.x < ndCntr.x;
//  v2    lftCirc = { nP.x+rad, ndCntr.y };
//  v2    rgtCirc = { nP.x+NODE_SZ.x-rad, ndCntr.y };
//
//  bool inLeftCircle = P.x < (nP.x + rad);
//  bool  inRgtCircle = P.x > (nP.x+NODE_SZ.x - rad);
//  bool        onTop = P.y < ndCntr.y;
//  enum Place { BOTTOM=0, LEFT, TOP, RIGHT };
//  Place place = BOTTOM;
//  if(inLeftCircle)     place = LEFT;
//  else if(inRgtCircle) place = RIGHT;
//  else if(onTop)       place = TOP;
//
//  v2 dir;
//  if(inLeftCircle)      dir = norm(P-lftCirc);
//  else if(inRgtCircle)  dir = norm(P-rgtCirc);
//  else dir = norm(P-ndCntr);
//
//  f32 railRad, arcSt, lftDist, lftArc, lftOpp, lftArcSt=hPi, rgtDist, rgtArc, rgtOpp, rgtArcSt=hPi; 
//  railRad = rlen/2;
//
//
//  if(inLeftCircle)     arcSt = PIf - asin(dir.y);
//  else if(inRgtCircle) arcSt = asin(dir.y);
//
//  if(inLeftCircle) lftArcSt = PIf - asin(dir.y);
//  lftDist = (inLeftCircle || inRgtCircle)? 0 : max(0.f, min(railRad, P.x-(nP.x+rad)) );
//  lftArc  = min(hPi*3 - lftArcSt, (railRad-lftDist) / rad );          // maximum arc amount is half a circle, which is PI radians
//  if(inRgtCircle){
//    lftArc  = max(0.f, min(hPi - lftArcSt, (railRad-lftDist)/rad) );
//  }
//  lftOpp  = railRad - lftArc*rad - lftDist;                           // lftOpp is left opposite side segment length
//
//  if(inRgtCircle) rgtArcSt = asin(dir.y);
//  rgtDist = inRgtCircle? 0 : max(0.f, min(railRad, (nP.x+NODE_SZ.x-rad)-P.x) );
//  rgtArc  = min(rgtArcSt + hPi, (railRad-rgtDist)/rad );              // maximum arc amount is half a circle, which is PI radians
//  rgtOpp  = railRad - rgtArc*rad - rgtDist;                           // rgtOpp is right opposite side segment length
//
//
//  //if(inLeftCircle)     arcSt = PIf - asin(dir.y);
//  //else if(inRgtCircle) arcSt = asin(dir.y);
//
//  Bnd nb(nP.x, nP.y, nP.x+NODE_SZ.x, nP.y+NODE_SZ.y);
//  
//  // relative distances
//  f32 lDst = max(0.f, min(railRad, (P.x-(nb.xmn+rad))) );
//  f32 rDst = max(0.f, min(railRad, ((nb.xmx-rad)-P.x)) );
//  f32 lArc = (railRad-lDst) / rad;
//  f32 rArc = (railRad-rDst) / rad;
//  f32 lOpp = railRad - lDst - lArc*rad;
//  f32 rOpp = railRad - rDst - rArc*rad;
//
//  if(inLeftCircle){
//    f32 ang = P.y>vMid?  acos(P.x)  :  PIf+acos(P.x);
//    lArc = min(hPi*3.f - ang, railRad);
//    rArc = min(ang - hPi, railRad);
//  }
//
//  // absolute positions
//  //v2  lArcPt =  node_border(nP, dir, io_rad);
//
//  v2  lPt    = { P.x - lDst, P.y };
//  v2  rPt    = { P.x + rDst, P.y };
//  f32 lArcSt = hPi + lArc;
//  f32 lArcEn = hPi; 
//  f32 rArcSt = hPi;
//  f32 rArcEn = hPi - rArc;
//  v2  lOppPt = { nb.xmn + rad + lOpp , nb.ymn };
//  v2  rOppPt = { nb.xmx - rad - rOpp , nb.ymn };
//  v2  lArcPt = { acos(lArcSt), asin(lArcSt) };
//  v2  rArcPt = { acos(rArcSt), asin(rArcSt) };
//
//  if(inLeftCircle){
//    
//  }
//
//  //SECTION(drawing fourth attempt)
//  //{
//  //  // maybe the rounded rectangle should be broken down into one number that represents a position on the border
//  //  // then taking the number backwards and forwards would at least give the start and end points 
//  //
//  //
//  //  v2 orthDir = { dir.y, -dir.x };
//  //  v2 lBrdr = node_border(nP, orthDir, io_rad);
//  //
//  //  nvgStrokeWidth(vg, rthk);
//  //  nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, 1.f));
//  //
//  //  nvgBeginPath(vg);
//  //    nvgMoveTo(vg, P.x, P.y);
//  //    nvgLineTo(vg, lBrdr.x, lBrdr.y);
//  //    //nvgRoundedRect(vg, nP.x, nP.y, NODE_SZ.x, NODE_SZ.y, NODE_SZ.y/2 );
//  //    //nvgRect(vg, nP.x, nP.y, NODE_SZ.x, NODE_SZ.y); 
//  //  nvgStroke(vg);
//  //}
//
//  //SECTION(drawing third attempt)
//  //{
//  //  nvgStrokeWidth(vg, rthk);
//  //  nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, 1.f));
//  //
//  //  nvgBeginPath(vg);
//  //    if(lOpp>0){
//  //      nvgMoveTo(vg, lOppPt.x, lOppPt.y);
//  //    }else if(lArc>0){
//  //      nvgMoveTo(vg, lArcPt.x, lArcPt.y);
//  //      nvgArc(vg, lftCirc.x, lftCirc.y, rad, lArcSt, lArcEn, NVG_CCW);
//  //    }else{
//  //      nvgMoveTo(vg, lPt.x, lPt.y);
//  //    }
//  //    if(rDst>0) nvgLineTo(vg, rPt.x, rPt.y);
//  //
//  //    if(rArc>0){
//  //      nvgArc(vg, rgtCirc.x, rgtCirc.y, rad, rArcSt, rArcEn, NVG_CCW);
//  //    }
//  //    if(rOpp>0) nvgLineTo(vg, rOppPt.x, rOppPt.y);
//  //
//  //    //if(lArc>0) nvgArc(vg, lftCirc.x, lftCirc.y, rad, lArcSt, lArcEn, NVG_CCW);
//  //  nvgStroke(vg);  
//  //}
//
//  //SECTION(drawing second attempt)
//  //{
//  //  nvgStrokeWidth(vg, rthk);
//  //  nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, 1.f));
//  //
//  //  nvgBeginPath(vg);  // left side
//  //    nvgMoveTo(vg, P.x, P.y);
//  //    //switch(place){      
//  //    if(onTop && !inLeftCircle)
//  //    { 
//  //      if(lftDist>0){ nvgLineTo(vg, P.x - lftDist, P.y); }
//  //      if(lftArc>0)   nvgArc(vg, lftCirc.x, lftCirc.y, rad, hPi*3, hPi*3 - lftArc, NVG_CCW);
//  //      if(lftOpp>0)   nvgLineTo(vg, nP.x+rad+lftOpp, nP.y+NODE_SZ.y);
//  //    }
//  //    else{
//  //      if(lftDist>0){ nvgLineTo(vg, P.x - lftDist, P.y); }
//  //      if(inLeftCircle){
//  //        if(lftArc>0) nvgArc(vg, lftCirc.x, lftCirc.y, rad, arcSt, arcSt + lftArc, NVG_CW);
//  //        if(lftOpp>0) nvgLineTo(vg, nP.x+rad+lftOpp, nP.y);
//  //      }else if(inRgtCircle){
//  //        if(lftArc>0) nvgArc(vg, rgtCirc.x, rgtCirc.y, rad, arcSt, arcSt + lftArc, NVG_CW);
//  //        if(lftOpp>0) nvgLineTo(vg, nP.x+rad+lftOpp, nP.y);
//  //      }else{
//  //        if(lftArc>0) nvgArc(vg, lftCirc.x, lftCirc.y, rad, hPi, hPi + lftArc, NVG_CW);
//  //        if(lftOpp>0) nvgLineTo(vg, nP.x+rad+lftOpp, nP.y);
//  //      }
//  //    }
//  //    //}
//  //  nvgStroke(vg);
//  //
//  //  nvgBeginPath(vg);  // left side
//  //  nvgMoveTo(vg, P.x, P.y);
//  //  //switch(place){      
//  //  if(onTop && !inRgtCircle)
//  //  { 
//  //    if(rgtDist>0){ nvgLineTo(vg, P.x + rgtDist, P.y); }
//  //    if(rgtArc>0)   nvgArc(vg, rgtCirc.x, rgtCirc.y, rad, -hPi, -hPi + rgtArc, NVG_CW);
//  //    if(rgtOpp>0)   nvgLineTo(vg, nP.x+rad+rgtOpp, nP.y+NODE_SZ.y);
//  //  }
//  //  else{
//  //    if(rgtDist>0){ nvgLineTo(vg, P.x + rgtDist, P.y); }
//  //    if(inRgtCircle){
//  //      if(rgtArc>0) nvgArc(vg, rgtCirc.x, rgtCirc.y, rad, arcSt, arcSt - rgtArc, NVG_CCW);
//  //    }else{
//  //      if(rgtArc>0) nvgArc(vg, rgtCirc.x, rgtCirc.y, rad, hPi, hPi - rgtArc, NVG_CCW);
//  //    }
//  //    if(rgtOpp>0) nvgLineTo(vg, nP.x+NODE_SZ.x-rad-rgtOpp, nP.y);
//  //  }
//  //  //}
//  //  nvgStroke(vg);
//  //}
//
//  //f32 rad = lerp(rnd, 0.f, h/2.f);                               // rad is corner radius
//  //f32 cntrX = x+border+rad, cntrY = y+border+h/2, 
//  //f32 rr=rad;        // rr is rail radius
//
//  //v2  hlf = NODE_HALF_SZ;
//  //v2 ndir = norm(nP - P);
//  //v2 pdir = ndir;
//  //v2   ds = sign(pdir);                                  // ds is direction sign
//  //f32  ax = abs(pdir.x);
//  //if( ax > hlf.x ){
//  //  pdir /= ax/hlf.x;                                    // can this never be 0, since ax is positive, hlf.x is positive, and ax is greater than hlf.x ? 
//  //}else{
//  //  f32 ay = abs(pdir.y);
//  //  pdir /= ay==0.f?  1.f  :  ay/hlf.y;
//  //}
//  //f32       r = hlf.y;
//  //v2 circCntr = (pdir.x<0)? nP+v2(r,r)  :  nP+NODE_SZ-r;
//
//  //v2 lftCirc;
//  //lftCirc.y = nP.y + hlf.y;
//  //lftCirc.x = leftSide? nP.x+rad  :  nP.x+NODE_SZ.x-rad;
//  //lftCirc.y = nP.y + hlf.y;
//
//  //f32      leftLine = P.x - rlen/2;
//  //f32 leftLineLimit = max(nP.x + rad, leftLine);
//  //f32     leftExtra = abs(leftLineLimit - leftLine);
//  ////f32       leftArc = inLeftCircle? (rlen/2)/rad  :  min(PIf*1.5f, leftExtra/rad);
//  //f32       leftArc = min(PIf*1.5f, leftExtra/rad);
//  //f32         arcSt = inLeftCircle? hPi + hPi-asin(dir.y) :  hPi;
//  //f32         arcEn = min(arcSt+leftArc, PIf*1.5f);
//  //f32       arcDist = (arcEn - arcSt) * rad;
//  //f32      leftWrap = max(0.f, leftExtra -  arcDist);
//
//  //st      = PIf - asin(dir.y);
//  //f32   lftSeg = P.x - lftDist;
//  //
//  //f32 leftLineLen = P.x - rlen/2 - leftLimit;  // nP.x + 2*rad rlen/2 - 
//  //
//  // black border
//  //f32 bthk = rthk+2;                        // bthk is black thickness 
//  //nvgBeginPath(vg);
//  //  nvgMoveTo(vg, x-bthk/2, P.y);
//  //  nvgArc(vg, P.x, P.y, rr, PIf*1.f, PIf*1.5f, NVG_CW);
//  //  nvgStrokeWidth(vg, bthk);
//  //nvgStrokeColor(vg, nvgRGBAf(0, 0, 0, 1.f) );
//  //nvgStroke(vg);
//  //
//  // inner color
//  //nvgBeginPath(vg);
//  //  nvgMoveTo(vg, x-rthk/2, P.y);
//  //  nvgArc(vg, P.x, P.y, rr, PIf*1.f, PIf*1.5f, NVG_CW);
//  //  nvgStrokeWidth(vg, rthk);
//  //  nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, 1.f));
//  //nvgStroke(vg);
//  //
//  //v2 brdr = node_border(nP, P - ndCntr, io_rad);      // brdr is border
//  //
//  //nvgBeginPath(vg);
//  //  nvgCircle(vg, P.x, P.y, 5.f);
//  //  nvgStrokeWidth(vg, rthk);
//  //  nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, 1.f));
//  //nvgStroke(vg);
//  //
//  //nvgBeginPath(vg);
//  //  nvgCircle(vg, circCntr.x, circCntr.y, 5.f);
//  //  nvgStrokeWidth(vg, rthk);
//  //  nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, 1.f));
//  //nvgStroke(vg);
//  //
//  //f32 angle = inLeftCircle? asin(dir.y) :  PIf/2;
//  //f32 arcSt = PIf/2.0f;
//  //f32 arcEn = PIf*1.5f;
//  //NVGwinding windDirection = leftSide? NVG_CW  :  NVG_CCW;
//  //if(onTop){
//  //  swap(arcSt, arcEn);
//  //  windDirection = NVG_CCW;
//  //}
//
//  //SECTION(drawing first attempt)
//  //{
//  //  nvgBeginPath(vg);
//  //    //if(inLeftCircle){
//  //    //  nvgMoveTo(vg, P.x, P.y);
//  //    //  if(leftArc>0){
//  //    //    nvgArc(vg, circCntr.x, circCntr.y, rad, arcSt, arcEn, windDirection);    // PIf*1.f, PIf*1.5f, NVG_CW);
//  //    //    if(leftWrap>0)
//  //    //      nvgLineTo(vg, nP.x+rad+leftWrap, nP.y);
//  //    //  }
//  //    //}else{
//  //    //if(leftSide){
//  //
//  //    //if(onTop && !inLeftCircle){
//  //    //  nvgMoveTo(vg, P.x, P.y);
//  //    //  nvgLineTo(vg, leftLineLimit, P.y);
//  //    //  nvgArc(vg, circCntr.x, circCntr.y, rad, arcEn, arcSt, NVG_CCW);      // PIf*1.f, PIf*1.5f, NVG_CW);
//  //    //  if(leftWrap>0)
//  //    //    nvgLineTo(vg, nP.x+rad+leftWrap, nP.y+NODE_SZ.y);
//  //    //}else{
//  //      nvgMoveTo(vg, P.x, P.y);
//  //      if(!inLeftCircle){ nvgLineTo(vg, leftLineLimit, P.y); }
//  //      if(leftArc>0){
//  //        if(onTop) nvgArc(vg, circCntr.x, circCntr.y, rad, arcEn, arcSt, NVG_CCW);
//  //        else      nvgArc(vg, circCntr.x, circCntr.y, rad, arcSt, arcEn, windDirection);
//  //        if(leftWrap>0)
//  //          nvgLineTo(vg, nP.x+rad+leftWrap, nP.y);
//  //      }
//  //    //}
//  //    //}
//  //    //}else if(leftSide && onTop){
//  //    //  
//  //    //}
//  //
//  //    //nvgMoveTo(vg, leftLineLimit, P.y);
//  //    //nvgLineTo(vg, P.x, P.y);
//  //    //nvgArc(vg, circCntr.x, circCntr.y, rr, arcSt, arcEn, windDirection);    // PIf*1.f, PIf*1.5f, NVG_CW);
//  //    nvgStrokeWidth(vg, rthk);
//  //    nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, 1.f));
//  //  nvgStroke(vg);
//  //}
//  //
//  //f32 arcEn = leftSide? PIf*1.5f  :  -PIf*0.5f;
//  //if(leftSide==false){ swap(arcSt, arcEn); }
//
//  //nvgBeginPath(vg);
//  //  nvgCircle(vg, nP.x, nP.y, 5.f);
//  //  nvgStrokeWidth(vg, rthk);
//  //  nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, 1.f));
//  //nvgStroke(vg);
//  //
//  //nvgBeginPath(vg);
//  //  nvgCircle(vg, P.x, P.y, 5.f);
//  //  nvgStrokeWidth(vg, rthk);
//  //  nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, 1.f));
//  //nvgStroke(vg);
//}

//GraphDB::Id id;
//printf("\n sizeof(GraphDB::Id): %d \n", sizeof(GraphDB::Id) );

//node_add("new node");
//
//node_add("message node", node::MSG);

//auto sIter = grph.nodeSlots(ndOrdr);            // sIter is slot iterator
//
//auto     sIdx = sIter->second;                    // sIdx is slot index

//
//nvgTextAlign(vg,  NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);  // NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);

//auto sz = cncts.size();
//TO(sz,i) src.add(cncts[i].src);
//TO(sz,i) dest.add(cncts[i].dest);

//FisData* fd = (FisData*)glfwGetWindowUserPointer(window);
//fd->ui.screen.resizeCallbackEvent(w, h);

//if(hit) *out_nrml = norm(circCntr - borderPt);
//
//  && !hasNaN(intrsct);
//
//f32        r = hlf.y;
//
//if(hit && borderPt.x > (nP.x+rad) && borderPt.x < (nP.x-rad) ){
//  if(ds.y > .5f) borderPt.y = nP.y + NODE_SZ.y;
//  else           borderPt.y = nP.y;
//}

//nvgBeginPath(vg);
//  nvgRect(vg, b.xmn, b.ymn, b.w(), b.h());
//nvgStroke(vg);

//if(preicon != 0){
//	nvgFontSize(vg, h*1.3f);
//	nvgFontFace(vg, "icons");
//	nvgFillColor(vg, nvgRGBA(255,255,255,96));
//	nvgTextAlign(vg,NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
//	nvgText(vg, x+w*0.5f-tw*0.5f-iw*0.75f, y+h*0.5f, cpToUTF8(preicon,icon), NULL);
//}

// lineCircleIntersect
//f32       r = NODE_SZ.y/2.f;
//v2  dirCirc = abs(pdir) / r;
//v2 circCntr = n.P + NODE_SZ - r;

//auto bkt = find(rnge.first, rnge.second, );
//if(bkt!=rnge.second){
//  cnct->insert( {a, b} );
//  return true;
//} 

//TO(grph.selsz(),i) grph.sel(i,false);
//grph.sel(ndOrdr,true);
//
//clearSelections = true;

//if(inNode && clk && (priSel<0||priSel!=ndOrdr) )
// priSel!=ndOrdr) )
//
//priSel     = ndOrdr;
//
//if(!grph.sel(ndOrdr)){

//int  ndOrdr = grph.order(i);
//Node&     n = grph.node(i);
//bool inNode = isIn(pntr.x,pntr.y, grph.bnd(ndOrdr) );

//if( lftClkUp && !(priSel>0) ){
//  clearSelections=false;
//}

//if( (drgbnd.w()+drgbnd.h()) > 0 ){
//  clearSelections=false;
//}

//i32  inClk = -1;
//i32 outClk = -1;
//
//TO(grph.ssz(), i)
//auto   nid = kv.first.id;
//
//lftDn && !prevLftDn)
//Slot&    s = *(grph.slot(i));
//
//outClk  = (i32)nid;
//
//Slot&    s = *(grph.slot(i));

//else{
//slotOutSel = slotInSel = Id(0,0);
//grph.clearSlotSels();
//}

//auto sz = fd.grph.nsz();
//
//int  ndOrdr = fd.grph.order(i);            // nd_ordr[i];
//if( !(ndOrdr < grph.nsz()) ){ continue; }
//
//auto      n = fd.grph.node(ndOrdr);
//bool selctd = ndOrdr==priSel || fd.grph.sel(ndOrdr);
//
//float round = secSel==ndOrdr? 0 : 1.f;
//fd.grph.bnd(ndOrdr) = node_draw(vg, 0, n, clr, round);

//f32  halfx = lerp(.5f, src.P.x, dest.P.x);
//f32  halfy = lerp(.5f, src.P.y, dest.P.y);
//f32   dist = len(src.P - dest.P);
//v2  outNxt = src.P  + src.N  * min(NODE_SZ.x/2, (dist/3));         // divide by 3 because there are 3 sections to the bezier
//v2   inNxt = dest.P + dest.N * min(NODE_SZ.x/2, (dist/3));

//nvgBeginPath(vg);
//  nvgMoveTo(vg,   src.P.x, src.P.y);
//  nvgBezierTo(vg, outNxt.x,outNxt.y, inNxt.x,inNxt.y, dest.P.x,dest.P.y);
//  nvgStrokeWidth(vg, 3.f);
//  nvgStrokeColor(vg, nvgRGBAf(.7f, 1.f, .9f, .5f));
//nvgStroke(vg);

//f32  halfx = lerp(.5f, src.P.x, avgP.x);
//f32  halfy = lerp(.5f, src.P.y, avgP.y);
//f32   dist = len(src.P - avgP);
//v2  outNxt = src.P  + src.N * min(NODE_SZ.x/2, (dist/3));         // divide by 3 because there are 3 sections to the bezier
//v2   inNxt = avgP   +  midN * min(NODE_SZ.x/2, (dist/3));

//nvgBeginPath(vg);
//  nvgMoveTo(vg,   src.P.x, src.P.y);
//  nvgBezierTo(vg, outNxt.x,outNxt.y, inNxt.x,inNxt.y, avgP.x, avgP.y);
//  nvgStrokeWidth(vg, 3.f);
//  nvgStrokeColor(vg, nvgRGBAf(.7f, 1.f, .9f, .5f));
//nvgStroke(vg);

//f32  halfx = lerp(.5f, dest.P.x, avgP.x);
//f32  halfy = lerp(.5f, dest.P.y, avgP.y);
//f32   dist = len(avgP - dest.P);
//v2  outNxt = dest.P  + dest.N * min(NODE_SZ.x/2, (dist/3));         // divide by 3 because there are 3 sections to the bezier
//v2   inNxt = avgP + -1.f*midN * min(NODE_SZ.x/2, (dist/3));
//nvgBeginPath(vg);
//  nvgMoveTo(vg,   dest.P.x,dest.P.y);
//  nvgBezierTo(vg, outNxt.x,outNxt.y, inNxt.x,inNxt.y, avgP.x, avgP.y);
//  nvgStrokeWidth(vg, 3.f);
//  nvgStrokeColor(vg, nvgRGBAf(.7f, 1.f, .9f, .5f));
//nvgStroke(vg);

//auto en = grph.cnctEnd();
//for(auto const& ci : grph.cncts())
//for(auto ci = grph.cnctBegin(); ci != en; )
//for(auto di : grph.srcCnctsMap())  // multi-map of src->dest connections, di is destination iterator 

//avgN += norm(dest.P
//
//f32  halfx = lerp(.5f, dest.P.x, src.P.x);
//f32  halfy = lerp(.5f, dest.P.y, src.P.y);
//f32   dist = len(src.P - dest.P);
//
//nvgMoveTo(vg, avgP.x, avgP.y);
//
//nvgBezierTo(vg, outNxt.x,outNxt.y, inNxt.x,inNxt.y, dest.P.x,dest.P.y);

//auto     srcIdx = ci->second;
//auto      count = grph.cncts().count(ci->first);

//auto srcInt  = srcId.get(i).toInt();
//auto destInt = destId.get(i).toInt();

//auto st = src.get(i).getType();
//auto dt = dest.get(i).getType();
//auto srcInt  = src.get(i).toInt();
//auto destInt = dest.get(i).toInt();

//Id srcId, destId;
//srcId.asInt  = srcInt;
//destId.asInt = destInt;
//g.addCnct(srcId, destId);
