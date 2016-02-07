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
