# ProfileCAD
2D/3D CAD/CAM Software

The Problem
-----------
There's a ton and a half of CAD CAM Solution on the web. Iv'e personally tried dozens. I'm a machinist and I use Mastercam on a daily basis to get work done. I think Mastercam V8 is great, the newer versions are better but I prefer V8. It's a fully functional solution, Draw your part, create your toolpaths, post it to your machine! Done, BAM! ready to run. Out of the dozens of Open Source solutions I have yet to find a practical one stop solution without detrimental quirks. In fact, If you prefer OSX or Linux to the horrible garbage Microsoft sells as an operating system, your work flow probably looks something like this LibreCAD=>Inkscape=>SVGtoDXF=>MakeCAMSomethingourother=>PythonGCodeUncrashFixerThing=>SomeKindOfOtherParserThing. Maybe not that bad, but its not good. So a complete solution like Master Cam costs big money and is only available on Windows.

The Goal
--------
Create not a clone to Mastercam V8, but a complete CAD/CAM solution thats comparable to Mastercam's feature set and can support itself as a respectable piece of software. I want it to be completely cost free and limitation free for hobbyist, small businesses, or anyone that needs such a thing. ProfileCAD at its core is just a Javascript engine written in C++ that knows how to draw lines and arcs using OpenGL and wxWidgets. All user interaction is coded in Javascript, which the Engine interprets. In this manner it makes developing new features, plugins, and well really anything easier and faster. Instead of recompiling, press ctrl-r and see the changes you made to any of the scripts right away.

Completed
--------
Simple 2d drawing tools are mostly working correctly

Todo
----
1. Toolbar with view functions (Fit to screen, Plane Selections, etc...)
2. Settings dialog -- Background color, etc ...
3. Tool Library
5. Toolpath generation on contours
6. Javascript nc post processors
7. Javascript file converters (save and open)
8. Entity Layer Support

Building
--------
profileCAD uses make to make building easy. Should compile on any platform architecture combination that required libraries have been ported to. Iv'e done testing on Linux, OSX, and Windows. I develop primarily on Linux.

I'm not much for having a ton of libraries to build against and I wanna keep this simple as possible with as few dependencies as i can.

Right now to build you need 5 libraries

WxWidgets<br>
OpenGL<br>

Build Instructions
------------------------
make<br>
./ProfileCAD<br>
