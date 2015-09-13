#include <Debug.h>

using namespace std;

void debugDumpArcStructure(arc_t a)
{
  #ifdef DEBUG
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
  #endif
}
