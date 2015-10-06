#ifndef CAD_
#define CAD_
#include <application.h>
#include <Scene.h>



struct cadSelectionBox_t{
  arc_t a;
  bool visable;
};

struct cadUndoStructure{
  std::vector<cadEntity> e;
  int Index;
};

void cadInit();
void cadUndo();
void cadUndoPushState();
void cadAppend(cadEntity);
void cadAppend(cadEntity, bool);
void cadEdit(int, cadEntity);
void cadEdit(int, cadEntity, bool);
void cadRemoveSelected();
std::vector<cadEntity> cadGetSelected();
std::vector<cadEntity>  cadGetMouseOver();
int cadCountSelection();
int cadCountEntitys();
void cadSetColor(color_t);

void cadSelectedToContour();
void cadSelectChain();

void cadRenderLine(line_t);
void cadRenderLine(line_t, float);
void cadRenderArc(arc_t);
void cadRenderArc(arc_t, float);
void cadRenderNote(note_t);
void cadRenderDimension(dimension_t);

void cadRender();
void cadRedraw();
cadEntity cadGetEntityArray(int);
int cadGetEntityArrayIndex();
point_t cadScreenCordToCadCord(int x, int y);
point_t cadCadCordToScreenCord(point_t);

void cadShowLiveEntity(std::vector<cadEntity>);
void cadHideLiveEntity();

cadSelectionBox_t cadGetSelectionBox();
void cadShowSelectionBox(point_t);
void cadHideSelectionBox();

void cadDrawLine(point_t start, point_t end);
void cadDrawLine(line_t);

void cadDrawArc(arc_t);
void cadDrawDimension(dimension_t);

contour_t cadGetCurrentContour();
color_t cadGetCurrentColor();
void cadReverseCurrentContour();


#endif
