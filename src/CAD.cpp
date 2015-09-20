#include <CAD.h>

using namespace std;

#define CAD_ARC 0x00
#define CAD_LINE 0x01

cadSelectionBox_t cadSelectionBox;

vector<cadUndoStructure> cadUndoArray;

vector<cadEntity> cadEntityArray;
int cadEntityArrayIndex;
color_t cadColorAttribute = GREEN;

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
  e.Selected = false;
  e.Removed = false;
  e.Index = cadEntityArrayIndex;
  cadEntityArray.push_back(cadEntity());
  cadEntityArray[cadEntityArrayIndex] = e;
  if (cadUndoArray.size() > 9) //Only keep ten elements
  {
    cadUndoArray.erase(cadUndoArray.begin());
  }
  cadUndoArray.push_back(cadUndoStructure{cadEntityArray, cadEntityArrayIndex});
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
    //D cout << KRED << "(cadEdit)Actual Edit!" << KNORMAL << endl;
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
  //D printf("(cadDrawLine) Drawing line at (%.6f, %.6f, %.6f) ===== (%.6f, %.6f, %.6f)\n", start.x, start.y, start.z, end.x, end.y, end.z);
  cadAppend(cadEntity{CAD_LINE, cadColorAttribute, line_t{ start, end }, arc_t{} });
}
void cadDrawArc(arc_t a)
{
  cadAppend(cadEntity{CAD_ARC, cadColorAttribute, line_t{}, a });
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
void cadRender()
{
  if (cadSelectionBox.visable == true)
  {
    //printf("Rendering selection box!\n");
    sceneColor(DARKGREY);
    cadRenderArc(cadSelectionBox.a);
  }
  for (int i = 0; i < cadEntityArrayIndex; i++)
  {
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
        //D printf("(cadRender) Found arc!\n");
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
void cadRenderLine(line_t l)
{
  glLineWidth(1);
  glBegin(GL_LINE_LOOP);
  glVertex3f((GLfloat) l.start.x, l.start.y, l.start.z);
  glVertex3f((GLfloat) l.end.x, l.end.y, l.end.z);
  glEnd();

  //glBegin(GL_POINTS);
  cadEntityArray[l.parentIndex].Vector.clear();
  cadEntityArray[l.parentIndex].Vector = geoGetPointsOfLine(l);
  /*for (int x = 0; x < cadEntityArray[l.parentIndex].Vector.size(); x++)
  {
      glVertex3f((GLfloat) cadEntityArray[l.parentIndex].Vector[x].x, (GLfloat) cadEntityArray[l.parentIndex].Vector[x].y, 0);
  }
  glEnd();*/
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
  cadEntityArray[a.parentIndex].Vector.clear();
  float inc_angle = 1; //Degrees
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
    if (cadEntityArray[a.parentIndex].Vector.size() != steps) //Populate vector if it hasnt already been
    {
      cadEntityArray[a.parentIndex].Vector.push_back(point_t());
      cadEntityArray[a.parentIndex].Vector[x] = l.end;
    }
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
