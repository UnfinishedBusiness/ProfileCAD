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
  //glutAddMenuEntry("Stencil on", 1);
  //glutAddMenuEntry("Stencil off", 2);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}
void menuCallback(int choice)
{
  switch (choice) {
  case 1:
    cadRedraw();
    break;
  }
}
