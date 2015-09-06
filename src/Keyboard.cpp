#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
#include <iostream>
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
    printf("Key -> %d \n",key);
    int mod = glutGetModifiers();
    if (mod != 0) //ALT=4  SHIFT=1  CTRL=2
      {
            switch(mod)
            {
               case 1 :  printf("SHIFT key %d\n",mod);  break;
               case 2 :  printf("CTRL  key %d\n",mod);  break;
               case 4 :  printf("ALT   key %d\n",mod);  break;
               mod=0;
            }
      }
      switch(key)
      {
        case 27 : cout << "Bye!\n"; exit(0); //Escape
        //case 13 : cout << "Drawing lines!\n"; CurrentMode = 3; //Return
      }
}
void keyboardCallbackSpecial(int key, int x, int y)
{
    switch (key)
    {
         case 27 :      break;
         case 100 : printf("GLUT_KEY_LEFT %d\n",key);   break;
         case 102: printf("GLUT_KEY_RIGHT %d\n",key);  ;  break;
         case 101   : printf("GLUT_KEY_UP %d\n",key);  ;  break;
         case 103 : printf("GLUT_KEY_DOWN %d\n",key);  ;  break;
    }
}
