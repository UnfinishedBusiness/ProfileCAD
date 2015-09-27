#include <Dialog.h>

#define DIALOG_BACKGROUND color_t{0.3, 0.3, 0.3, 1}
int DIALOG_WIDTH = 300;
int DIALOG_HEIGHT = 200;

using namespace std;
int dialogWindow;

void dialogResize(int width, int height)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();//these two lines are unchanged

  double bound = 1.0; //I reduced the edge distance to make the shape larger in the window
  double scaleX = double(width)/DIALOG_WIDTH;
  double scaleY = double(height)/DIALOG_HEIGHT;
  double Yoffset = 100;

  glOrtho( -bound*scaleX/2, bound*scaleX/2, -bound*scaleY, bound*scaleY, -1,1 );
  glScalef(1, DIALOG_WIDTH/DIALOG_HEIGHT + DIALOG_WIDTH/DIALOG_HEIGHT + 0.5, 1); //Not sure wtf, but it works!
  //gluPerspective(0, 16.0/9.0*float(width)/float(height), -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glViewport(0,0,width,height);
}
void dialogRender()
{
  glClearColor(DIALOG_BACKGROUND.r, DIALOG_BACKGROUND.g, DIALOG_BACKGROUND.b, DIALOG_BACKGROUND.a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void dialogClose()
{
  glutDestroyWindow(dialogWindow);
}
void dialogOpen()
{
  glutInitWindowSize(DIALOG_WIDTH, DIALOG_HEIGHT);
  dialogWindow = glutCreateWindow(WINDOW_TITLE);
  glutDisplayFunc(dialogRender);
  glutReshapeFunc(dialogResize);
}
