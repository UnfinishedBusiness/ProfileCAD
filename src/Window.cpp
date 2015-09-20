#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
#include <iostream>
#include <Window.h>

using namespace std;

void windowInit()
{
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  glutCreateWindow(WINDOW_TITLE);
  glutReshapeFunc(windowResize);
}
void windowResize(int width, int height)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();//these two lines are unchanged

  double bound = 1.0; //I reduced the edge distance to make the shape larger in the window
  double scaleX = double(width)/WINDOW_WIDTH;
  double scaleY = double(height)/WINDOW_HEIGHT;
  double Yoffset = 100;

  glOrtho( -bound*scaleX/2, bound*scaleX/2, -bound*scaleY, bound*scaleY, -1,1 );
  glScalef(1, WINDOW_WIDTH/WINDOW_HEIGHT + WINDOW_WIDTH/WINDOW_HEIGHT + 0.5, 1); //Not sure wtf, but it works!
  //gluPerspective(0, 16.0/9.0*float(width)/float(height), -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glViewport(0,0,width,height);
}
