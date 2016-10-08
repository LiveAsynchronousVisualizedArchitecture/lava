


// -todo: write load for ColorSamples
// -todo: Load ColorSamples
// -todo: get openGL to work 
// -todo: convert Shape and VertexBuffer classes
// -todo: write out ColorSamples again
// -todo: try JSON
// -todo: visualize ColorSamples
// -todo: try binary and portable binary cereal archives
// -todo: test getting a function out of a .dll
// -todo: test getting a struct out of a .dll
// -todo: make a data type struct with a  
//   -string type, string name, ui8 data array, ui64 sizeBytes, ui64 id, ui64 version, ?parameters?
// -todo: split out visualization into a .dll
// -todo:  -load .dll with windows api - juce API
// -todo:  -put in type, version, and visualization function
// -todo:  test visualization .dll by passing in ColorSamples pointer and filling IndexedVerts, then Destroying them
// -todo:  integrate vector of juce lib objects
// -todo: use a struct in Viz.h and have a cleanup function since this data will be passed to the GPU
// -todo: try copying new .dll to a temp location, and loading the new new .dll
// -todo: change that binaries launched from different places and have different paths
// -todo: copy .dll to temp, then mv tmp to live,
// -todo: loop while file is still being written
// -todo: check so shape doesn't draw if there are no vertex buffers? why is the screen white with no viz.dll?
// -todo: make shared lib class that can keep track of file status - doesn't track status
// -todo: unloading .dll, then hot loading another if the file has changed?
// -todo:  -will it need it's own struct to keep state information like vertex buffer ids ?
// -todo: split out Shape into .hpp
// -todo: split out DynLib into .hpp
// -todo: make sure DynLib doesn't check file lastModification for more than reload minimum
// -todo: test interface for available visualizations
// -todo: create test properties box
// -todo: create open gl component
// -todo: update visualization in thread safe manner
// -todo: extend DynLib to be a timer
// -todo: create TimedCall class
// -todo: integrate TimedCall into MainComponent for hot reloading
// -todo: integrate properties box into layout
// -todo: fill properties box from viz plugins
// -todo: fill properties box info from type and viz_type information
// -todo: make shapes use their visible field
// -todo: rename moved to owned and reverse it's value in VertexBuffer
// -todo: make a struct to hold all of Properties
// -todo: add listener and hook it up to a visible flag on Shape s
// -todo: split VizComponent into separate file
// -todo: put back locks so that updates always go through?
// -todo: split out main component into separate file
// -todo: make shape a vector
// -todo: break down ReloadLibsInto separate functions
// -todo: build double buffered data into concurrency.hpp
// -todo: make another visualization in ColorSampls_viz, have it make red points on error?  let it use depth first, directions and ray length?
// -todo: figure out why original visualization becomes detached - not firing valueChanged event - not deconstructing the original component memory
// -todo: predeclare all DataStore classes
// -todo: make a serialization util file
// -todo: find out what network connection functions are getting called and which aren't
// -todo: grab data from a network connection or pipe
// -todo: put string constructor in type string
// -todo: make data by id
// -todo: make data structure for generic data
// -todo: needs to contain a map with key "type" and a vector of bytes, maybe "name" also ?
// -todo: give data a name, type, and id
// -todo: change testPP name
// -todo: make wrapped properties panel
// -todo: split DataVizProperties out into separate file
// -todo: make properties component use collapsable names
// -todo: integrate a view of the data into visualizer GUI
// -todo: make interface for data - data opens up into visualization options?
// -todo: put layout bars into the gui
// -todo: make createVizualization(viz,data)
// -todo: cross reference visualizations with data types
// -todo: make destroyVisualization(idx)
// -todo: visibility off, destruct and free on turn off
// -todo: make a visualization command struct is VizComponents to delete or change visualizations
// -todo: use juce to load file to bytes?
// -todo: load, visualize and show on turn on 
// -todo: do unserialize and free in ColorSamples_datatype
// -todo: test ColorSamplesViz with two visualizations
// -todo: load color samples with a .dll
// -todo: load all color samples
// -todo  make dll for deserializing the data and deallocating it based on type
// -todo: find memory leak - found? must cast void pointer when using delete to insure destructor is run
// -todo: get message of types from data store
// -todo: get names and types from data store
// -todo: change viz buffers to have order
// -todo: make data server respond to specific query of a name
// -todo: make data store 
// -todo: make a separate data server project
// -todo: put paths in the gui to look for data files? - in the server
// -todo: add a text logger to the bottom of server
// -todo: change query message in visualizer to query the server
// -todo: integrate server into FileServer
// -todo: change .clrsmpl files to .ColorSamples
// -todo: connect the queries to the properties
// -todo: take extension out of DataType
// -todo: have data server load up all data files
// -todo: have data server look at names and extensions - name is name type is extension
// -todo: make a data store that can declare itself and the data it has, then send it on request
// -todo: build properties out of data server response
// -todo: hash both string and data
// -todo: build updating into the properties panel
// -todo: make more ColorSamples and combine them
// -todo: zoom in camera
// -todo: put back PortableBinary
// -todo: hash serialized data
// -todo: refine DataType.h
// -todo: set up release build targets
// -todo: make Transform.h
// -todo: make an IndexedVerts DataType
// -todo: test IndexedVerts DataType
// -todo: test remove the TestDLL project
// -todo: try creating a template project - copy paste and text replace
// -todo: update ColorSamples to use new DataType.h
// -todo: test if va_arg can be used in the C abi for constructors - yes, but not forwarded without libFFI, will libFFI do it?
// -todo: test Juce Table
// -todo: split out name-type-hash into separate .h and .cpp file
// -todo: try boost multi-map for accessing named-type-hash structures by each value as a key
// -todo: put border on lower text box - outlineColour -> 128,128,128
// -todo: integrate multi-map with DataTable
// -todo: make a generalized transform plugin from Viz.h
// -todo: make combo data with add and remove methods to TableCombo
// -todo: integrate all columns of table and mult_index
// -todo: scan for transforms
// -todo: put transforms into TableCombo box
// todo: set up ColorSamples to IndexedVerts project
// -todo: get name from transform
// -todo: get data names from files
// -todo: add id number to data table
// -todo: load data plug ins
// -todo: cull files by available types
// -todo: split up FileDataServer into DataTable and TableCombo and FDSMainComp files
// -todo: split DataTable
// -todo: split FDSMain into h and cpp
// -todo: load ColorSamples, do_transform, save IndexedVerts to memory
// -todo: put indexed verts into table
// -todo: make a simple transform .dll as a test
// -todo: generate new data on change of transform
// -todo: make indexed verts available to client
// -todo: make new data types available to the server
// -todo: build a transform into FileDataServer
// -todo: give fileserver a list of available names
// -todo: transform to a new name and type
// -todo: break apart ColorSamples_datatype
// -todo: test ColorSamples with proper transformation to IndexedVerts
// -todo: error check to make sure proper transformation exists
// -todo: unify data store of server and main program?
// -todo: take lib reloading out of DynLib - already done, the time in DynLib is a mininum reload time
// -todo: transforms: 
// -todo: need to check for data using their transform, 
// -todo: regenerate it, 
// -todo: then send an update message to the client
// -todo: make server proactivly send updated serialized data to the client
// -todo: make file server update transformation on .dll reload
// -todo: make sure data type is loaded
// -todo: make FDSMainComp.insert replace data
// -todo: make server only serve up indexed verts?
// -todo: selectivly update
// -todo: put vizComp shape updating into a function
// -todo: put vizComp openGL frame setup and cleanup into separate (const?) functions 
// -todo: test recompiling and reloading of transformation .dll
// -todo: build refreshing of .dll into transform
// -todo: split out data transform into own function?
// -todo: attach an image to IndexedVerts
// -todo: test image creation
// -todo: make image attach in openGL and shader query the image
// -todo: debug image in openGL
// -todo: make sparser point set from renderman
// -todo: make shader put texture over vertex color
// -todo: make 512 res ColorSamples
// -todo: rasterize points into IndexedVerts
// -todo: fix refresh leaves displayed points bug - keep state of properties - fixed?
// -todo: take out status to text panel
// -todo: put in nearest filtering
// -todo: take out Img and just use ChanImg
// -todo: add mode to indexed verts
// -todo: put back image over in shader
// -todo: put ChanImg to IndexedVerts into function
// -todo: output real normal values from renderman
// -todo: shift pathVn and pathNn to angle values then rasterize
// -todo: add gaussian filter 
// -todo: create an ApplyKernel function
// -todo: output 2 or 4 spp point set from renderman
// -todo: output flipped pathVn from renderman?
// -todo: make a correlation channel
// -todo: build in mode into vertex buffer
// -todo: split out image filtering into separate transform
// -todo: test chaining modification of IndexedVerts
// -todo: figure out why visualizations don't make it to visualizer - plenty of reasons
// -todo: fix out of sync properties
// -todo: recompile ColorSamples and IndexedVerts as release
// -todo: compile transform as release
// -todo: split out image filter
// -todo: fix no update bug - emplace doesn't insert if a key already exists
// -todo: make a bilateral ApplyKernel
// -todo: change sent data to be from the server cache and not from disk
// -todo: fix on off bug 
// -todo: make arbitrary size gaussian
// -todo: output ray length
// -todo: make fat ColorSamples
// -todo: make use channels list for bilateral blur
// -todo: make fat IndexedVerts image but only use first 4 channels for visualization
// -todo: make bilateral filter take just one vec<float> for kernel
// -todo: make guide image for bilateral blur
// -todo: make cross bilateral filter
// -todo: try log or gamma on samples?
// -todo: include spec in fat samples
// -todo: output dot product of pathVn and pathNn
// -todo: make apply and add functions
// -todo: try oversampled bilateral
// -todo: keep gamma rasterized color channels separate - have to rasterize twice
// -todo: try transformation with eigen?
// -todo: try to create a 3 dimensional pca version of an image
// -todo: make PCA from gammad samples
// -todo: filter original color from PCA image
// -todo: use read pixels to get the color under the cursor
// -todo: make integer downsample
// -todo: restore after crash
// -todo: output depth
// -todo: figure out how to get AOVs from full ray depth render
// -todo: try memstore
// -todo: put in write time
// -todo: make visualizer keep track of time loaded in a MemStore for comparison
// -todo: try not refreshing properties if nothing new is found
// -todo: make FileDataServer overwrite same names instead of creating new objects
//       or make it just by id and manually delete?
// -todo: clean up commented sections in MainComponent
// -todo: try visualizer refreshing multiple times per second
// -todo: try using drag and drop from the windows file explorer
// -todo: try loading files on demand from the file listing - replaced by drag and drop
// -todo: clean up FileDataServer
// -todo: strip out 
// -todo: try freeing encodings in MemStore
// -todo: make separate table spreadsheet to view number in ColorSamples and IndexedVerts
// -todo: make spreadsheet have a table of the databases
// -todo: have a popup to read the values of indexed verts? - done with spreadsheet 
// -todo: load .dlls on demand too as a foreign type arrives? - done in spreadsheet
// -todo: fix database insert bug - records are deleted and recreated, is the set_field on pre-existing record a bug?
// -todo: fix visualizer refresh bug
// -todo: fix no transforms on existing database bug in FileDataServer
// -todo: calculate new mean from from real gaussian mult formula - reweighted using variance
// -todo: try keeping the weights of the image? not neccesary because of variance?
// -todo: add constant shift to variance in img filter
// -todo: normalize vector points
// -todo: fix crash with empty FileDataServer 
// -todo: add panning to visualizer
// -todo: add reset key (space bar)
// -todo: try locking whitedb in memstore
// -todo: try openmp in rasterize
// -todo: try openmp in bilateral filter
// -todo: make guide rasterized at gamma 2.2 at regular resolution, 
// -todo: make visibility on/off by id and not names 
// -todo: fix visualization of points
// -todo: move transforms and datatype implementations to header files and global source directory 

// todo: check if a visualization should be turned on before showing
// todo: put resolution and channels of IndexedVerts in VizComp status
// todo: output roughness
// todo: make filtering happen at oversample resolution
// todo: add row number column to spreadsheet
// todo: add second slider to spreadsheet for finer scroll control
// todo: make bilinear upsampling of ChanImg
// todo: create a table visualizer plugin format? 
//       takes a data structure and rowId and columnId and gives back a string?

// idea: try stand alone program to generate continuous IndexedVerts data?
// idea: try manifold fusion with variance?
// idea: take file browser out of FileDataServer
// idea: port guided filter?
// idea: try histogram from oversampled raster
// idea: try interpolated PCA buffer?
// idea: put in data table from file dialog?
// idea: make an external logging function for debugging?
// idea: make outlier apply to windows
// idea: add table/spread sheet of values as a different visualization type
// idea: use pipes instead of sockets for IPC
// idea: add 2D camera controls
// idea: make indexed verts a flat data structure?
// idea: unify data store and NthMap?
// idea: make the query for data reconnect if disconnected
// idea: make file server destroy transformation when setting changes
// idea: will need some way of subscribing to a visualization from the server
// idea: make visualizer only look for indexed verts?
// idea: make command queue from lock free queue
//       make global transform store from BDB
//       make global datatype store from BDB
//       have multiple command queues, one for each component?
//       FROM is an int, TO is what queue it gets put into, msg/command is an INT, args are 64 bytes? args are 60 bytes?
//         -args are ids to a shared args DB ?
//       two levels to the command queue, one that uses lock free queues inside processes, one that uses the DB?
//       give each bucket 
// idea: make simple command queue that does one command per frame?
// idea: delay loading of .dlls by 1s to make gui come up faster?
// idea: load and remove on selection or drag and drop of files from the file browser?
// idea: cache DynLib transform information?
// idea: resize table cells by name size?
// idea: can va_arg be used for parameters to transforms - can't be forwarded?
// idea: make an Args header file like RslArgs using va_args to create?
// idea: make a struct of parameters for arguments to the transform?
// idea: pair an non-atomic check first and an atomic check second for more scalability in lock free concurrency
//       -check non-atomic flag, compare_exchange atomic flag, change non-atomic flag to ready=false
// idea: make every data store entry a memory mapped file so it is shared, always cached, and doesn't count towards memory?
// idea: make a red yellow green icon to show connection status?
// idea: spit MainComponent into declaration and implementation
// idea: make a TO that has a reference and an index
// idea: use drop down menus to select transforms and a final transform to IndexedVerts for visualization
// idea: have Main give a list of valid extensions to the data server - set extensions command
//   idea: maybe make the data structure the generic 'table'
//   idea: make table able to serialize itself into a vector of bytes
// idea: build datatype Destroy into a constructor
// idea: run a separate process that runs the .dll ?
//   -needs interprocess communication
//   -named pipe
//   -child process
// idea: convert visualizer to use precompiled headers
// idea: node graph: 
//   -inputs come from a data store,
//   -data transformations come from hot loaded .dlls, live C with tinyCC, or live julia
//   -outputs are visualizations
// idea: look at gaffer for a node graph
// idea: use tcc to compile a function to memory as a transform
// idea: have an image instead of a just vertices
// idea: be able to alter the vertex and fragment shaders 

#include "stdafx.h"
#include "MainComponent.hpp"
#include "Visualizer.h"

MemStore     bindb;
MemStore    namedb;
MemStore    typedb;
MemStore     srcdb;
MemStore   xformdb;
MemStore    timedb;


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()    { return new MainComponent(); }

class VisualizerApplication  : public JUCEApplication
{
public:
  VisualizerApplication() {}
  const String getApplicationName()           override { return ProjectInfo::projectName; }
  const String getApplicationVersion()        override { return ProjectInfo::versionString; }
  bool moreThanOneInstanceAllowed()           override { return true; }
  void initialise(const String& commandLine)  override
  {
    // This method is where you should put your application's initialisation code..
    
    bindb.init(    "bin",  MemStore::GB * 3);
    namedb.init(  "name",  MemStore::GB * 3);
    typedb.init(  "type",  MemStore::GB * 3);
    srcdb.init(    "src",  MemStore::GB * 3);
    timedb.init(  "time",  MemStore::GB * 3);

    mainWindow = new MainWindow(getApplicationName());
  }
  void shutdown()                             override
  {
    // Add your application's shutdown code here..

    mainWindow = nullptr; // (deletes our window)
  }
  void systemRequestedQuit()                  override
  {
      // This is called when the app is being asked to quit: you can ignore this
      // request and let the app carry on running, or call quit() to allow the app to close.
      quit();
  }
  void anotherInstanceStarted(const String& commandLine) override
  {
      // When another instance of the app is launched while this one is running,
      // this method is invoked, and the commandLine parameter tells you what
      // the other instance's command-line arguments were.
  }

  /*
      This class implements the desktop window that contains an instance of
      our MainContentComponent class.
  */
  class MainWindow    : public DocumentWindow
  {
  public:
      MainWindow(String name)  : DocumentWindow (name,
                                                  Colours::lightgrey,
                                                  DocumentWindow::allButtons)
      {
          setUsingNativeTitleBar (true);
          setContentOwned (createMainContentComponent(), true);
          setResizable (true, true);

          centreWithSize (getWidth(), getHeight());
          setVisible (true);
      }

      void closeButtonPressed() override
      {
          // This is called when the user tries to close this window. Here, we'll just
          // ask the app to quit when this happens, but you can change this to do
          // whatever you need.
          JUCEApplication::getInstance()->systemRequestedQuit();
      }

      /* Note: Be careful if you override any DocumentWindow methods - the base
          class uses a lot of them, so by overriding you might break its functionality.
          It's best to do all your work in your content component instead, but if
          you really have to override any DocumentWindow methods, make sure your
          subclass also calls the superclass's method.
      */

  private:
      JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
  };

private:
  ScopedPointer<MainWindow> mainWindow;
};

// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION(VisualizerApplication)


























//
//Component* createMainContentComponent();

//#include "Viz.h"
//#include "Shape.hpp"
//#include "DynLib.hpp"
//#include "VizComponent.hpp"

//  template<class ARG>
//struct ButtonClicked : public ButtonListener
//{
//  using FUNC = function<void(ARG)>;
//  FUNC func;
//  ButtonClicked(){}
//  ButtonClicked(FUNC _func) : func(func) {}
//  void buttonClicked(ARG arg) override 
//  {
//    func(arg);
//  }
//};

//m_vizListener       =  Clicked([ths](Button* b)
//{
//  auto boolB    =  (BooleanPropertyComponent*)(b);
//  bool checked  =  boolB->getState();
//  ths->vizComponent().setShapeVisibility(checked);
//  ths->vizComponent().repaint();
//});

//auto boolB    =  (BooleanPropertyComponent*)(button);
//bool checked  =  boolB->getState();
//bool checked  =  this->getState();        
//
//AlertWindow("hit", toString(value.getValue().isBool()),  AlertWindow::AlertIconType(), nullptr );




