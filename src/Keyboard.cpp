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
    //D fflush(stdout);
    int mod = glutGetModifiers();
    if (mod != 0) //ALT=4  SHIFT=1  CTRL=2
      {
            switch(mod)
            {
               case 1 :  D printf("SHIFT key %d\n",mod);  break;
               case 2 :
                  D printf("CTRL  key %d\n",mod);
                  if (key == 1) cliScreenSelectAll();
                  if (key == 21) cliScreenUnSelectAll();
                  if (key == 4) cliScreenDeleteSelected(); cadRedraw();
                  break;
               case 4 :  D printf("ALT   key %d\n",mod);  break;
               mod=0;
            }
            return;
      }
      switch(key)
      {
        case 97 : cliPush("a"); break;
        case 98 : cliPush("b"); break;
        case 99 : cliPush("c"); break;
        case 100 : cliPush("d"); break;
        case 101 : cliPush("e"); break;
        case 102 : cliPush("f"); break;
        case 103 : cliPush("g"); break;
        case 104 : cliPush("h"); break;
        case 105 : cliPush("i"); break;
        case 106 : cliPush("j"); break;
        case 107 : cliPush("k"); break;
        case 108 : cliPush("l"); break;
        case 109 : cliPush("m"); break;
        case 110 : cliPush("n"); break;
        case 111 : cliPush("o"); break;
        case 112 : cliPush("p"); break;
        case 113 : cliPush("q"); break;
        case 114 : cliPush("r"); break;
        case 115 : cliPush("s"); break;
        case 116 : cliPush("t"); break;
        case 117 : cliPush("u"); break;
        case 118 : cliPush("v"); break;
        case 119 : cliPush("w"); break;
        case 120 : cliPush("x"); break;
        case 121 : cliPush("y"); break;
        case 122 : cliPush("z"); break;

        case 48 : cliPush("0"); break;
        case 49 : cliPush("1"); break;
        case 50 : cliPush("2"); break;
        case 51 : cliPush("3"); break;
        case 52 : cliPush("4"); break;
        case 53 : cliPush("5"); break;
        case 54 : cliPush("6"); break;
        case 55 : cliPush("7"); break;
        case 56 : cliPush("8"); break;
        case 57 : cliPush("9"); break;

        case 45 : cliPush("-"); break;

        case 8 : cliBackup(); break; //Normal backspace
        case 127 : cliBackup(); break; //Mac delete
        case 27 : cliBackup(); break; //Escape

        case 46 : cliPush("."); break;

        case 32 : cliPush(" "); break;

        case 13 : cliReturn(); break;

        case 96 :
          cadSetColor(RED);
          cadDrawLine(point_t{-6,-8}, point_t{5,2});
          break;
      }
      //D printf("(keyboardCallbackNormal) CmdInput = %s\n", CmdInput.c_str());
      //uiEdit(0, uiEntity{UI_TEXT, WHITE, "> " + CmdInput, UI_CMD_POSITION});
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
