#include <CAD.h>

using namespace std;

cadSelectionBox_t cadSelectionBox;
vector<cadEntity> cadLiveEntity;

vector<cadUndoStructure> cadUndoArray;

vector<cadEntity> cadEntityArray;
int cadEntityArrayIndex;
color_t cadColorAttribute = GREEN;
contour_t CurrentContour;
void cadInit()
{
  cadEntityArrayIndex = 0;
}

void cadUndo()
{
  if (cadUndoArray.size() > 1)
  {
    cadUndoArray.pop_back();
    cadEntityArray.clear();
    cadEntityArray = cadUndoArray.back().e;
    cadEntityArrayIndex = cadEntityArray.size();
    cadRedraw();
  }
  cliScreenUnSelectAll();
}
void cadAppend(cadEntity e)
{
  cadAppend(e, true);
}
void cadAppend(cadEntity e, bool undo)
{
  e.Selected = false;
  e.Removed = false;
  e.Index = cadEntityArrayIndex;
  cadEntityArray.push_back(cadEntity());
  cadEntityArray[cadEntityArrayIndex] = e;
  if (undo == true)
  {
    if (cadUndoArray.size() > 9) //Only keep ten elements
    {
      cadUndoArray.erase(cadUndoArray.begin());
    }
    cadUndoArray.push_back(cadUndoStructure{cadEntityArray, cadEntityArrayIndex});
  }
  cadEntityArrayIndex++;
  glutPostRedisplay();
}
void cadEdit(int i, cadEntity e)
{
  /*
  int SelectionIndex;
  */
  bool OpEdit = true;
  if (cadEntityArrayIndex > i)
  {
    if (e.Removed != cadEntityArray[i].Removed ||
        e.Color != cadEntityArray[i].Color ||
        e.Line.start != cadEntityArray[i].Line.start ||
        e.Line.end != cadEntityArray[i].Line.end ||
        e.Arc.start != cadEntityArray[i].Arc.start ||
        e.Arc.end != cadEntityArray[i].Arc.end ||
        e.Arc.radius != cadEntityArray[i].Arc.radius ||
        e.Arc.direction != cadEntityArray[i].Arc.direction)
    {
      OpEdit = false;
    }
    cadEntityArray[i] = e;
  }
  if (OpEdit == false)//Dont waste undo buffer!
  {
    //V cout << KRED << "(cadEdit)Actual Edit!" << KNORMAL << endl;
    if (cadUndoArray.size() > 9) //Only keep ten elements
    {
      cadUndoArray.erase(cadUndoArray.begin());
    }
    cadUndoArray.push_back(cadUndoStructure{cadEntityArray, cadEntityArrayIndex});
  }

  glutPostRedisplay();
}
void cadDrawLine(line_t l)
{
  cadDrawLine(l.start, l.end);
}
void cadDrawLine(point_t start, point_t end)
{
  //V printf("(cadDrawLine) Drawing line at (%.6f, %.6f, %.6f) ===== (%.6f, %.6f, %.6f)\n", start.x, start.y, start.z, end.x, end.y, end.z);
  cadAppend(cadEntity{CAD_LINE, cadColorAttribute, line_t{ start, end }, arc_t{} });
}
void cadDrawArc(arc_t a)
{
  cadAppend(cadEntity{CAD_ARC, cadColorAttribute, line_t{}, a });
}
void cadDrawDimension(dimension_t d)
{
  cadEntity e;
  e.Type = CAD_DIMENSION;
  e.Color = cadColorAttribute;
  e.Dimension = d;
  cadAppend(e);
}
void cadSetColor(color_t c)
{
  cadColorAttribute = c;
}
cadEntity cadGetEntityArray(int x)
{
  if (cadEntityArrayIndex > x)
  {
    return cadEntityArray[x];
  }
}
int cadGetEntityArrayIndex()
{
  return cadEntityArrayIndex;
}
void cadRemoveSelected()
{
  cadEntity e;
  for (int i = 0; i < cadEntityArrayIndex; i++)
  {
      if (cadEntityArray[i].Selected && !cadEntityArray[i].Removed) //Make sure were selected and not removed
      {
        //cadEntityArray[i].Removed = true;
        e.Removed = true;
        cadEdit(i, e);
      }
  }
}
std::vector<cadEntity> cadGetSelected()
{
  std::vector<cadEntity> e; //Unordered
  std::vector<cadEntity> o; //Ordered by selection
  for (int i = 0; i < cadEntityArrayIndex; i++)
  {
      if (cadEntityArray[i].Selected && !cadEntityArray[i].Removed) //Make sure were selected and not removed
      {
        //cout << "SelectionIndex = " << cadEntityArray[i].SelectionIndex << endl;
        e.push_back(cadEntity());
        e[e.size()-1] = cadEntityArray[i];
      }
  }
  int next = 1;
  for (int a = 0; a < e.size(); a++)
  {
    for (int x = 0; x < e.size(); x++)
    {
      if (e[x].SelectionIndex == next)
      {
        //cout << "Ordered = " << e[x].SelectionIndex << endl;
        o.push_back(cadEntity());
        o[next-1] = e[x];
        next++;
      }
    }
  }

  return o;
}
int cadCountSelection()
{
  int r = 0;
  for (int i = 0; i < cadEntityArrayIndex; i++)
  {
      if (cadEntityArray[i].Selected && !cadEntityArray[i].Removed) //Make sure were selected and not removed
      {
        r++;
      }
  }
  return r;
}
int cadCountEntitys()
{
  int r = 0;
  for (int i = 0; i < cadEntityArrayIndex; i++)
  {
      if (!cadEntityArray[i].Removed) //Make sure were selected and not removed
      {
        r++;
      }
  }
  return r;
}
std::vector<cadEntity> cadGetMouseOver()
{
  std::vector<cadEntity> e; //Unordered
  for (int i = 0; i < cadEntityArrayIndex; i++)
  {
      if (cadEntityArray[i].MouseOver && !cadEntityArray[i].Removed) //Make sure were selected and not removed
      {
        e.push_back(cadEntityArray[i]);
      }
  }
  return e;
}
cadSelectionBox_t cadGetSelectionBox()
{
  return cadSelectionBox;
}
void cadShowSelectionBox(point_t p)
{
  //line_t geoGetPerpendicularLine(line_t, point_t, float);
  //line_t geoGetParallelLine(line_t, point_t, float);
  cadSelectionBox.a.radius = 0.005 / sceneGetScale();
  cadSelectionBox.a.center = p;
  cadSelectionBox.a.start = p;
  cadSelectionBox.a.start.x += cadSelectionBox.a.radius;
  cadSelectionBox.a.end = p;
  cadSelectionBox.a.end.x += cadSelectionBox.a.radius;
  cadSelectionBox.visable = true;
}
void cadHideSelectionBox()
{
  cadSelectionBox.visable = false;
}
void cadSelectedToContour()
{
  CurrentContour = geoGetContour(cadGetSelected());
  //debugDumpContourStructure(CurrentContour);

  vector<cadEntity> l;
  cadEntity e;
  for (int x = 0; x < CurrentContour.Entitys.size(); x++)
  {
    if (CurrentContour.Entitys[x].Type == CAD_LINE)
    {
      e.Type = CAD_LINE;
      //e.Line = geoExtendLineStartpoint(geoRotateLine(CurrentContour.Entitys[x].Line, geoGetLineMidpoint(CurrentContour.Entitys[x].Line), 135), 0.050);
      e.Line = geoExtendLineAngle(geoGetLineMidpoint(CurrentContour.Entitys[x].Line), geoGetLineAngle(CurrentContour.Entitys[x].Line) + geoDegreesToRadians(45), 0.1);
      l.push_back(e);
      e.Line = geoExtendLineAngle(geoGetLineMidpoint(CurrentContour.Entitys[x].Line), geoGetLineAngle(CurrentContour.Entitys[x].Line) + geoDegreesToRadians(-45), 0.1);
      l.push_back(e);
    }
    if (CurrentContour.Entitys[x].Type == CAD_ARC)
    {
      cout << "Found Arc!" << endl;
      e = CurrentContour.Entitys[x];
      e.Arc = geoGetCircle(e.Arc.center, 0.1);
      l.push_back(e);
    }

  }
  cadShowLiveEntity(l);
}
void cadShowLiveEntity(vector<cadEntity> e)
{
  cadLiveEntity.clear();
  for (int x = 0; x < e.size(); x++)
  {
    e[x].Removed = false;
    cadLiveEntity.push_back(e[x]);
  }
}
void cadHideLiveEntity()
{
  cadLiveEntity.clear();
}

void cadRender()
{
  if (cadSelectionBox.visable == true)
  {
    //printf("Rendering selection box!\n");
    sceneColor(DARKGREY);
    cadRenderArc(cadSelectionBox.a);
  }
  for (int x = 0; x < cadLiveEntity.size(); x++)
  {
    if (cadLiveEntity[x].Removed == false)
    {
      //printf("Rendering live entity!\n");
      if (cadLiveEntity[x].Type == CAD_LINE)
      {
        sceneColor(DARKGREY);
        cadRenderLine(cadLiveEntity[x].Line);
      }
      if (cadLiveEntity[x].Type == CAD_ARC)
      {
        sceneColor(DARKGREY);
        cadRenderArc(cadLiveEntity[x].Arc);
      }
      if (cadLiveEntity[x].Type == CAD_NOTE)
      {
        sceneColor(DARKGREY);
        cadRenderNote(cadLiveEntity[x].Note);
      }
      if (cadLiveEntity[x].Type == CAD_DIMENSION)
      {
        sceneColor(DARKGREY);
        cadRenderDimension(cadLiveEntity[x].Dimension);
      }
    }
  }
  for (int i = 0; i < cadEntityArrayIndex; i++)
  {
      if (cadEntityArray[i].Type == CAD_DIMENSION && !cadEntityArray[i].Removed) //Where a line and its not been removed
      {
        if (cadEntityArray[i].Selected || cadEntityArray[i].MouseOver)
        {
          sceneColor(WHITE);
        }
        else
        {
          sceneColor(cadEntityArray[i].Color);
        }
        cadEntityArray[i].Dimension.parentIndex = i;
        cadRenderDimension(cadEntityArray[i].Dimension);
        //cout << "Found dimension!" << endl;
      }
      if (cadEntityArray[i].Type == CAD_NOTE && !cadEntityArray[i].Removed) //Where a line and its not been removed
      {
        if (cadEntityArray[i].Selected || cadEntityArray[i].MouseOver)
        {
          sceneColor(WHITE);
        }
        else
        {
          sceneColor(cadEntityArray[i].Color);
        }
        cadEntityArray[i].Note.parentIndex = i;
        cadRenderNote(cadEntityArray[i].Note);
      }
      if (cadEntityArray[i].Type == CAD_LINE && !cadEntityArray[i].Removed) //Where a line and its not been removed
      {
        if (cadEntityArray[i].Selected || cadEntityArray[i].MouseOver)
        {
          sceneColor(WHITE);
        }
        else
        {
          sceneColor(cadEntityArray[i].Color);
        }
        cadEntityArray[i].Line.parentIndex = i;
        cadRenderLine(cadEntityArray[i].Line);
      }
      if (cadEntityArray[i].Type == CAD_ARC && !cadEntityArray[i].Removed && cadEntityArray[i].Arc.radius > 0)
      {
        //V printf("(cadRender) Found arc!\n");
        if (cadEntityArray[i].Selected || cadEntityArray[i].MouseOver)
        {
          sceneColor(WHITE);
        }
        else
        {
          sceneColor(cadEntityArray[i].Color);
        }
        cadEntityArray[i].Arc.parentIndex = i;
        cadRenderArc(cadEntityArray[i].Arc);
      }
  }
}
void cadRenderNote(note_t n)
{
  //cout << "Render note!" << endl;
  void *font;
  if (n.size == 12)
  {
    font = GLUT_BITMAP_HELVETICA_12;
  }
  else
  {
    return;
  }
  glRasterPos3f(n.pos.x, n.pos.y, n.pos.z);
  for (int i = 0; i < strlen(n.text); i++)
  {
    glutBitmapCharacter (font, n.text[i]);
  }
}
void cadRenderDimension(dimension_t d)
{
  note_t n;
  stringstream X;
  stringstream Y;
  stringstream Z;
  //cout << d.Type << endl;
  if (d.Type == DIMENSION_POINT)
  {
    n.pos = d.Point.text_pos;
    n.size = 12;
    X << d.Point.snap_pos.x;
    Y << d.Point.snap_pos.y;
    Z << d.Point.snap_pos.z;
    string tmp;
    if (d.Point.snap_pos.z != 0)
    {
      tmp = "X: " + X.str() + " Y: " + Y.str() + " Z: " + Z.str();
    }
    else
    {
      tmp = "X: " + X.str() + " Y: " + Y.str();
    }
    strcpy(d.Point.text, tmp.c_str());

    //n.text = d.Point.text;
    strcpy(n.text, d.Point.text);
    
    cadRenderNote(n);
    line_t leader_body = geoExtendLineEndpoint(line_t{d.Point.text_pos, d.Point.snap_pos}, 0.050);
    leader_body = geoExtendLineEndpoint(line_t{leader_body.end, leader_body.start}, 0.050);
    cadRenderLine(leader_body);
    cadRenderLine(geoExtendLineStartpoint(geoRotateLine(leader_body, leader_body.start, 135), 0.050));
    cadRenderLine(geoExtendLineStartpoint(geoRotateLine(leader_body, leader_body.start, -135), 0.050));
  }
}
void cadRenderLine(line_t l)
{
  glLineWidth(1);
  glBegin(GL_LINE_LOOP);
  glVertex3f((GLfloat) l.start.x, l.start.y, l.start.z);
  glVertex3f((GLfloat) l.end.x, l.end.y, l.end.z);
  glEnd();
}
void cadRenderArc(arc_t a)
{
  float includedAngle = geoGetIncludedAngle(a);
  line_t l = line_t{ point_t{a.center.x, a.center.y,} , point_t{a.start.x, a.start.y} };
  glBegin(GL_LINE_STRIP);
  int steps;
  if (a.start == a.end)
  {
    //Were a circle
    steps = 361; //Go the full 360 to close the gap
  }
  else
  {
    glVertex3f(a.start.x, a.start.y, a.start.z);
    steps = geoGetIncludedAngle(a);
  }
  float inc_angle = 1; //Degrees
  steps++; //Shifts plus one
  for (int x=0; x < steps; x++)
  {
    if (a.direction == ARC_CW)
    {
      l = geoRotateLine(l, a.center, -inc_angle);
    }
    else
    {
      l = geoRotateLine(l, a.center, inc_angle);
    }
    glVertex3f(l.end.x, l.end.y, 0);
  }
  if (a.start != a.end)
  {
    glVertex3f(a.end.x, a.end.y, a.end.z);
  }

  glEnd();
}
void cadRedraw()
{
  glutPostRedisplay();
}
point_t cadScreenCordToCadCord(int x, int y)
{
  GLint viewport[4]; //var to hold the viewport info
  GLdouble modelview[16]; //var to hold the modelview info
  GLdouble projection[16]; //var to hold the projection matrix info
  GLfloat winX, winY, winZ; //variables to hold screen x,y,z coordinates
  GLdouble worldX, worldY, worldZ; //variables to hold world x,y,z coordinates
  glGetDoublev( GL_MODELVIEW_MATRIX, modelview ); //get the modelview info
  glGetDoublev( GL_PROJECTION_MATRIX, projection ); //get the projection matrix info
  glGetIntegerv( GL_VIEWPORT, viewport ); //get the viewport info
  winX = (float)x;
  winY = (float)viewport[3] - (float)y;
  winZ = 0;
  //get the world coordinates from the screen coordinates
  gluUnProject( winX, winY, winZ, modelview, projection, viewport, &worldX, &worldY, &worldZ);
  //printf("tx: %.6f, ty: %.6f\n", sceneGetPanOffset().x, sceneGetPanOffset().y);
  worldX = (worldX/sceneGetScale());
  worldY = (worldY/sceneGetScale());
  point_t panOffset = sceneGetPanOffset();
  return point_t{(float)worldX - (panOffset.x), (float)worldY - (panOffset.y), 0};
}
point_t cadCadCordToScreenCord(point_t s)
{
  GLint viewport[4];
  GLdouble modelview[16];
  GLdouble viewVector[3];
  GLdouble projection[16];
  GLdouble winX, winY, winZ;//2D point
  GLdouble posX, posY, posZ;//3D point
  posX=s.x;
  posY=s.y;
  posZ=s.z;
  //get the matrices
  glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
  viewVector[0]=modelview[8];
  viewVector[1]=modelview[9];
  viewVector[2]=modelview[10];
  glGetDoublev( GL_PROJECTION_MATRIX, projection );
  glGetIntegerv( GL_VIEWPORT, viewport );
  gluProject(posX,posY,posZ,modelview,projection,viewport,&winX,&winY,&winZ);
  return point_t{(float)winX, (float)winY, (float)winZ,};
}
contour_t cadGetCurrentContour()
{
  return CurrentContour;
}
color_t cadGetCurrentColor()
{
  return cadColorAttribute;
}
