function ParseHelper(key, json)
{
  //print("Looking for " + key + " in " + json);
  var obj = JSON.parse(json);
  return eval("obj." + key);
}
function DumpEntity(i)
{
  var e = GetEntity(i);
  if (e.type == "line")
  {
    print("Start point: X:" + e.start.x + " Y: " + e.start.y);
    print("End point: X:" + e.end.x + " Y: " + e.end.y);
  }
  if (e.type == "arc")
  {
    print("Start point: X:" + e.start.x + " Y: " + e.start.y);
    print("End point: X:" + e.end.x + " Y: " + e.end.y);
    print("Radius: " + e.radius);
    print("Direction: " + e.direction);
  }
}
function EditEntity(i, e) //Should always GetEntity before we edit it to avoid losing properties!
{
  //Use like follows!
  /*var id = DrawLine({x: 0, y: 0}, {x: -1.5086, y: 0});
  var e = GetEntity(id);
  e.end.x = 0;
  e.end.y = 5;
  EditEntity(0, e);
  */
  NativeEditEntity(i, JSON.stringify(e));
}
function GetEntity(i)
{
  return JSON.parse(NativeGetEntity(i));
}
function DrawLine(start, end)
{
  return NativeDrawLine2D(start.x,start.y,end.x,end.y);
}
function DrawArc(start, end, radius, direction)
{
  return NativeDrawArc2D(start.x,start.y,end.x,end.y,radius, direction);
}
