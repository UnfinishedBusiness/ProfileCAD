ButtonCallbackStack = [];

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
function GetSelectedEntities(i)
{
  var e = NativeGetSelectedEntities();
  if (e == "None")
  {
    return undefined;
  }
  else
  {
      return JSON.parse(e);
  }
}
function DrawLine(start, end)
{
  return NativeDrawLine2D(start.x,start.y,end.x,end.y);
}
function DrawArc(start, end, radius, direction)
{
  return NativeDrawArc2D(start.x,start.y,end.x,end.y,radius, direction);
}
function ShowLiveEntity(obj)
{
  NativeShowLiveEntity(JSON.stringify(obj));
}
function MouseGetSnap()
{
  var snap = NativeMouseGetSnap();
  if (snap != "None")
  {
    return JSON.parse(NativeMouseGetSnap());
  }
  else
  {
    return "None";
  }
}
function ExecuteButtonCallback(id)
{
  //print("Button Callback ID: " + id);
  for(var i = 0; i < ButtonCallbackStack.length; i++)
  {
    if (ButtonCallbackStack[i].id == id)
    {
      ButtonCallbackStack[i].callback();
    }
  }
}
function DialogAddButton(obj)
{
  var n = {};
  n.id = NativeDialogAddButton(JSON.stringify(obj));
  n.callback = obj.callback;
  ButtonCallbackStack.push(n);
}
function DialogAddStaticBox(obj)
{
  NativeDialogAddStaticBox(JSON.stringify(obj));
}
function DialogAddTextBox(obj)
{
  if (obj != undefined)
  {
    if (!obj.hasOwnProperty('default_text'))
    {
      obj.default_text = "";
    }
    NativeDialogAddTextBox(JSON.stringify(obj));
  }
}
function DialogAddRadioButton(obj)
{
  NativeDialogAddRadioButton(JSON.stringify(obj));
}
function DialogShow(obj)
{
  if (obj != undefined)
  {
    if (!obj.hasOwnProperty('size'))
    {
      obj.size = { width: 200, height: 200};
    }
    if (!obj.hasOwnProperty('title'))
    {
      obj.title = "Untitled";
    }
    //print("OBJ = " + VarDump(obj));
    NativeDialogShow(JSON.stringify(obj));
  }
}
function VarDump(o)
{
  return JSON.stringify(o);
}
function toDegrees (angle) {
  return angle * (180 / Math.PI);
}
function toRadians (angle) {
  return angle * (Math.PI / 180);
}
