# ProfileCAD
3D CAD/CAM Software

Why spend the time when Fusion360 is free?
-----------
I use Fusion360 on almost a daily basis at my day job, A machine & fab shop which I (Travis Gillin) Own, and it does it's job but it's frustrating. It's buggy, takes
forever to start up, crashes almost on a daily basis, the Fusion cloud is painfully slow, never ending updates yet I never notice any functional improvements, and it only supports windows which I don't care for at all and is the only piece of software that keeps me tethered to the operating system that I least enjoy using.

A little history
----------------
The ProfileCAD project started several years ago and was originally written in C/C++ using WxWidgets (and a bunch of other attempts, checkout the branches...) for platform cross-compatibility but I struggled to make it stable and the project stalled right around the time that my CNC Plasma Cutter Machine manufacturing business started to take off and their was a rather large demand for a 2D CAD system that is quick, easy to learn and use and have really good DXF support. It's cloud-based and runs in the browser as well as offline via Electron and is used by a bunch of my customers on a daily basis, [here it is if you want to check it out](https://jetcad.io)

The Goal
--------
Build on-top of JetCad's success, use most of it's drafting tools code for a likewise workflow but completely redesign the renderer so it's 3D and create all the drafting tools necessary to create 3D models from 2D geometry

Features
1. Include an CAD workbench with a part model tree for creating 3D geometry.
2. Include an CAM workbench for creating toolpaths on 2D or 3D geometry with a workflow alike HSM Works CAM
3. Include an Assembly workbench where parts can be assembled and animated with joints.

How to build
------------
Clone this repository to a project folder.
Run "npm install"
Mac - "sudo npm install --unsafe-perm=true --allow-root"
Then "npm start" to launch the Application via Electron (Formerly Atom-Shell)
