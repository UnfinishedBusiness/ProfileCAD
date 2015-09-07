#include <Keyboard.h>

using namespace std;

void keyboardInit()
{
  glutSpecialFunc( keyboardCallbackSpecial );
  glutKeyboardFunc( keyboardCallbackNormal );
}
void keyboardCallbackNormal(unsigned char key, int x, int y)
{
    // Press ALT or  SHIFT or  CTRL in combination with other keys.
    D printf("Key -> %d \n",key);
    int mod = glutGetModifiers();
    if (mod != 0) //ALT=4  SHIFT=1  CTRL=2
      {
            switch(mod)
            {
               case 1 :  D printf("SHIFT key %d\n",mod);  break;
               case 2 :  D printf("CTRL  key %d\n",mod);  break;
               case 4 :  D printf("ALT   key %d\n",mod);  break;
               mod=0;
            }
      }
      switch(key)
      {
        case 27 : cout << "Bye!\n"; exit(0); //Escape
        case 13 :
          point_t angle = sceneGetViewAngle();
          printf("Screen angle is x: %.6f y: %.6f z: %.6f\n", angle.x, angle.y, angle.z);
          break; //Return
      }
}
void keyboardCallbackSpecial(int key, int x, int y)
{
  int mod = glutGetModifiers();
  switch (key)
  {
       case 27 :      break;
       case 100 : D printf("GLUT_KEY_LEFT %d\n",key); if (mod == GLUT_ACTIVE_CTRL) sceneIncViewAngle(0, -5, 0); break;
       case 102: D printf("GLUT_KEY_RIGHT %d\n",key); if (mod == GLUT_ACTIVE_CTRL) sceneIncViewAngle(0, +5, 0); break;
       case 101   : D printf("GLUT_KEY_UP %d\n",key); if (mod == GLUT_ACTIVE_CTRL) sceneIncViewAngle(-5, 0, 0); break;
       case 103 : D printf("GLUT_KEY_DOWN %d\n",key); if (mod == GLUT_ACTIVE_CTRL) sceneIncViewAngle(+5, 0, 0); break;
  }
}
