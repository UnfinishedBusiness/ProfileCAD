#include <Geometry.h>

using namespace std;

bool geoInTolerance(point_t a, point_t b, float t)
{
  return (geoInTolerance(a.x, b.x, t) && geoInTolerance(a.y, b.y, t) && geoInTolerance(a.z, b.z, t));
}
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
string geoSupressZeros(float i)
{
  stringstream r;
  r << i;
  return r.str();
}
string geoBoolToString(bool i)
{
  stringstream r;
  r << i;
  return r.str();
}
line_t geoReplaceClosestEndpoint(line_t l, point_t p)
{
  line_t r = l;
  if (geoGetLineLength(line_t{ l.start, p }) < geoGetLineLength(line_t{ l.end, p}))
  {
    r.start = p;
  }
  else
  {
    r.end = p;
  }
  return r;
}
point_t geoGetIntersection(arc_t a, line_t l)
{
  return geoGetLineArcIntersection(a, l);
}
point_t geoGetIntersection(line_t l, arc_t a)
{
  return geoGetLineArcIntersection(a, l);
}
point_t geoGetIntersection(arc_t a1, arc_t a2)
{
  return geoGetArcIntersection(a1, a2);
}
point_t geoGetIntersection(line_t l1, line_t l2)
{
  return geoGetLineIntersection(l1, l2);
}
point_t geoGetLineIntersection(line_t l1, line_t l2)
{
  float a1 = l1.end.y - l1.start.y;
  float b1 = l1.start.x - l1.end.x;
  float c1 = a1 * l1.start.x + b1 * l1.start.y;
  float a2 = l2.end.y - l2.start.y;
  float b2 = l2.start.x - l2.end.x;
  float c2 = a2 * l2.start.x + b2 * l2.start.y;
  float det = a1 * b2 - a2 * b1;
  if (det == 0)
  {
        return point_t{NAN, NAN, NAN}; //Lines are parallel
  }
  float x = (b2 * c1 - b1 * c2) / det;
  float y = (a1 * c2 - a2 * c1) / det;
  return point_t{x, y, 0};
}
point_t geoGetLineArcIntersection(arc_t a, line_t l) //Not really the best way
{
  vector<point_t> av = geoGetPointsOfArc(a);
  vector<point_t> lv = geoGetPointsOfLine(l, 0.0005);
  for (int x = 0; x < lv.size(); x++)
  {
    for (int y = 0; y < av.size(); y++)
    {
      if (geoInTolerance(lv[x], av[y], 0.0001))
      {
        return lv[x];
      }
    }
  }
  return point_t{NAN, NAN, NAN}; //No intersection;*/
}
point_t geoGetArcIntersection(arc_t a1, arc_t a2)
{
  vector<point_t> av1 = geoGetPointsOfArc(a1);
  vector<point_t> av2 = geoGetPointsOfArc(a2);
  for (int x = 0; x < av1.size(); x++)
  {
    for (int y = 0; y < av2.size(); y++)
    {
      if (geoInTolerance(av1[x], av2[y], 0.0001))
      {
        return av1[x];
      }
    }
  }
  return point_t{NAN, NAN, NAN}; //No intersection;
}
bool geoDoLinesIntersect(line_t l1, line_t l2)
{
  // Store the values for fast access and easy
  // equations-to-code conversion
  point_t p1 = l1.start;
  point_t p2 = l1.end;
  point_t p3 = l2.start;
  point_t p4 = l2.end;

  float x1 = p1.x, x2 = p2.x, x3 = p3.x, x4 = p4.x;
  float y1 = p1.y, y2 = p2.y, y3 = p3.y, y4 = p4.y;

  float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
  // If d is zero, there is no intersection
  if (d == 0) return false;

  // Get the x and y
  float pre = (x1*y2 - y1*x2), post = (x3*y4 - y3*x4);
  float x = ( pre * (x3 - x4) - (x1 - x2) * post ) / d;
  float y = ( pre * (y3 - y4) - (y1 - y2) * post ) / d;

  // Check if the x and y coordinates are within both lines
  if ( x < min(x1, x2) || x > max(x1, x2) ||
  x < min(x3, x4) || x > max(x3, x4) ) return false;
  if ( y < min(y1, y2) || y > max(y1, y2) ||
  y < min(y3, y4) || y > max(y3, y4) ) return false;

  return true;
}
vector<point_t> geoGetPointsOfLine(line_t l)
{
  float scale = 0.005 / sceneGetScale();
  if (scale < 0.002)
  {
    //V cout << KRED << "(geoGetPointsOfLine) Min scale reached - scale = " <<  KGREEN << scale << KNORMAL << "\r";
    scale = 0.002;
  }
  return geoGetPointsOfLine(l, scale);
}
vector<point_t> geoGetPointsOfLine(line_t l, float scale)
{
  if (scale < 0.0001)
  {
    scale = 0.0001;
  }
  //V printf("(geoGetPointsOfLine) Start(%.6f, %.6f) End(%.6f, %.6f)\n", start.x, start.y, end.x, end.y);
  vector<point_t> p;
  float x = l.end.x - l.start.x;
	float y = l.end.y - l.start.y;
	float length = sqrtf( x*x + y*y );
  //float scale = 0.005;


	float addx = (x / length) * scale;
	float addy = (y / length) * scale;
	x = l.start.x;
	y = l.start.y;
  int count = 0;
	for(float i = 0; i < length; i += scale)
	{
		p.push_back(point_t());
    p[count] = point_t{ x, y };
	  x += addx;
	  y += addy;
    count ++;
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
  //V printf("(geoGetLineMidpoint) Midpoint of (%.6f, %.6f) --- (%.6f, %.6f) is (%.6f, %.6f)\n", l.start.x, l.start.y, l.end.x, l.end.y, m.x, m.y);
  return m;
}
point_t geoRotatePointAroundPoint(point_t p, point_t o, float angle, bool dir) //angle is in degrees!
{
  //Clockwise is negative and ccw is positive, backword to logic!
  float a;
  if (dir == ARC_CW)
  {
    a = -angle;
  }
  else
  {
    a = POSITIVE(angle);
  }
  return geoRotatePointAroundPoint(p, o, a);
}
point_t geoRotatePointAroundPoint(point_t p, point_t o, float angle) //angle is in degrees!
{
  float rad = angle * (3.14159265359 / 180.0);
  return point_t{ geoRound(cosf(rad) * (p.x - o.x) - sinf(rad) * (p.y - o.y) + o.x),
                  geoRound(sinf(rad) * (p.x - o.x) + cosf(rad) * (p.y - o.y) + o.y), 0 };
}
point_t geoTranslatePointByVector(point_t o, point_t v)
{
  o.x -= v.x;
  o.y -= v.y;
  return o;
}
point_t geoTranslateCalculateVector(point_t o, point_t t)
{
  return point_t{ (o.x - t.x), (o.y - t.y) };
}
float geoRadiansToDegrees(float r)
{
  return (r * 57.2957795);
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
    //V printf("(geoExtendLine) Entending from start point\n");
    return line_t{ new_startpoint, l.end };
  }
  else
  {
    //V printf("(geoExtendLine) Entending from end point\n");
    return line_t{ l.start, new_endpoint };
  }
}
line_t geoExtendLineEndpoint(line_t l, float d)
{
  float angle = geoGetLineAngle(l);
  point_t new_endpoint = point_t{ l.end.x + (fabs(d) * cosf(angle)), l.end.y + (fabs(d) * sinf(angle)) };
  return line_t{ l.start, new_endpoint };
}
line_t geoExtendLineStartpoint(line_t l, float d)
{
  float angle = geoGetLineAngle(l);
  point_t new_endpoint = point_t{ l.start.x + (fabs(d) * cosf(angle)), l.start.y + (fabs(d) * sinf(angle)) };
  return line_t{ new_endpoint, l.start };
}
line_t geoExtendLineAngle(point_t s, float angle, float d) //Angle is in radians
{
  point_t new_endpoint = point_t{ s.x + (fabs(d) * cosf(angle)), s.y + (fabs(d) * sinf(angle)) };
  return line_t{ s, new_endpoint };
}
line_t geoFlipLine(line_t l)
{
  return line_t{l.end, l.start};
}
arc_t geoFlipArc(arc_t a)
{
  arc_t r = a;
  r.direction = !a.direction;
  r.start = a.end;
  r.end = a.start;
  r.center = a.center;
  //V debugDumpArcStructure(r);
  return r;
}
float geoGetPerpendicularDistance(line_t l , point_t p)
{
  /*line_t perp = geoGetPerpendicularLine(l, p, 1); //Just give it any length, we have to find the intersection point
  float l_length = geoGetLineLength(l);
  float angle = geoGetLineAngle(l);
  line_t parallel_line = geoExtendLineAngle(p, angle, l_length / 2);
  point_t intersection = geoGetIntersection(parallel_line, perp);
  return geoGetLineLength(line_t{intersection, geoGetLineMidpoint(l)});*/
  // Return minimum distance between line segment vw and point p

  //double FindDistanceToSegment(double x1, double y1, double x2, double y2, double pointX, double pointY)
  float x1 = l.start.x;
  float y1 = l.start.y;
  float x2 = l.end.x;
  float y2 = l.end.y;
  float pointX = p.x;
  float pointY = p.y;
  float diffX = x2 - x1;
  float diffY = y2 - y1;
  if ((diffX == 0) && (diffY == 0))
  {
      diffX = pointX - x1;
      diffY = pointY - y1;
      return sqrtf(diffX * diffX + diffY * diffY);
  }
  float t = ((pointX - x1) * diffX + (pointY - y1) * diffY) / (diffX * diffX + diffY * diffY);
  if (t < 0)
  {
      //point is nearest to the first point i.e x1 and y1
      diffX = pointX - x1;
      diffY = pointY - y1;
  }
  else if (t > 1)
  {
      //point is nearest to the end point i.e x2 and y2
      diffX = pointX - x2;
      diffY = pointY - y2;
  }
  else
  {
      //if perpendicular line intersect the line segment.
      diffX = pointX - (x1 + t * diffX);
      diffY = pointY - (y1 + t * diffY);
  }
  //returning shortest distance
  return sqrtf(diffX * diffX + diffY * diffY);

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
  //V printf("(geoGetPerpendicularLine) R270 Distance: %.6f, R90 Distance: %.6f\n", r270_d, r90_d);
  if ( r90_d < r270_d )
  {
    //V printf("(geoGetPerpendicularLine) R270 is closer!\n");
    r = r270;
  }
  else
  {
    //V printf("(geoGetPerpendicularLine) R90 is closer!\n");
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
  return atan2(a.start.y - a.center.y, a.start.x - a.center.x);
}
float geoGetArcEndAngle(arc_t a)
{
  if (a.direction == ARC_CCW && geoGetArcStartAngle(a) > 0 && (atan2(a.end.y - a.center.y, a.end.x - a.center.x) + 6.28318531) < 6.28318531)
  {
    return atan2(a.end.y - a.center.y, a.end.x - a.center.x) + 6.28318531;
  }
  else
  {
    return atan2(a.end.y - a.center.y, a.end.x - a.center.x);
  }

}
float geoGetArcStartAngleAbs(arc_t a)
{
  //Draw line from arc center to start point and calculate angle of line
  float angle = geoRadiansToDegrees(geoGetLineAngle(line_t{ a.center, a.start}));
  //cout << KRED << "(geoGetArcStartAngleAbs) Angle = " << angle << KNORMAL << endl;
  return angle;
}
float geoGetArcEndAngleAbs(arc_t a)
{
  float angle = geoRadiansToDegrees(geoGetLineAngle(line_t{ a.center, a.end}));
  //cout << KRED << "(geoGetArcStartAngleAbs) Angle = " << angle << KNORMAL << endl;
  return angle;
}
point_t geoGetArcPoint(arc_t a, float angle) //Angle is in degrees
{
  point_t e = point_t{ a.radius + a.center.x, a.center.y, 0 };
  return geoRotatePointAroundPoint(e, a.center, angle);
}
std::vector<point_t> geoGetPointsOfArc(arc_t a)
{
  std::vector<point_t> r;
  int rx = 0;
  float includedAngle = geoGetIncludedAngle(a);
  line_t l = line_t{ point_t{a.center.x, a.center.y,} , point_t{a.start.x, a.start.y} };
  if (a.start != a.end)
  {
    r.push_back(point_t());
    r[rx] = a.start;
    rx++;
  }
  int steps;
  if (a.start == a.end)
  {
    //Were a circle
    steps = 361; //Go the full 360 to close the gap
  }
  else
  {
    steps = geoGetIncludedAngle(a);
  }
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
    r.push_back(point_t());
    r[rx] = l.end;
    rx++;
  }
  if (a.start != a.end)
  {
    r.push_back(point_t());
    r[rx] = a.end;
    rx++;
  }
  return r;
}
float geoGetArcLength(arc_t a) //Not varified
{
  float angle = geoGetIncludedAngle(a);
  return a.radius * angle;
}
point_t geoGetArcMidpoint(arc_t a)
{
  float c=atan2(a.start.y + a.end.y, a.start.x + a.end.x);
  return point_t{a.radius * cosf(c) + a.center.x, a.radius * sinf(c) + a.center.y};
}
float geoGetIncludedAngle(arc_t a)
{
  float angle;
  /*if (a.direction == ARC_CCW)
  {
    angle = geoGetArcEndAngle(a) - geoGetArcStartAngle(a);
  }
  else
  {
    angle = geoGetArcStartAngle(a) - geoGetArcEndAngle(a) + geoDegreesToRadians(360);
  }*/
  angle = geoGetArcEndAngle(a) - geoGetArcStartAngle(a);
  if (angle == 0)
  {
    return 360;
  }
  else
  {
    return geoRadiansToDegrees(fabs(angle));
  }
}
bool geoGetArcDirection(float s, float e)
{
  float direction;
  if (s < e)
  {
    direction = ARC_CCW;
  }
  else
  {
    direction = ARC_CW;
  }
  /*if ((360 + (int)s - (int)e) % 360 < 180)
  {
    //cout << "True!\n";
    direction = !direction;
  }*/
  return direction;
}
float geoRound(float x) //Round to 6 places
{
   return round(1000000 * x) / 1000000;
}
arc_t geoGetCircle(point_t p, float r)
{
  arc_t c;
  c.radius = r;
  c.center = p;
  V debugDumpPointStructure(p);
  c.start = p;
  c.start.x += c.radius;
  c.end = c.start;
  c.direction = ARC_CW;
  return c;
}
std::vector<cadEntity> geoOffsetContour(contour_t c, bool s, float d)
{
  std::vector<cadEntity> v;
  cadEntity e;
  for (int x = 0; x < c.Entitys.size(); x++)
  {
    if ( c.Entitys[x].Type == CAD_LINE)
    {
      if (s == CONTOUR_RIGHT)
      {
        line_t l = geoExtendLineAngle(geoGetLineMidpoint(c.Entitys[x].Line), geoGetLineAngle(c.Entitys[x].Line) + geoDegreesToRadians(90), 0.1);
        line_t p = geoGetParallelLine(c.Entitys[x].Line, l.end, d);
        e.Type = CAD_LINE;
        e.Line = p;
        v.push_back(e);
      }
      if (s == CONTOUR_LEFT)
      {
        line_t l = geoExtendLineAngle(geoGetLineMidpoint(c.Entitys[x].Line), geoGetLineAngle(c.Entitys[x].Line) - geoDegreesToRadians(90), 0.1);
        line_t p = geoGetParallelLine(c.Entitys[x].Line, l.end, d);
        e.Type = CAD_LINE;
        e.Line = p;
        v.push_back(e);
      }
    }
    if ( c.Entitys[x].Type == CAD_ARC)
    {
      if (s == CONTOUR_RIGHT && c.Entitys[x].Arc.direction == ARC_CW)
      {
        //Radius is -
        cout << "RIGHT Arc CW" << endl;
        e = c.Entitys[x];
        e.Arc.radius -= d;
        if (e.Arc.radius < 0)
        {
          V cout << "Arc not posible, making square corner!" << endl;
        }
        else
        {
          line_t start_line = line_t{e.Arc.center, e.Arc.start};
          e.Arc.start = geoExtendLineAngle(e.Arc.center, geoGetLineAngle(start_line) + geoDegreesToRadians(180) , e.Arc.radius).end;
          line_t end_line = line_t{e.Arc.center, e.Arc.end};
          e.Arc.end = geoExtendLineAngle(e.Arc.center, geoGetLineAngle(end_line) - geoDegreesToRadians(180) , e.Arc.radius).end;
          e.Type = CAD_ARC;
          v.push_back(e);
        }
      }
      else if (s == CONTOUR_RIGHT && c.Entitys[x].Arc.direction == ARC_CCW)
      {
        cout << "RIGHT Arc CCW" << endl;
        //Radius is +
        e = c.Entitys[x];
        e.Arc.radius += d;
        e.Arc.direction = !e.Arc.direction;
        line_t start_line = line_t{e.Arc.center, e.Arc.start};
        e.Arc.start = geoExtendLineAngle(e.Arc.center, geoGetLineAngle(start_line) - geoDegreesToRadians(90), e.Arc.radius).end;
        line_t end_line = line_t{e.Arc.center, e.Arc.end};
        e.Arc.end = geoExtendLineAngle(e.Arc.center, geoGetLineAngle(end_line) + geoDegreesToRadians(90), e.Arc.radius).end;
        e.Type = CAD_ARC;
        v.push_back(e);
      }
      else if (s == CONTOUR_LEFT && c.Entitys[x].Arc.direction == ARC_CW)
      {
        cout << "LEFT Arc CW" << endl;
        //Radius is +
        e = c.Entitys[x];
        e.Arc.radius += d;
        //e.Arc.direction = !e.Arc.direction;
        line_t start_line = line_t{e.Arc.center, e.Arc.start};
        e.Arc.start = geoExtendLineAngle(e.Arc.center, geoGetLineAngle(start_line) - geoDegreesToRadians(180), e.Arc.radius).end;
        line_t end_line = line_t{e.Arc.center, e.Arc.end};
        e.Arc.end = geoExtendLineAngle(e.Arc.center, geoGetLineAngle(end_line) + geoDegreesToRadians(180), e.Arc.radius).end;
        e.Type = CAD_ARC;
        v.push_back(e);
      }
      else if (s == CONTOUR_LEFT && c.Entitys[x].Arc.direction == ARC_CCW)
      {
        cout << "LEFT Arc CCW" << endl;
        //Radius is -
        e = c.Entitys[x];
        e.Arc.radius -= d;
        if (e.Arc.radius < 0)
        {
          V cout << "Arc not posible, making square corner!" << endl;

        }
        else
        {
          line_t start_line = line_t{e.Arc.center, e.Arc.start};
          e.Arc.start = geoExtendLineAngle(e.Arc.center, geoGetLineAngle(start_line) + geoDegreesToRadians(180) , e.Arc.radius).end;
          line_t end_line = line_t{e.Arc.center, e.Arc.end};
          e.Arc.end = geoExtendLineAngle(e.Arc.center, geoGetLineAngle(end_line) - geoDegreesToRadians(180) , e.Arc.radius).end;
          e.Type = CAD_ARC;
          v.push_back(e);
        }
      }
    }
  }
  for (int x = 0; x < v.size()-1; x++)
  {
    if (v[x].Type == CAD_LINE && v[x+1].Type == CAD_LINE)
    {
      //Trim last line to this line via intersection
      point_t intersection = geoGetIntersection(v[x].Line, v[x+1].Line);
      v[x].Line.end = intersection;
      v[x+1].Line.start = intersection;

    }
  }
  if (c.isClosed && v.back().Type == CAD_LINE && v.front().Type == CAD_LINE) //Were a closed contour
  {
    V cout << KRED << "(geoOffsetContour)" << KGREEN << "Trimming endpoints!" << KNORMAL << endl;
    point_t intersection = geoGetIntersection(v.back().Line, v.front().Line);
    v.back().Line.end = intersection;
    v.front().Line.start = intersection;
  }
  return v;
}
point_t geoScalePoint(point_t p, float s)
{
  p.x = p.x * s;
  p.y = p.y * s;
  p.z = p.z * s;
  return p;
}
cadEntity geoScaleEntity(cadEntity e, float s)
{
  if (e.Type == CAD_LINE)
  {
    e.Line.start = geoScalePoint(e.Line.start, s);
    e.Line.end = geoScalePoint(e.Line.end, s);
  }
  if (e.Type == CAD_ARC)
  {
    e.Arc.start = geoScalePoint(e.Arc.start, s);
    e.Arc.end = geoScalePoint(e.Arc.end, s);
    e.Arc.center = geoScalePoint(e.Arc.center, s);
    e.Arc.radius = e.Arc.radius * s;
  }
  return e;
}
