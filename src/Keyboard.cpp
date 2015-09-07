#include <Keyboard.h>

using namespace std;

string CmdInput = "";

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
        case 97 : CmdInput.append("a"); break;
        case 98 : CmdInput.append("b"); break;
        case 99 : CmdInput.append("c"); break;
        case 100 : CmdInput.append("d"); break;
        case 101 : CmdInput.append("e"); break;
        case 102 : CmdInput.append("f"); break;
        case 103 : CmdInput.append("g"); break;
        case 104 : CmdInput.append("h"); break;
        case 105 : CmdInput.append("i"); break;
        case 106 : CmdInput.append("j"); break;
        case 107 : CmdInput.append("k"); break;
        case 108 : CmdInput.append("l"); break;
        case 109 : CmdInput.append("m"); break;
        case 110 : CmdInput.append("n"); break;
        case 111 : CmdInput.append("o"); break;
        case 112 : CmdInput.append("p"); break;
        case 113 : CmdInput.append("q"); break;
        case 114 : CmdInput.append("r"); break;
        case 115 : CmdInput.append("s"); break;
        case 116 : CmdInput.append("t"); break;
        case 117 : CmdInput.append("u"); break;
        case 118 : CmdInput.append("v"); break;
        case 119 : CmdInput.append("w"); break;
        case 120 : CmdInput.append("x"); break;
        case 121 : CmdInput.append("y"); break;
        case 122 : CmdInput.append("z"); break;

        case 27 : EXIT; //Escape
        case 13 :
          //D point_t angle = sceneGetViewAngle();
          //D printf("Screen angle is x: %.6f y: %.6f z: %.6f\n", angle.x, angle.y, angle.z);
          if (CmdInput == "line")
          {
            cadDrawLine(point_t{0, 0, 0}, point_t{2,2,2});
            cadRedraw();
          }
          CmdInput = "";
          break; //Return
      }
      //D printf("(keyboardCallbackNormal) CmdInput = %s\n", CmdInput.c_str());
      //
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
