#ifndef _application_
#define _application_

#define WindowTitle "ProfileCAD"
#define Version "1"
#define WindowWidth 1280
#define WindowHeight 800

#define OriginOffsetX (WindowWidth/2)
#define OriginOffsetY (WindowHeight/2)

float ViewRatio = 0.10;

float MouseX, MouseY;
int ActiveTool;


//Tools
#define NoTool 0
#define LineTool 1

//LineTool
int LineClickStep = 0;
float LineStart[2];
float LineEnd[2];

#endif

