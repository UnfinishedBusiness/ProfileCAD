#include <Geometry.h>

using namespace std;

vector<point_t> geoGetPointsOfLine(point_t start, point_t end)
{
  //D printf("(geoGetPointsOfLine) Start(%.6f, %.6f) End(%.6f, %.6f)\n", start.x, start.y, end.x, end.y);
  vector<point_t> p;
	float x = end.x - start.x;
	float y = end.y - start.y;

	float length = sqrtf( x*x + y*y );
	float addx = x / length;
  float addy = y / length;
  //D printf("x: %.6f, y: %.6f, addx: %.6f, addy: %.6f, length: %.6f\n", x, y, addx, addy, length);
	x = start.x;
	y = start.y;

  point_t tmp;
  int count = 0;
	for(double i = 0; i < length; i += 0.001)
	{
	  x += addx;
	  y += addy;
    tmp.x = x;
    tmp.y = y;
    p.push_back(point_t());
    p[count] = tmp;
    //D printf("\t Point# %d, x: %.6f, y: %.6f\n", i, x, y);
    count++;
	}
  return p;
}
