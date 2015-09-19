#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
#include <iostream>
#include <Menu.h>

using namespace std;

void menuInit()
{
  glutCreateMenu(menuCallback);
  glutAddMenuEntry("Repaint", 1);
  glutAddMenuEntry("Toggle Snap Center", 2);
  glutAddMenuEntry("Toggle Snap Enpoints", 3);
  glutAddMenuEntry("Toggle Snap Midpoint", 4);

  glutAttachMenu(GLUT_RIGHT_BUTTON);
}
void menuCallback(int choice)
{
  switch (choice)
  {
  case 1: cadRedraw(); break;
  case 2: mouseToggleCenterSnap(); break;
  case 3: mouseToggleEndpointSnap(); break;
  case 4: mouseToggleMidpointSnap(); break;
  }
}
