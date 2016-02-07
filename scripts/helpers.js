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
