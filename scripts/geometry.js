function geoInToleranceSimple(a, b, t)
{
  var diff;
  if (a > b)
  {
    diff = a - b;
  }
  else
  {
    diff = b - a;
  }
  //printf("(geoInTolerance) Difference: %.6f, Plus: %.6f, Minus: %.6f\n", diff, fabs(t), -fabs(t));
  if (diff <= Math.abs(t) && diff >= -Math.abs(t))
  {
    return true;
  }
  else
  {
    return false;
  }
}
function geoInTolerance(point1, point2, t)
{
  //print(JSON.stringify(point1));
  //print(JSON.stringify(point2));
  return (geoInToleranceSimple(point1.x, point2.x, t) && geoInToleranceSimple(point1.y, point2.y, t));
}
function geoGetLineLength(line)
{
  var x,y;
  x = line.end.x - line.start.x;
  y = line.end.y - line.start.y;
  return Math.sqrt(x*x + y*y);
}
function geoGetDistanceBetweenPoints(one, two)
{
  var line = {};
  line.start = one;
  line.end = two;
  return geoGetLineLength(line);
}
function geoGetLineAngle(line)
{
  return toDegrees(Math.atan2(line.start.y - line.end.y, line.start.x - line.end.x));
}
function geoRotateLine(line, origin, angle)
{
  //print("Line: " + JSON.stringify(line));
  //print("Origin: " + JSON.stringify(origin));
  var new_line = {}
  new_line.start = geoRotatePointAroundPoint(line.start, origin, angle);
  new_line.end = geoRotatePointAroundPoint(line.end, origin, angle);
  return new_line;
}
function geoRotatePointAroundPoint(p, o, angle) //angle is in degrees!
{
  var rad = angle * (3.14159265359 / 180.0);
  //print("Point: " + JSON.stringify(p));
  //print("Origin: " + JSON.stringify(o));
  //print("Angle: " + angle);
  var x = Math.cos(rad) * (p.x - o.x) - Math.sin(rad) * (p.y - o.y) + o.x;
  var y = Math.sin(rad) * (p.x - o.x) + Math.cos(rad) * (p.y - o.y) + o.y;
  return { x: parseFloat(x).toFixed(4), y: parseFloat(y).toFixed(4) };
}
function geoGetPerpendicularDistance(base_line, point)
{
  var x1 = Number(base_line.start.x);
  var  y1 = Number(base_line.start.y);
  var  x2 = Number(base_line.end.x);
  var  y2 = Number(base_line.end.y);
  var  pointX = Number(point.x);
  var  pointY = Number(point.y);
  var  diffX = x2 - x1;
  var  diffY = y2 - y1;
  if ((diffX == 0) && (diffY == 0))
  {
      diffX = pointX - x1;
      diffY = pointY - y1;
      return Math.sqrt(diffX * diffX + diffY * diffY);
  }
  var  t = ((pointX - x1) * diffX + (pointY - y1) * diffY) / (diffX * diffX + diffY * diffY);
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
  return Math.sqrt(diffX * diffX + diffY * diffY);
}
function geoGetLineMidpoint(line)
{
  var midpoint = {};
  midpoint.x = (Number(line.end.x) + Number(line.start.x)) / 2;
  midpoint.y = (Number(line.end.y) + Number(line.start.y)) / 2;
  return midpoint;
}
function geoExtendLineAngle(point, angle, distance)
{
  //print("Point: " + VarDump(point) + " Angle: " + angle + " Distance: " + distance);
  var new_endpoint = {};
  new_endpoint.x = Number(point.x) + (Math.abs(Number(distance)) * Math.cos(Number(toRadians(angle))));
  new_endpoint.y = Number(point.y) + (Math.abs(Number(distance)) * Math.sin(Number(toRadians(angle))));

  var ret = {};
  ret.start = {x: point.x, y: point.y };
  ret.end = {x: Number(new_endpoint.x.toFixed(4)), y: Number(new_endpoint.y.toFixed(4))};
  //print("[geoExtendLineAngle] ret = " + VarDump(ret));
  return ret;
}
function geoGetPerpendicularLine(base_line, distance, relative_to_point)
{
  var base_angle = geoGetLineAngle(base_line)
  //print("Base angel " + base_angle);
  var midpoint = geoGetLineMidpoint(base_line);
  //print("Line Midpoint: " + VarDump(midpoint));
  var ret_line = {};
  ret_line.start = {x: midpoint.x, y: midpoint.y};
  var extended_line_one = geoExtendLineAngle(ret_line.start, base_angle + 90, distance);
  var extended_line_two = geoExtendLineAngle(ret_line.start, base_angle + 270, distance);
  if (geoGetDistanceBetweenPoints(extended_line_one.end, relative_to_point) < geoGetDistanceBetweenPoints(extended_line_two.end, relative_to_point))
  {
    return extended_line_one;
  }
  else
  {
    return extended_line_two;
  }

}
function geoGetLineIntersection(line_one, line_two)
{
  var  a1 = line_one.end.y - line_one.start.y;
  var  b1 = line_one.start.x - line_one.end.x;
  var  c1 = a1 * line_one.start.x + b1 * line_one.start.y;
  var  a2 = line_two.end.y - line_two.start.y;
  var  b2 = line_two.start.x - line_two.end.x;
  var  c2 = a2 * line_two.start.x + b2 * line_two.start.y;
  var  det = a1 * b2 - a2 * b1;
  if (det == 0)
  {
        return "None"; //Lines are parallel
  }
  var  x_int = (b2 * c1 - b1 * c2) / det;
  var  y_int = (a1 * c2 - a2 * c1) / det;
  return {x: x_int, y: y_int,};
}
function geoGetLineArcIntersection(line, arc)
{
  //p1 is the first line point
  //p2 is the second line point
  //c is the circle's center
  //r is the circle's radius

  p1 = line.start;
  p2 = line.end;

  c = arc.center;
  r = arc.radius;

  var p3 = {x:p1.x - c.x, y:p1.y - c.y}; //shifted line points
  var p4 = {x:p2.x - c.x, y:p2.y - c.y};

  var m = (p4.y - p3.y) / (p4.x - p3.x); //slope of the line
  if (m == Infinity)
  {

  }
  else
  {
    var b = p3.y - m * p3.x; //y-intercept of line

    var underRadical = Math.pow(r,2) * Math.pow(m,2) + Math.pow(r,2) - Math.pow(b,2); //the value under the square root sign

    print("p1: " + JSON.stringify(p1));
    print("p2: " + JSON.stringify(p2));
    print("p3: " + JSON.stringify(p3));
    print("p4: " + JSON.stringify(p4));
    print("m: " + m);
    print("b: " + b);
    print("underRadical: " + underRadical);

    if (underRadical < 0)
    {
        //No Intersection
        return "None";
    }
    else
    {
        var t1 = (-m*b + Math.sqrt(underRadical))/(Math.pow(m,2) + 1); //one of the intercept x's
        var t2 = (-m*b - Math.sqrt(underRadical))/(Math.pow(m,2) + 1); //other intercept's x
        var i1 = {x:t1+c.x, y:m*t1+b+c.y}; //intercept point 1
        var i2 = {x:t2+c.x, y:m*t2+b+c.y}; //intercept point 2

        i1.x = i1.x.toFixed(4);
        i1.y = i1.y.toFixed(4);
        i2.x = i2.x.toFixed(4);
        i2.y = i2.y.toFixed(4);
        return [i1, i2];
    }
  }


}
function geoPointMe(p)
{
  var length = 0.050;
  var line = {};
  line.type = "line";
  line.start = { x: p.x, y: p.y };
  line.end = { x: p.x + length, y: p.y };
  DrawLine(line.start, line.end);

  line.type = "line";
  line.start = { x: p.x, y: p.y };
  line.end = { x: p.x - length, y: p.y };
  DrawLine(line.start, line.end);

  line.type = "line";
  line.start = { x: p.x, y: p.y };
  line.end = { x: p.x, y: p.y + length};
  DrawLine(line.start, line.end);

  line.type = "line";
  line.start = { x: p.x, y: p.y };
  line.end = { x: p.x, y: p.y - length};
  DrawLine(line.start, line.end);

}
function geoGetParallelLine(base_line, distance, relative_to_point)
{
  var base_line_length = geoGetLineLength(base_line);
  var perpendicular_line = geoGetPerpendicularLine(base_line, distance, relative_to_point);
  var perpendicular_line_angle = geoGetLineAngle(perpendicular_line);

  var half_line = geoExtendLineAngle(perpendicular_line.end, perpendicular_line_angle + 90, base_line_length / 2);
  var parallel_line  = geoExtendLineAngle(half_line.end, geoGetLineAngle(half_line), base_line_length);

  //print("Line = " + VarDump(parallel_line));
  return parallel_line;
}
