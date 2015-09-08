#ifndef UI_
#define UI_
#include <application.h>
#include <Scene.h>

#define UI_TEXT 0x00

#define UI_MENU_POSITION point_t{5, 10, 0}
#define UI_INPUT_POSITION point_t{5, 30, 0}

struct uiEntity{
  int Type; //Text
  color_t Color;
  std::string Text;
  point_t Position;
};

void uiInit();
void uiAppend(uiEntity);
void uiEdit(int, uiEntity);
void uiDrawText(void *, char *, float, float, float);
void uiRender();
#endif
