VerboseKeycode = false;
Mouse = {};
function main()
{
  Source("scripts/helpers.js");
  Source("scripts/cli.js");
  Source("scripts/drawing.js");
  SetDrawColor("green");
  //Entry point for scriptRun()!
  //DrawLine({x: 0, y: 0}, {x: -4, y: 0});
}
function OnMouseMotion(x, y)
{
  Mouse.x = x;
  Mouse.y = y;
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
  else if (mod == "None" && keycode == 32) //Space
  {
    //SetStatusText('Test!');
    DrawLineVerticalFromOrigin();
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
