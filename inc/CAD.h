#ifndef CAD_
#define CAD_
#include <application.h>
#include <Scene.h>

struct cadEntityArc{
  point_t start, end;
  float radius;
};
struct cadEntityLine{
  point_t start, end;
};
struct cadEntity{
  int Type; //Line or Arc
  color_t Color;
  cadEntityLine Line;
  cadEntityArc Arc;
};

void cadInit();
void cadAppend(cadEntity);
void cadSetColor(color_t);
void cadRender();
void cadRedraw();
cadEntity cadGetEntityArray(int);
int cadGetEntityArrayIndex();
void cadDrawLine(point_t start, point_t end);
#endif
