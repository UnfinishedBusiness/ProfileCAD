#include <Debug.h>

using namespace std;

void debugDumpArcStructure(arc_t a)
{
  cout << KMAGENTA;
  cout << "arc_t{\n";
  cout << KGREEN;
  cout << "\tstart=point_t{" << a.start.x << ", "<< a.start.y << "},\n";
  cout << "\tend=point_t{" << a.end.x << ", "<< a.end.y << "},\n";
  cout << "\tcenter=point_t{" << a.center.x << ", "<< a.center.y << "},\n";
  cout << "\tradius=float{" << a.radius << "},\n";
  if (a.direction)
  {
    cout << "\tdirection=bool{true||ARC_CW},\n";

  }
  else
  {
    cout << "\tdirection=bool{false||ARC_CCW},\n";
  }
  cout << KMAGENTA;
  cout << "};\n";
  cout << KNORMAL;
  cout << endl;
}
void debugDumpLineStructure(line_t l)
{
  cout << KMAGENTA;
  cout << "line_t{\n";
  cout << KGREEN;
  cout << "\tstart=point_t{" << l.start.x << ", "<< l.start.y << "},\n";
  cout << "\tend=point_t{" << l.end.x << ", "<< l.end.y << "},\n";
  cout << KMAGENTA;
  cout << "};\n";
  cout << KNORMAL;
  cout << endl;
}
void debugDumpPointStructure(point_t p)
{
  cout << KMAGENTA;
  cout << "point_t{\n";
  cout << KGREEN;
  cout << "\tx=float(" << p.x << "),\n";
  cout << "\ty=float(" << p.y << "),\n";
  cout << "\tz=float(" << p.z << "),\n";
  cout << KMAGENTA;
  cout << "};\n";
  cout << KNORMAL;
  cout << endl;
}
