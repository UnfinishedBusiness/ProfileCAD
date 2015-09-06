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
  glutAddMenuEntry("Motion", 3);
  glutAddMenuEntry("Stencil on", 1);
  glutAddMenuEntry("Stencil off", 2);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}
void menuCallback(int choice)
{
  switch (choice) {
  case 3:

    //glutIdleFunc(animation);
    break;
  case 2:

    //glutSetWindowTitle("Stencil Disabled");
    //glutPostRedisplay();
    break;
  case 1:
    //stencilOn = 1;
    //glutSetWindowTitle("Stencil Enabled");
    //glutPostRedisplay();
    break;
  }
}
