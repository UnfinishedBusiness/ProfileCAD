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
	for(double i = 0; i < length; i += 0.0001)
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
float geoGetLineLength(line_t l)
{
  float x,y,z;
  x = l.end.x - l.start.x;
  y = l.end.y - l.start.y;
  z = l.end.z - l.start.z;
  return sqrtf(x*x + y*y + z*z);
}
point_t geoGetLineMidpoint(line_t l)
{
  point_t m;
  m.x = (l.end.x + l.start.x) / 2;
  m.y = (l.end.y + l.start.y) / 2;
  m.z = (l.end.z + l.start.z) / 2;
  D printf("(geoGetLineMidpoint) Midpoint of (%.6f, %.6f) --- (%.6f, %.6f) is (%.6f, %.6f)\n", l.start.x, l.start.y, l.end.x, l.end.y, m.x, m.y);
  return m;
}
point_t geoRotatePointAroundPoint(point_t p, point_t o, float angle)
{
  float rad = angle * (3.14159265359 / 180.0);
  return point_t{ cosf(rad) * (p.x - o.x) - sinf(rad) * (p.y - o.y) + o.x,
                    sinf(rad) * (p.x - o.x) + cosf(rad) * (p.y - o.y) + o.y,
                    0 };
}
float geoRadiansToDegrees(float r)
{
  return r * 57.2957795;
}
float geoGetLineAngle(line_t l)
{
  float angle = atan2f(l.start.y - l.end.y, l.start.x - l.end.x);
  return angle;
}
line_t geoGetPerpendicularLine(line_t l, float d)
{
  int angle;
  point_t midpoint = geoGetLineMidpoint(l);
  if (d > 0)
  {
    angle = 270;
  }
  else
  {
    angle = 90;
  }
  line_t r = line_t{ midpoint, geoRotatePointAroundPoint(l.start, midpoint, angle) };
  float perp_angle = geoGetLineAngle(r);
  point_t real_endpoint = point_t{ midpoint.x + (fabs(d) * cosf(perp_angle)), midpoint.y + (fabs(d) * sinf(perp_angle)) };
  r.end = real_endpoint;
  D printf("(geoGetPerpendicularLine) New Line:  (%.6f, %.6f) --- (%.6f, %.6f)\n", r.start, r.end);
  D printf("(geoGetPerpendicularLine) Returned line of length %.6f and angle %.6f\n", geoGetLineLength(r), geoGetLineAngle(r));
  return r;
}
