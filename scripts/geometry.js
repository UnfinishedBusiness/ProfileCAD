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
function geoGetParallelLine(base_line, distance, relative_to_point)
{
  var base_line_length = geoGetLineLength(base_line);
  var perpendicular_line = geoGetPerpendicularLine(base_line, distance, relative_to_point);
  var perpendicular_line_angle = geoGetLineAngle(perpendicular_line);

  var half_line = geoExtendLineAngle(perpendicular_line.end, perpendicular_line_angle + 90, base_line_length / 2);
  var parallel_line  = geoExtendLineAngle(half_line.end, geoGetLineAngle(half_line), base_line_length);

  print("Line = " + VarDump(parallel_line));
  return parallel_line;
}
