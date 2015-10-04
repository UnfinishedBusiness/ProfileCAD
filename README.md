# ProfileCAD
2D/3D CAD/CAM Software with a MasterCam 8 like menu driven workflow

Built in TinyG control interface

Controls
--------
Ctrl+click orbit
Mouse middle-click+drag panning
Mouse scroll zoom

Fileformats
-----------
1. pfcad < todo (native profileCAD format that saves toolpaths and dimensions)
1. dxf < done
3. stl < todo
4. iges < todo
5. sti < todo
6. step < todo

Completed
--------
Simple 2d drawing tools are mostly working correctly

Todo
----
Design a un-encrypted native file format
Settings dialog -- Background color, etc ...
Tool Library
Machine controller (Like bridgeport ez-path type thing)
Toolpath generation on contours
Lua post processors
Dialog file selector
Update the 1980's looking keyboard menu to something more "Pretty" << Josh's suggestion

Building
--------
profileCAD uses cmake to make building easy. Should compile on any platform architecture combination that required libraries have been ported to.

I'm not much for libraries and I wanna keep this simple as possible with as few dependencies as i combination

Right now to build you need 4 libraries

Freeglut
OpenGL
PNG
Lua

Linux Build Instructions
------------------------
cd build/
rm -r CMake*
cmake ../src
make
./ProfileCAD

OSX Build Instructions
------------------------
cd build_osx/
rm -r CMake*
cmake ../src
make
open ProfileCAD.app
