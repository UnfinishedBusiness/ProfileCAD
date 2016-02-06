#ifndef KEYBOARD_
#define KEYBOARD_
#include <application.h>

void keyboardInit();
void keyboardCallbackNormal(unsigned char, int, int);
void keyboardCallbackSpecial(int, int, int);
#endif
