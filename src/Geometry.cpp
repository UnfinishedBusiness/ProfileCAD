#include <Geometry.h>

using namespace std;

bool geoInTolerance(float a, float b, float t)
{
  float diff;
  if (a > b)
  {
    diff = a - b;
  }
  else
  {
    diff = b - a;
  }
  //printf("(geoInTolerance) Difference: %.6f, Plus: %.6f, Minus: %.6f\n", diff, fabs(t), -fabs(t));
  if (diff <= fabs(t) && diff >= -fabs(t))
  {
    return true;
  }
  else
  {
    return false;
  }
}
point_t geoGetLineIntersection(line_t l1, line_t l2)
{
  float a1 = (l1.start.y - l1.end.y) / (l1.start.x - l1.end.x);
  float b1 = l1.start.y - a1 * l1.start.x;
  float a2 = (l2.start.y - l2.end.y) / (l2.start.x - l2.end.x);
  float b2 = l2.start.y - a2 * l2.start.x;
  if (fabs(a1 - a2) < 0.000001)
  {
    D printf("(geoGetLineIntersection) Lines dont intersect!\n");
  }
  float x = (b2 - b1) / (a1 - a2);
  float y = a1 * x + b1;
  return point_t{x, y};
}
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
  //D printf("(geoGetLineMidpoint) Midpoint of (%.6f, %.6f) --- (%.6f, %.6f) is (%.6f, %.6f)\n", l.start.x, l.start.y, l.end.x, l.end.y, m.x, m.y);
  return m;
}
point_t geoRotatePointAroundPoint(point_t p, point_t o, float angle) //angle is in degrees!
{
  float rad = angle * (3.14159265359 / 180.0);
  return point_t{ geoRound(cosf(rad) * (p.x - o.x) - sinf(rad) * (p.y - o.y) + o.x),
                  geoRound(sinf(rad) * (p.x - o.x) + cosf(rad) * (p.y - o.y) + o.y), 0 };
}
float geoRadiansToDegrees(float r)
{
  return r * 57.2957795;
}
float geoDegreesToRadians(float r)
{
  return 2 * M_PI * (r / 360);
}

float geoGetLineAngle(line_t l)
{
  float angle = atan2f(l.start.y - l.end.y, l.start.x - l.end.x);
  return angle;
}
line_t geoRotateLine(line_t l, point_t o, float a)
{
  return line_t{ geoRotatePointAroundPoint(l.start, o, a), geoRotatePointAroundPoint(l.end, o, a) };
}
line_t geoExtendLine(line_t l, float d)
{
  float angle = geoGetLineAngle(l);
  point_t new_startpoint = point_t{ l.start.x + (fabs(d) * cosf(angle)), l.start.y + (fabs(d) * sinf(angle)) };
  point_t new_endpoint = point_t{ l.end.x + (fabs(d) * cosf(angle)), l.end.y + (fabs(d) * sinf(angle)) };
  float new_startpoint_distance = geoGetLineLength(line_t{ l.start, new_startpoint });
  float new_endpoint_distance = geoGetLineLength(line_t{ l.end, new_endpoint });
  if (new_startpoint_distance > new_endpoint_distance)
  {
    D printf("(geoExtendLine) Entending from start point\n");
    return line_t{ new_startpoint, l.end };
  }
  else
  {
    D printf("(geoExtendLine) Entending from end point\n");
    return line_t{ l.start, new_endpoint };
  }
}
line_t geoExtendLineAngle(point_t s, float angle, float d)
{
  point_t new_endpoint = point_t{ s.x + (fabs(d) * cosf(angle)), s.y + (fabs(d) * sinf(angle)) };
  return line_t{ s, new_endpoint };
}
line_t geoGetPerpendicularLine(line_t l, point_t direction, float d)
{
  int angle;
  point_t midpoint = geoGetLineMidpoint(l);
  //Get lines perpendicular on both sides and see which enpoint is closest to direction point
  line_t r;
  line_t r270 = line_t{ midpoint, geoRotatePointAroundPoint(l.start, midpoint, 270) };
  line_t r90 = line_t{ midpoint, geoRotatePointAroundPoint(l.start, midpoint, 90) };
  float r270_d = geoGetLineLength(line_t{ r270.end, direction });
  float r90_d = geoGetLineLength(line_t{ r90.end, direction });
  //D printf("(geoGetPerpendicularLine) R270 Distance: %.6f, R90 Distance: %.6f\n", r270_d, r90_d);
  if ( r90_d < r270_d )
  {
    //D printf("(geoGetPerpendicularLine) R270 is closer!\n");
    r = r270;
  }
  else
  {
    //D printf("(geoGetPerpendicularLine) R90 is closer!\n");
    r = r90;
  }
  float a = geoGetLineAngle(r);
  point_t new_endpoint = point_t{ midpoint.x + (fabs(d) * cosf(a)), midpoint.y + (fabs(d) * sinf(a)) };
  return line_t{ midpoint, new_endpoint };
}
line_t geoGetParallelLine(line_t l, point_t direction, float d)
{
  line_t perp = geoGetPerpendicularLine(l, direction, d);
  point_t midpoint = geoGetLineMidpoint(l);
  line_t r1 = geoExtendLineAngle(perp.end, geoGetLineAngle(l), geoGetLineLength(line_t{midpoint, l.end})); //Get half of line
  line_t r2 = geoExtendLineAngle(r1.end, geoGetLineAngle(l) + 3.14159265, geoGetLineLength(line_t{l.end, l.start})); //Get rest of line
  return r2;
}
point_t geoGetArcEndpoint(arc_t a, float length)
{
    float r = sqrtf(powf(a.start.x - a.center.x, 2) + powf(a.start.y - a.center.y, 2));
    float angle = atan2(a.start.y - a.center.y, a.start.x - a.center.x);
    if (a.direction == ARC_CW) {
        angle = angle - length / r;
    }
    else {
        angle = angle + length / r;
    }
    return point_t{a.center.x + r * cos(angle), a.center.y + r * sin(angle) };
}
float geoGetArcStartAngle(arc_t a)
{
  //return geoGetLineAngle(line_t{a.center, a.start});
  return atan2(a.start.y - a.center.y, a.start.x - a.center.x);
}
float geoGetArcEndAngle(arc_t a)
{
  //return geoGetLineAngle(line_t{a.center, a.end});
  return atan2(a.end.y - a.center.y, a.end.x - a.center.x);
}
point_t geoGetArcPoint(arc_t a, float angle) //Angle is in degrees
{
  point_t e = point_t{ a.radius + a.center.x, a.center.y, 0 };
  return geoRotatePointAroundPoint(e, a.center, angle);
}
float geoGetArchLength(arc_t a)
{
  float start_angle = geoGetArcStartAngle(a);
  float end_angle = geoGetArcEndAngle(a);
  //printf("Start angle: %.6f\nEnd angle: %.6f\n", start_angle, end_angle);
  float angle;
  float r;
  if (start_angle > end_angle)
  {
    angle = start_angle - end_angle;
    //cout << "start_angle > end_angle - difference= " << angle << "\n";
  }
  else
  {
    angle = end_angle - start_angle;
    //cout << "end_angle > start_angle - difference= " << angle << "\n";
  }
  if (a.direction == ARC_CW)
  {
      return a.radius * angle;
  }
  else
  {
     return (2 * M_PI * a.radius) - (a.radius * angle);
  }
}
float geoGetIncludedAngle(arc_t a)
{
  bool direction;
  float angle;
  if (a.direction == ARC_CCW)
  {
    direction = true;
  }
  else
  {
    direction = false;
  }
  if ((360 + (int)geoGetArcStartAngle(a) - (int)geoGetArcEndAngle(a)) % 360 < 180)
  {
    //cout << "True!\n";
    direction = !direction;
  }
  if (direction)
  {
    angle = fabs(geoGetArcStartAngle(a) - geoGetArcEndAngle(a));
  }
  else
  {
    angle = geoDegreesToRadians(360) - fabs(geoGetArcStartAngle(a) - geoGetArcEndAngle(a));
  }
  if (angle == 0)
  {
    return 360;
  }
  else
  {
    return angle;
  }
}
float geoRound(float x) //Round to 6 places
{
   return round(1000000 * x) / 1000000;
}
