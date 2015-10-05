# ProfileCAD
2D/3D CAD/CAM Software with a MasterCam 8 like menu driven workflow

Controls
--------
Ctrl+click orbit<br>
Mouse middle-click+drag panning<br>
Mouse scroll zoom<br>

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
1. Design a un-encrypted native file format
2. Settings dialog -- Background color, etc ...
3. Tool Library
4. Machine controller (Like bridgeport ez-path type thing)
5. Toolpath generation on contours
6. Lua post processors
7. Dialog file selector
8. Update the 1980's looking keyboard menu to something more "Pretty" << Josh's suggestion

Building
--------
profileCAD uses cmake to make building easy. Should compile on any platform architecture combination that required libraries have been ported to.

I'm not much for libraries and I wanna keep this simple as possible with as few dependencies as i can

Right now to build you need 5 libraries

Freeglut<br>
OpenGL<br>
PNG<br>
Lua<br>
ZLib<br>

Linux Build Instructions
------------------------
cd build/<br>
rm -r CMake*<br>
cmake ../src<br>
make<br>
./ProfileCAD<br>

OSX Build Instructions
------------------------
cd build_osx/<br>
rm -r CMake*<br>
cmake ../src<br>
make<br>
open ProfileCAD.app<br>
