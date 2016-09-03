#include <CAD.h>

using namespace std;

cadSelectionBox_t cadSelectionBox;
vector<cadEntity> cadLiveEntity;
vector<cadToolpath> cadToolpathsArray;
vector<cadUndoStructure> cadUndoArray;

vector<cadEntity> cadEntityArray;
bool renderToolpaths = false;
int cadEntityArrayIndex;
color_t cadColorAttribute = GREEN;
contour_t CurrentContour;
void cadInit()
{
  cadEntityArrayIndex = 0;
}
void cadUndoPushState()
{
  if (cadUndoArray.size() > 9) //Only keep ten elements
  {
    cadUndoArray.erase(cadUndoArray.begin());
  }
  cadUndoArray.push_back(cadUndoStructure{cadEntityArray, cadEntityArrayIndex});
  //V cout << KRED << "(cadUndoPushState) Pushed Current state!" << KNORMAL << endl;
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
  //cliScreenUnSelectAll();
}
void cadClearEntityStack()
{
  cadEntityArrayIndex = 0;
  cadEntityArray.clear();
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
    cadUndoPushState();
  }
  cadEntityArrayIndex++;
  //glutPostRedisplay();
}
void cadEdit(int i, cadEntity e)
{
  cout << "(cadEdit) Undo true" << endl;
  cadEdit(i, e, true);
}
void cadEdit(int i, cadEntity e, bool undo)
{
  if (cadEntityArrayIndex > i)
  {
    cadEntityArray[i] = e;
  }
  if (undo == true)//Dont waste undo buffer!
  {
    cadUndoPushState();
  }

  //glutPostRedisplay();
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
  cadUndoPushState();
  cadEntity e;
  for (int i = 0; i < cadEntityArrayIndex; i++)
  {
      if (cadEntityArray[i].Selected && !cadEntityArray[i].Removed) //Make sure were selected and not removed
      {
        //cadEntityArray[i].Removed = true;
        e.Removed = true;
        cadEdit(i, e, false);
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
        e.push_back(cadEntityArray[i]);
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
        //printf("Found! SelectionIndex = %d\n not %d!\n", e[x].SelectionIndex, next);
        o.push_back(e[x]);
        next++;
      }
    }
  }
  if (e.size() > o.size()) //We could order them properly, give an Unordered list!
  {
    return e;
  }
  return o;
}
int cadCountSelection()
{
  int r = 0;
  for (int i = 0; i < cadEntityArray.size(); i++)
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
  cadSelectionBox.a.radius = 0.008 / sceneGetScale();
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
void cadShowToolpaths()
{
  renderToolpaths = true;
}
void cadHideToolpaths()
{
  renderToolpaths = false;
}
void cadAppendToolpath(cadToolpath t)
{
  cadToolpathsArray.push_back(t);
}
std::vector<cadToolpath> cadGetToolpaths()
{
  return cadToolpathsArray;
}
void cadRender()
{
  if (renderToolpaths)
  {
    //cout << "Rendering toolpaths!" << endl;
    sceneColor(TOOLPATH_COLOR);
    for (int x = 0; x < cadToolpathsArray.size(); x++)
    {
      for (int i = 0; i < cadToolpathsArray[x].Path.Entitys.size(); i++)
      {
        if (cadToolpathsArray[x].Path.Entitys[i].Type == CAD_LINE)
        {
          cadRenderLine(cadToolpathsArray[x].Path.Entitys[i].Line, 2);
        }
        if (cadToolpathsArray[x].Path.Entitys[i].Type == CAD_ARC)
        {
          cadRenderArc(cadToolpathsArray[x].Path.Entitys[i].Arc, 2);
        }
      }
    }
  }
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
      //sceneColor(cadLiveEntity[x].Color);
      sceneColor(YELLOW);
      if (cadLiveEntity[x].Type == CAD_LINE)
      {
        cadRenderLine(cadLiveEntity[x].Line, cadLiveEntity[x].LineWidth);
      }
      if (cadLiveEntity[x].Type == CAD_ARC)
      {
        cadRenderArc(cadLiveEntity[x].Arc, cadLiveEntity[x].LineWidth);
      }
      if (cadLiveEntity[x].Type == CAD_NOTE)
      {
        cadRenderNote(cadLiveEntity[x].Note);
      }
      if (cadLiveEntity[x].Type == CAD_DIMENSION)
      {
        cadRenderDimension(cadLiveEntity[x].Dimension);
      }
    }
  }
  for (int i = 0; i < cadEntityArrayIndex; i++)
  {
      if (cadEntityArray[i].Type == CAD_DIMENSION && !cadEntityArray[i].Removed) //Where a line and its not been removed
      {
        if (cadEntityArray[i].Selected)
        {
          sceneColor(SELECTED_COLOR);
        }
        else if (cadEntityArray[i].MouseOver)
        {
          sceneColor(MOUSEOVER_COLOR);
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
        if (cadEntityArray[i].Selected)
        {
          sceneColor(SELECTED_COLOR);
        }
        else if (cadEntityArray[i].MouseOver)
        {
          sceneColor(MOUSEOVER_COLOR);
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
        if (cadEntityArray[i].Selected)
        {
          sceneColor(SELECTED_COLOR);
        }
        else if (cadEntityArray[i].MouseOver)
        {
          sceneColor(MOUSEOVER_COLOR);
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
        if (cadEntityArray[i].Selected)
        {
          sceneColor(SELECTED_COLOR);
        }
        else if (cadEntityArray[i].MouseOver)
        {
          sceneColor(MOUSEOVER_COLOR);
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
    //font = glut_BITMAP_HELVETICA_12;
  }
  else
  {
    return;
  }
  glRasterPos3f(n.pos.x, n.pos.y, n.pos.z);
  for (int i = 0; i < strlen(n.text); i++)
  {
    //glutBitmapCharacter (font, n.text[i]);
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
void cadRenderLine(line_t l, float w)
{
  glLineWidth(w);
  glBegin(GL_LINE_STRIP);
  glVertex3f((GLfloat) l.start.x, l.start.y, l.start.z);
  glVertex3f((GLfloat) l.end.x, l.end.y, l.end.z);
  glEnd();
}
void cadRenderLine(line_t l)
{
  cadRenderLine(l, 1);
}
void cadRenderArc(arc_t a)
{
  cadRenderArc(a, 1);
}
void cadRenderArc(arc_t a, float w)
{
  glLineWidth(w);
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
  PostRedisplay();
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
  return point_t{ (float)worldX, (float)worldY };
  //worldX = (worldX);
  //worldY = (worldY);
  //point_t panOffset = sceneGetPanOffset();
  //return point_t{(float)worldX - (panOffset.x), (float)worldY - (panOffset.y), 0};
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
void cadReverseCurrentContour()
{
  for (int x = 0; x < CurrentContour.Entitys.size(); x++)
  {
    //V debugDumpEntityStructure(CurrentContour.Entitys[x]);
    if (CurrentContour.Entitys[x].Type == CAD_LINE) CurrentContour.Entitys[x].Line = geoFlipLine(CurrentContour.Entitys[x].Line);
    if (CurrentContour.Entitys[x].Type == CAD_ARC) CurrentContour.Entitys[x].Arc = geoFlipArc(CurrentContour.Entitys[x].Arc);
  }
  //mouseLiveShow("CurrentContour");
}
contour_t cadGetCurrentContour()
{
  return CurrentContour;
}
color_t cadGetCurrentColor()
{
  return cadColorAttribute;
}
void cadSelectChain()
{
  if (cadCountSelection() > 0)
  {
    //cout << "Select chain!" << endl;
    /*
    Loop through ENTITIES to generate a chain
    If this entity is connected to the last entity in the chain, push it
    to the chain stack and search entitys again
    */
    vector<int> used;
    vector<cadEntity> chain;
    chain.push_back(cadGetSelected()[0]);
    used.push_back(cadGetSelected()[0].Index);
    bool Done = false;
    bool Found = false;
    while(!Done)
    {
      for (int i = 0; i < cadEntityArrayIndex; i++)
      {
        if (cadEntityArray[i].Type == CAD_LINE)
        {
          if (chain.back().Type == CAD_LINE)
          {
            if ((chain.back().Line.end == cadEntityArray[i].Line.end ||
                chain.back().Line.end == cadEntityArray[i].Line.start ||
                chain.back().Line.start == cadEntityArray[i].Line.end ||
                chain.back().Line.start == cadEntityArray[i].Line.start) &&
                std::find(used.begin(), used.end(), cadEntityArray[i].Index) == used.end())
            {
              Found = true;
              cadEntityArray[i].Selected = true;
              cadEntityArray[i].SelectionIndex = cadCountSelection() + 1;
              //cadEdit(i, cadEntityArray[i], false);
              chain.push_back(cadEntityArray[i]);
              used.push_back(cadEntityArray[i].Index);
              //cout << "Found!" << endl;
              break;
            }
          }
          else if (chain.back().Type == CAD_ARC)
          {
            if ((chain.back().Arc.end == cadEntityArray[i].Line.end ||
                chain.back().Arc.end == cadEntityArray[i].Line.start ||
                chain.back().Arc.start == cadEntityArray[i].Line.end ||
                chain.back().Arc.start == cadEntityArray[i].Line.start) &&
                std::find(used.begin(), used.end(), cadEntityArray[i].Index) == used.end())
            {
              Found = true;
              cadEntityArray[i].Selected = true;
              cadEntityArray[i].SelectionIndex = cadCountSelection() + 1;
              //cadEdit(i, cadEntityArray[i], false);
              chain.push_back(cadEntityArray[i]);
              used.push_back(cadEntityArray[i].Index);
              //cout << "Found!" << endl;
              break;
            }
          }
        }
        else if (cadEntityArray[i].Type == CAD_ARC)
        {
          if (chain.back().Type == CAD_LINE)
          {
            if ((chain.back().Line.end == cadEntityArray[i].Arc.end ||
                chain.back().Line.end == cadEntityArray[i].Arc.start ||
                chain.back().Line.start == cadEntityArray[i].Arc.end ||
                chain.back().Line.start == cadEntityArray[i].Arc.start) &&
                std::find(used.begin(), used.end(), cadEntityArray[i].Index) == used.end())
            {
              Found = true;
              cadEntityArray[i].Selected = true;
              cadEntityArray[i].SelectionIndex = cadCountSelection() + 1;
              //cadEdit(i, cadEntityArray[i], false);
              chain.push_back(cadEntityArray[i]);
              used.push_back(cadEntityArray[i].Index);
              //cout << "Found!" << endl;
              break;
            }
          }
          else if (chain.back().Type == CAD_ARC)
          {
            if ((chain.back().Arc.end == cadEntityArray[i].Arc.end ||
                chain.back().Arc.end == cadEntityArray[i].Arc.start ||
                chain.back().Arc.start == cadEntityArray[i].Arc.end ||
                chain.back().Arc.start == cadEntityArray[i].Arc.start) &&
                std::find(used.begin(), used.end(), cadEntityArray[i].Index) == used.end())
            {
              Found = true;
              cadEntityArray[i].Selected = true;
              cadEntityArray[i].SelectionIndex = cadCountSelection() + 1;
              //cadEdit(i, cadEntityArray[i], false);
              chain.push_back(cadEntityArray[i]);
              used.push_back(cadEntityArray[i].Index);
              //cout << "Found!" << endl;
              break;
            }
          }
        }
      }
      if (Found)
      {
        Found = false;
      }
      else
      {
        Done = true;
      }
    }
    for (int x = 0; x < chain.size() - 1; x++)
    {
      if (chain[x].Type == CAD_LINE && chain[x + 1].Type == CAD_LINE)
      {
        if (chain[x].Line.start == chain[x + 1].Line.end) //Flip both
        {
          chain[x].Line = geoFlipLine(chain[x].Line);
          chain[x+1].Line = geoFlipLine(chain[x+1].Line);
        }
        else if (chain[x].Line.start == chain[x + 1].Line.start) //Flip this
        {
          chain[x].Line = geoFlipLine(chain[x].Line);
        }
        else if (chain[x].Line.end == chain[x + 1].Line.end) //Flip next
        {
          chain[x + 1].Line = geoFlipLine(chain[x + 1].Line);
        }
      }
      else if (chain[x].Type == CAD_LINE && chain[x + 1].Type == CAD_ARC)
      {
        if (chain[x].Line.start == chain[x + 1].Arc.end) //Flip both
        {
          chain[x].Line = geoFlipLine(chain[x].Line);
          chain[x+1].Arc = geoFlipArc(chain[x+1].Arc);
        }
        else if (chain[x].Line.start == chain[x + 1].Arc.start) //Flip this
        {
          chain[x].Line = geoFlipLine(chain[x].Line);
        }
        else if (chain[x].Line.end == chain[x + 1].Arc.end) //Flip next
        {
          chain[x + 1].Arc = geoFlipArc(chain[x + 1].Arc);
        }
      }
      else if (chain[x].Type == CAD_ARC && chain[x + 1].Type == CAD_ARC)
      {
        if (chain[x].Arc.start == chain[x + 1].Arc.end) //Flip both
        {
          chain[x].Arc = geoFlipArc(chain[x].Arc);
          chain[x+1].Arc = geoFlipArc(chain[x+1].Arc);
        }
        else if (chain[x].Arc.start == chain[x + 1].Arc.start) //Flip this
        {
          chain[x].Arc = geoFlipArc(chain[x].Arc);
        }
        else if (chain[x].Arc.end == chain[x + 1].Arc.end) //Flip next
        {
          chain[x + 1].Arc = geoFlipArc(chain[x + 1].Arc);
        }
      }
      else if (chain[x].Type == CAD_ARC && chain[x + 1].Type == CAD_LINE)
      {
        if (chain[x].Arc.start == chain[x + 1].Line.end) //Flip both
        {
          chain[x].Arc = geoFlipArc(chain[x].Arc);
          chain[x+1].Line = geoFlipLine(chain[x+1].Line);
        }
        else if (chain[x].Arc.start == chain[x + 1].Line.start) //Flip this
        {
          chain[x].Arc = geoFlipArc(chain[x].Arc);
        }
        else if (chain[x].Arc.end == chain[x + 1].Line.end) //Flip next
        {
          chain[x + 1].Line = geoFlipLine(chain[x + 1].Line);
        }
      }
    }
    CurrentContour.Entitys.clear();
    CurrentContour.Entitys = chain;
    CurrentContour.isClosed = false;
    if (CurrentContour.Entitys.front().Type == CAD_LINE && CurrentContour.Entitys.back().Type == CAD_LINE)
    {
      if (CurrentContour.Entitys.front().Line.start == CurrentContour.Entitys.back().Line.end)
      {
        //cout << "Chain line=>line closed" << endl;
        CurrentContour.isClosed = true;
      }
    }
    else if (CurrentContour.Entitys.front().Type == CAD_LINE && CurrentContour.Entitys.back().Type == CAD_ARC)
    {
      if (CurrentContour.Entitys.front().Line.start == CurrentContour.Entitys.back().Arc.end)
      {
        //cout << "Chain line=>arc closed" << endl;
        CurrentContour.isClosed = true;
      }
    }
    else if (CurrentContour.Entitys.front().Type == CAD_ARC && CurrentContour.Entitys.back().Type == CAD_ARC)
    {
      if (CurrentContour.Entitys.front().Arc.start == CurrentContour.Entitys.back().Arc.end)
      {
        //cout << "Chain arc=>arc closed" << endl;
        CurrentContour.isClosed = true;
      }
    }
    else if (CurrentContour.Entitys.front().Type == CAD_ARC && CurrentContour.Entitys.back().Type == CAD_LINE)
    {
      if (CurrentContour.Entitys.front().Arc.start == CurrentContour.Entitys.back().Line.end)
      {
        //cout << "Chain arc=>line closed" << endl;
        CurrentContour.isClosed = true;
      }
    }
    //mouseLiveShow("CurrentContour");
  }
}
