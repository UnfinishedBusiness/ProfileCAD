#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
#include <iostream>
#include <Menu.h>
using namespace std;

void testOK()
{
  cout << "Test Textbox had value: " << dialogTextboxGetString("Test") << endl;
  dialogClose();
}
void Test()
{
  dialogAddLabel(point_t{-450, 320}, "Test?");
  dialogAddTextBox(point_t{-450, 200}, 500, 100, "Test");
  dialogAddButton(point_t{-450, -350}, 200, 100, "OK", testOK);
  dialogOpen();
}
void menuInit()
{
  glutCreateMenu(menuCallback);
  glutAddMenuEntry("Repaint", 1);
  glutAddMenuEntry("Toggle Snap Center", 2);
  glutAddMenuEntry("Toggle Snap Arc Enpoints", 3);
  glutAddMenuEntry("Toggle Snap Line Enpoints", 4);
  glutAddMenuEntry("Toggle Snap Midpoint", 5);
  glutAddMenuEntry("Toggle Snap Intersection", 6);
  glutAddMenuEntry("Toggle Snap Vector", 7);
  glutAddMenuEntry("Test", 8);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}
void menuCallback(int choice)
{
  switch (choice)
  {
    case 1: cadRedraw(); break;
    case 2: mouseToggleCenterSnap(); break;
    case 3: mouseToggleArcEndpointSnap(); break;
    case 4: mouseToggleLineEndpointSnap(); break;
    case 5: mouseToggleMidpointSnap(); break;
    case 6: mouseToggleIntersectionSnap(); break;
    case 7: mouseToggleVectorSnap(); break;
    case 8: Test(); break;
  }
  //uiSnapIndicator();
}
