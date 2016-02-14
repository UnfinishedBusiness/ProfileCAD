VerboseKeycode = false;
Mouse = {};
MouseMotionCallback = "";
MouseClickCallback = "";
Live = {};
CancelAction = false;

function main()
{
  Source("scripts/helpers.js");
  Source("scripts/cli.js");
  Source("scripts/drawing.js");
  Source("scripts/advanced_drawing.js");
  Source("scripts/popup_menu.js");
  Source("scripts/geometry.js");

  Popup_MainMenu();
  SetDrawColor("green");


  //Entry point for scriptRun()!
  //DrawLine({x: 0, y: 0}, {x: -4, y: 0});
}
function ClearMouseCallback()
{
  MouseMotionCallback = "";
  MouseClickCallback = "";
}
function OnIdle()
{
  if (CancelAction == true)
  {
    CancelAction = false;
    ClearMouseCallback();
    cliReturn();
    HideLiveEntity();
    SetStatusText("");
  }
}
function OnMouseClick()
{
  if (MouseClickCallback != "")
  {
    MouseClickCallback();
  }
}
function OnMouseMotion(x, y)
{
  Mouse.x = x;
  Mouse.y = y;
  if (MouseMotionCallback != "")
  {
      MouseMotionCallback();
  }
}
function OnKeyDown(mod, keycode)
{
  if (VerboseKeycode)
  {
    print("KeyCode: " + mod + "+" + keycode + "\n");
  }
  else if (mod == "None" && keycode == 127) //Delete
  {
    RemoveSelectedEntities();
  }
  else if (mod == "None" && keycode == 27) //esc
  {
    ClearMouseCallback();
    cliReturn();
    HideLiveEntity();
    SetStatusText("");
    CancelAction = true;
  }
  else if(mod == "None" && keycode == 308) //Left Ctrl
  {
    //print("Mouse Position - " + VarDump(Mouse));
  }
  else if (mod == "None" && keycode == 32) //Space
  {
    AdvancedDrawingDrawGear();
  }
  else if (mod == "Ctrl" && keycode == 65) //Ctrl-a
  {
    SelectAllEntities();
  }
  else if (mod == "Ctrl" && keycode == 79) //Ctrl-o
  {
    OpenFile("test/dxf/boltpattern.dxf");
  }
  else if (mod == "Ctrl" && keycode == 85) //Ctrl-u
  {
    UnSelectAllEntities();
  }
  else if (mod == "Ctrl" && keycode == 86) //Ctrl-v
  {
    print("Toggling Verbose Keycode\n");
    VerboseKeycode = !VerboseKeycode;
  }
  else if (mod == "None" && keycode == "9")
  {
    print("\x1B[2J\x1B[H");
    DumptEntityStack();
  }
  else
  {
    cliPush(mod, keycode);
  }
}
