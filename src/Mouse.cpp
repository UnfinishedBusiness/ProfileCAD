#include <Mouse.h>

using namespace std;

int mod;
int button;
mouse_t mouseLast;
void mouseInit()
{
  glutMouseFunc(mouseCallback);
  glutMotionFunc(mouseMotionCallback);
}
void mouseCallback(int btn, int state, int x, int y)
{
    button = btn;
    //D printf("(mouseCallback) btn = %d\n", btn);
    mod = glutGetModifiers();
    if ((btn == 3) || (btn == 4)) // It's a wheel event
    {
      if (btn == 3) //Zoom in
      {
        sceneIncZoom(+0.1);
      }
      else //Zoom out
      {
        sceneIncZoom(-0.1);
      }
      D printf("Scroll %s At %d %d\n", (btn == 3) ? "Up" : "Down", x, y);
    }
    if(btn==GLUT_LEFT_BUTTON && state==GLUT_DOWN && mod == GLUT_ACTIVE_CTRL)
    {
        D printf("Left + Ctrl button at X: %d, Y: %d\n", x, y);
    }
    if(btn==GLUT_LEFT_BUTTON && state==GLUT_DOWN && mod != GLUT_ACTIVE_CTRL)
    {
        D printf("Left button at X: %d, Y: %d\n", x, y);
    }
    if(btn==GLUT_RIGHT_BUTTON && state==GLUT_DOWN)
    {
        D printf("Right button at X: %d, Y: %d\n", x, y);
    }
}
void mouseMotionCallback(int x, int y)
{
    if (button == 1) //Middle mouse button
    {
      mouse_t inc{x - mouseLast.x , y - mouseLast.y};
      D printf("\rPan %d, %d", inc.x, inc.y);
      D fflush(stdout);
      if (abs(inc.x) < 10  && abs(inc.y) < 10)
      {
        sceneIncPan(inc.x * 0.01, (-1 * inc.y) * 0.01 , 0);
      }
    }
    else if (mod == GLUT_ACTIVE_CTRL)
    {
      mouse_t inc{x - mouseLast.x , y - mouseLast.y};
      D printf("\rOrbit %d, %d", inc.x, inc.y);
      D fflush(stdout);
      if (abs(inc.x) < 10  && abs(inc.y) < 10)
      {
        sceneIncViewAngle(inc.y, -1 * inc.x, 0);
      }
    }
    else
    {
      D printf("\rMouse %d, %d", x, y);
      D fflush(stdout);
    }
    mouseLast.x = x;
    mouseLast.y = y;
}
