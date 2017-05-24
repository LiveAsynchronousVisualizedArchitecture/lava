// -todo: compile with nanogui
// -todo: take out nuklear 
// -todo: make initital nanogui window
// -todo: fix nuklear ui disappearing when panning on rays changing - first it was a crash due to a gl function returning a null buffer and it not being checked for by nuklear - doesn't come back when resetting the camera - nuklear taken out
// -todo: replace nuklear with nanogui
// -todo: put database name/list in visualizer title
// -todo: integrate nanogui with openGL drawing
// -todo: set up events to get nanogui window to be interactive
// -todo: set drop, scroll and framebuffer callbacks for nanogui
// -todo: get release mode to work
// -todo: look into taking out ui lag
// -todo: make sidebar
// -todo: make sidebar the height of the window with a custom width
// -todo: make background color back to the original dark color
// -todo: make sidebar stick to the side
// -todo: make sidebar transparent - already is, make it more transparent
// -todo: figure out reference counting so that files are cleaned up on exit - taken care of by simdb
// -todo: turn on geometry by default to test openGL rendering - turn nanogui off to test, maybe it is drawing the screen over - works
// -todo: get opengl and nanogui to work at the same time - take out nanogui's screen drawing? - set the screen to be smaller?
// -todo: get input working to change the camera as before
// -todo: use callback for focus on sidebar to take away mouse button presses so that the camera doesn't move on the side - used window boolean for 'focused' which half works - doesn't prevent camera controls until the window is clicked
// -todo: get keys working on the side as buttons or labels
// -todo: erase key ui elements and recreate them on refresh
// -todo: have toggle button presses turn active on and off
// -todo: add fps counter in the corner - use nanovg alone?
// -todo: make heads up display font size a parameter
// -todo: add apache and bsd licenses
// -todo: add color under cursor like previous visualizer - use the gl get frame like the previous visualizer - check if the cursor is over the gl window first as an optimization? - sort of in but not working
// -todo: put vd.now as a variable loop? - isn't broke, don't fix
// -todo: change to saving indices of buttons and the combo box, so that deleting is done by index instead of in bulk
// -todo: integrate combo box into key list
// -todo: make database listing stick
// -todo: make database listing actually change on selection
// -todo: put actual db listing into db menu
// -todo: make combo box callback list the dbs
// -todo: make the sidebar re-layout on data base selection
// -todo: take out simdb_ prefix from database list
// -todo: add list of databases to select  
// -todo: make drop down menu or text field or file dialog for typing in database name - not neccesary due to listing databases and different UI approach
// -todo: is it possible to turn off vertical sync? is it possible to skip the buffer swap if not enough time has passed? - swap interval is set to 0, probably good enough for now to avoid a rabbit hole
// -todo: add list of databases currently open - only one currently open
// -todo: test switching between two different databases
// -todo: change font to normal and not bold for fps and color scanner
// -todo: make separate database referesh function that can be called on a switch, but also subtracts time from the refresh counter
// -todo: fix refresh of db to not mangle names
// -todo: build in move constructor for simdb
// -todo: make simdb_listDBs return unprefixed names
// -todo: clean out nanogui example
// -todo: sort keys
// -todo: organize nanogui globals into global states
// -todo: move declarations into VizDecl.h
// -todo: clean out done todo list and dead lines



// -todo: fix crash on focus event while db list is open - fixed by checking for refCount in the focus?
// -todo: generate .c arrays of bytes from .ttf files
// -todo: integrate font files as .c files so that .exe is contained with no dependencies
// -todo: use serialized file data with nanovg
// -todo: use serialized fonts with nanogui
// -todo: make an initial overlay text that has hotkeys and dissapears when the screen is clicked
// -todo: make title change on different database selection
// -idea: put version next to key value 
// -idea: test indexed verts with images
// -idea: ability to save indexed verts objects to a file
// -idea: make a text visualizer? - will be taken care of by table 
//       - display keys and data from db directly - keys then string on one line, click the line and add a tab with the key name in the tab title and a split window between hex and string form?
//       - display values and strings
// -idea: make a strings binary format - will this work for an arbitrary packed binary list? - should there be a data structure type and an underlying data type? - should be taken care of by tbl.hpp
//       - first 8 bytes -> total size in bytes
//       - next  4 bytes  -> data structure type? - binary list here?
//       - next  4 bytes  -> underlying data type?
//       - next  8 bytes  -> number of strings
//       - for the number of strings -> 8 bytes for the offset of each string from the start of the whole binary 
//       - next bytes are all string data - does the encoding matter here? should it be utf8 since you know the length of each string?
// -idea: work out a type enum for lava data structures? use the upper 16 bits of the size? this leaves 'only' 65,536 different types and 281 terabytes as the max size - use first 8 bytes for size and next 8 bytes for version? - put magic number in the upper 16 bits and type in a 'type' key 


