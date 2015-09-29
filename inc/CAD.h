#ifndef CAD_
#define CAD_
#include <application.h>
#include <Scene.h>

#define CAD_ARC 0x00
#define CAD_LINE 0x01

struct cadSelectionBox_t{
  arc_t a;
  bool visable;
};

struct cadEntity{
  int Type; //Line or Arc
  color_t Color;
  line_t Line;
  arc_t Arc;

  //Operational data
  bool Selected;
  bool MouseOver;
  point_t SelectedAt;
  bool Removed;
  int Index;
  int SelectionIndex;
};

struct cadUndoStructure{
  std::vector<cadEntity> e;
  int Index;
};

void cadInit();
void cadUndo();
void cadAppend(cadEntity);
void cadEdit(int, cadEntity);
void cadRemoveSelected();
std::vector<cadEntity> cadGetSelected();
std::vector<cadEntity>  cadGetMouseOver();
int cadCountSelection();
void cadSetColor(color_t);
void cadRenderLine(line_t);
void cadRenderArc(arc_t);
void cadRender();
void cadRedraw();
cadEntity cadGetEntityArray(int);
int cadGetEntityArrayIndex();
point_t cadScreenCordToCadCord(int x, int y);
point_t cadCadCordToScreenCord(point_t);

void cadShowLiveEntity(cadEntity);
void cadHideLiveEntity();

cadSelectionBox_t cadGetSelectionBox();
void cadShowSelectionBox(point_t);
void cadHideSelectionBox();

void cadDrawLine(point_t start, point_t end);
void cadDrawLine(line_t);

void cadDrawArc(arc_t);



#endif
