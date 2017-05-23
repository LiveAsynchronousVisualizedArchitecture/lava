



// -TODO(Chris): Resize sidebar on window resize
// -todo: fix sidebar not changing with window resize
// -todo: put GLFW window pointer and nuklear context pointer into VizData
// -todo: remove shapes when keys are missing
// -todo: will need to remove keys that aren't in VizData too
// -todo: need simdb.getVersion() - using VerStr for versioned strings
// -todo: need simdb::VerIdx and simdb::VerKey structs
// -todo: make keys check for version before get()
// -todo: add timed key update in main loop
// -todo: add const char* key get() to simdb
// -todo: add timed key version check 
// -todo: figure out why empty string is making it into the db - not making it in, but nxtKey doesn't 
// -todo: check if timing is behind more than 2 updates and wipe out the extra
// -todo: test with updating geometry from separate process
// -TODO: Control camera with mouse
// -todo: make updates by all keys respect previous visibility
// -todo: make IndexedVerts single header with split declaration and implementation sections
// -todo: get working uvs and images
// -TODO: Add all attributes
// -todo: get single pixel images working with compositing
// -todo: figure out why tri key disapears  when using gradient - seems like the number of blocks searched was not doing signed math and was returning a bogus large number, meaning that on wrap around, the number searched would go over the number of blocks even if few blocks had actually been searched
//       - cube, and tri keys 
//       - leftTriangle also dissapears when using original generators
//       - if all the keys shows up the first time, why would one not show up the second time?
//       - off by one error in getKeyStrs() when it loops around?
//       - leftTriangle's return from put() is 0 - this is the block index, not the index in the hash map
// -todo: fix key strings being cut off before the end of the nuklear sidebar - larger width on nk_layout_row_static doesn't cut off the text, but it adds a horizontal scroll bar and clicking keys no longer works - rect.w-25.f seems to work
// -todo: fix camera rotation resetting on mouse down
// -todo: make wrapAngle use modulo operator so that remainder is kept and the angle is not clamped - 2*PI stored as const static, fmodf used to get modulo of two floats, giving the remainer
// -todo: get visualizer compiling on osx
// -todo: get visualizer running without segfault on osx
// -todo: get simdb working on osx - will have to use mmap(SHARED)
// -todo: try closing file after mmap - seems to work
// -todo: take camera position directly out of the transformation matrix
// -todo: make mouse delta a per frame change and not an accumulated changed
// -todo: separate mouse delta and camera sensitivity 
// -todo: take camera functions out of render shape
// -todo: fix Y switching rotation if camera is rotated 180 degrees with X - Y rotation needs to be relative to camera position
// -todo: scale sensitivity based on the circumfrence of a circle with the radius of the camera distance to origin
// -todo: figure out why quads don't work - quads don't work with drawElements, ChanImg conversion changed to use triangles
// -todo: figure out why location returns -1 - now returns 0
// -todo: try debugging gradient plane object in update_test - quads no drawn by glDrawElements
// -todo: rename VizDataStructures to just VizData
// -todo: reverse vertical rotation
// -todo: make vertical rotation relative to the camera - only need the Y value, so always rotate (0,Y,dist) around X axis and take the Y value from that, the real rotation will come from the lookat function
// -todo: cap change in distance - scroll callback revamped to take into account distance and yoffset
// -todo: make 'h' revert camera settings 
// -todo: reverse scroll distance control
// -todo: put back pan 
// -todo: add panning to right mouse button - probably by multing x and y vectors by the cameras transformation matrix, then adding those vectors to the cameras position
// -todo: fix far clipping plane - not sure what the problem is yet - seems fixed with the re-done camera - might have been a projection issue
// -todo: work out file locking so there is no race condition on two programs creating mmaped files - possibly fixed in simdb
// -todo: make pan sensitivity vary with scale of cam.dist
// -todo: fix camera pan when rotated - needed to lock the pan transform correctly?
// -todo: fix panning after rotation again - decomposed into quaternion which is then conjugated? and turned into a matrix
// -todo: make 'f' create a bounding box of all the active shapes and focus the camera on them
//- todo: invert Y - FOV is specified in radians, so a 75 or 80 field of view was inverting the camera
// -todo: fix rotation to not flip on the other side - just needed to reverse rotation around the Y axis completely
// -todo: fix lack of color - color was fine, input was at fault
// -todo: fix memory leak - has to do with crash? - has to do with memory mapped pages coming into the process' memory space? - if that is the case, how does memory rise above the size of the memory mapped file? - at least partially due to the destructor not being run in the move constructor
// -todo: fix versions not being updated, leading to constant updates 
// -todo: fix dissapearing keys on updates - is the problem that active is not being kept? - if keys are updating quickly, does erasing unused keys remove keys even if they were just updated? - the update version number was from the database, while the version checked in erase...  was the key from getDbStrs()
// -todo: fix further memory leak? version not being put into ivbuf_to_shape ? - version is now updated from the database and dbKeys have their versions being updated  as well
// -todo: fix keys disappearing - erase was doing a binary search that also compared versions, which were always different
// -todo: find memory leak when rapidly updating a key - Shape::vertbuf not being deleted in destructor
// -todo: fix crash in nuklear assertion that only happens in release mode - only happens when panning while rays are updating?
// -todo: add control to set line and particle thickness
// -todo: make separate settings struct under viz data - can just use the ui struct for this?

