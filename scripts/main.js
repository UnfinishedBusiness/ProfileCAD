VerboseKeycode = false;

function DrawLine(start, end)
{
  NativeDrawLine2D(start.x,start.y,end.x,end.y);
}

function main()
{
  SetDrawColor("green");
  //Entry point for scriptRun()!
  //DrawLine({x: 0, y: 0}, {x: -4, y: 0});
}

function OnKeyDown(mod, keycode)
{
  if (VerboseKeycode)
  {
    print("KeyCode: " + mod + "+" + keycode + "\n");
  }
  if (mod == "None" && keycode == 127) //Delete
  {
    RemoveSelectedEntities();
  }
  if (mod == "None" && keycode == 32) //Space
  {
    DrawLine({x: 0, y: 0}, {x: -1.5086, y: 0});
    print("Number of Entites: " + CountEntities() + "\n");
  }
  if (mod == "Ctrl" && keycode == 65) //Ctrl-a
  {
    SelectAllEntities();
  }
  if (mod == "Ctrl" && keycode == 79) //Ctrl-o
  {
    OpenFile("test/dxf/box.dxf");
  }
  if (mod == "Ctrl" && keycode == 85) //Ctrl-u
  {
    UnSelectAllEntities();
  }
  if (mod == "Ctrl" && keycode == 86) //Ctrl-v
  {
    print("Toggling Verbose Keycode\n");
    VerboseKeycode = !VerboseKeycode;
  }
  if (mod == "None" && keycode == "9")
  {
    print("\x1B[2J\x1B[H");
    DumptEntityStack();
  }
}
