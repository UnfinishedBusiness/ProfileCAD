#include <Debug.h>

using namespace std;

inline const char * const BoolToString(bool b)
{
  return b ? "true" : "false";
}

void debugDumpArcStructure(arc_t a)
{
  cout << KMAGENTA;
  cout << "arc_t{\n";
  cout << KGREEN;
  cout << "\t\tstart=point_t{" << a.start.x << ", "<< a.start.y << "},\n";
  cout << "\t\tend=point_t{" << a.end.x << ", "<< a.end.y << "},\n";
  cout << "\t\tcenter=point_t{" << a.center.x << ", "<< a.center.y << "},\n";
  cout << "\t\tradius=float{" << a.radius << "},\n";
  if (a.direction)
  {
    cout << "\t\tdirection=bool{true||ARC_CW},\n";

  }
  else
  {
    cout << "\t\tdirection=bool{false||ARC_CCW},\n";
  }
  cout << KMAGENTA;
  cout << "\t};\n";
  cout << KNORMAL;
  cout << endl;
}
void debugDumpLineStructure(line_t l)
{
  cout << KMAGENTA;
  cout << "line_t{\n";
  cout << KGREEN;
  cout << "\t\tstart=point_t{" << l.start.x << ", "<< l.start.y << "},\n";
  cout << "\t\tend=point_t{" << l.end.x << ", "<< l.end.y << "},\n";
  cout << KMAGENTA;
  cout << "\t\t};\n";
  cout << KNORMAL;
  cout << endl;
}
void debugDumpEntityStructure(cadEntity e)
{
  if (e.Removed == false)
  {
    cout << KMAGENTA;
    cout << "cadEntity{\n";
    cout << KGREEN;
    cout << KGREEN << "\tSelected=" << KRED << BoolToString(e.Selected) << "\n";
    cout << KGREEN << "\tMouseOver=" << KRED << BoolToString(e.MouseOver) << "\n";
    cout << KGREEN << "\tIndex=" << KRED << e.Index << "\n";
    cout << KGREEN << "\tSelectedIndex=" << KRED << e.SelectionIndex << "\n";
    debugDumpColorStructure(e.Color);
    if (e.Type == CAD_ARC)
    {
      cout << KGREEN << "\tarc_t=";
      debugDumpArcStructure(e.Arc);
    }
    if (e.Type == CAD_LINE)
    {
      cout << KGREEN << "\tline_t=";
      debugDumpLineStructure(e.Line);
    }
    cout << KMAGENTA;
    cout << "};\n";
    cout << KNORMAL;
    cout << endl;
  }

}
void debugDumpPointStructure(point_t p)
{
  cout << KMAGENTA;
  cout << "\tpoint_t{\n";
  cout << KGREEN;
  cout << "\t\tx=float(" << p.x << "),\n";
  cout << "\t\ty=float(" << p.y << "),\n";
  cout << "\t\tz=float(" << p.z << "),\n";
  cout << KMAGENTA;
  cout << "\t};\n";
  cout << KNORMAL;
  cout << endl;
}
void debugDumpColorStructure(color_t c)
{
  cout << KMAGENTA;
  cout << "\tcolor_t{\n";
  cout << KGREEN;
  cout << "\t\tr=float(" << c.r << "),\n";
  cout << "\t\tg=float(" << c.g << "),\n";
  cout << "\t\tb=float(" << c.b << "),\n";
  cout << "\t\ta=float(" << c.a << "),\n";
  cout << KMAGENTA;
  cout << "\t};\n";
  cout << KNORMAL;
  cout << endl;
}
void debugDumpContourStructure(contour_t c)
{
  cout << KMAGENTA;
  cout << "contour_t{\n";
  cout << KGREEN;
  for (int x=0; x < c.Entitys.size(); x++)
  {
      cout << KMAGENTA;
      cout << "\t\tEntitys[" << x << "]=";
      if (c.Entitys[x].Type == CAD_LINE)
      {
        debugDumpLineStructure(c.Entitys[x].Line);
      }
      else if (c.Entitys[x].Type == CAD_ARC)
      {
        debugDumpArcStructure(c.Entitys[x].Arc);
      }
  }
  cout << KMAGENTA;
  cout << "\t};\n";
  cout << KNORMAL;
  cout << endl;
}
