#ifndef CAD_
#define CAD_
#include <application.h>
#include <Scene.h>

struct cadEntity{
  int Type; //Line or Arc
  color_t Color;
  line_t Line;
  arc_t Arc;

  //Operational data
  bool Selected;
  point_t SelectedAt;
  bool SelectedBody;
  bool Removed;
  int Index;
};

void cadInit();
void cadAppend(cadEntity);
void cadEdit(int, cadEntity);
void cadRemoveSelected();
std::vector<cadEntity> cadGetSelected();
void cadSetColor(color_t);
void cadRenderLine(line_t);
void cadRenderArc(arc_t);
void cadRender();
void cadRedraw();
cadEntity cadGetEntityArray(int);
int cadGetEntityArrayIndex();


void cadDrawLine(point_t start, point_t end);
void cadDrawLine(line_t);

void cadDrawArc(arc_t);

#endif
