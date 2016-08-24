VerboseKeycode = false;
Mouse = {};
MouseMotionCallback = "";
MouseClickCallback = "";
Live = {};
CancelAction = false;
CurrentFile = "";

function main()
{
  Source("scripts/helpers.js");
  Source("scripts/cli.js");
  Source("scripts/drawing.js");
  Source("scripts/xform.js");
  Source("scripts/advanced_drawing.js");
  Source("scripts/popup_menu.js");
  Source("scripts/geometry.js");
  Source("scripts/file_converters.js");
  Source("scripts/screen.js");

  Popup_MainMenu();
  SetDrawColor("green");


  //Entry point for scriptRun()!
  //DrawLine({x: 0, y: 0}, {x: -4, y: 0});
}
function OnFileOpenDialog(file)
{
  ClearEntityStack();
  print("Opening " + file);
  CurrentFile = file;
  var re = /(?:\.([^.]+))?$/;
  var ext = re.exec(CurrentFile)[1].toLowerCase();
  //print(ext);
  if (ext == "pfcad")
  {
    Read_PFCAD();
  }
  if (ext == "dxf")
  {
    Read_DXF();
  }
}
function OnFileSaveDialog(file)
{
  print("Saving " + file);
  CurrentFile = file;
  var re = /(?:\.([^.]+))?$/;
  var ext = re.exec(CurrentFile)[1].toLowerCase();
  //print(ext);
  if (ext == "pfcad" || ext == "")
  {
    Write_PFCAD();
  }
  if (ext == "dxf")
  {
    Write_DXF();
  }
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
    //print("Dumping enties!");
    //DumpEntityStack();
    print("Number of Selected Entities - ");
    print(JSON.stringify(GetSelectedEntities()));
  }
  else if (mod == "Ctrl" && keycode == 65) //Ctrl-a
  {
    SelectAllEntities();
  }
  else if (mod == "Ctrl" && keycode == 79) //Ctrl-o
  {
    //Keep a variable with the current filename. if theres no current file name open, use the file open dialog!
    //OpenFile("test/dxf/boltpattern.dxf");
    if (CurrentFile == "")
    {
      FileOpenDialog();
    }
    OnFileOpenDialog(CurrentFile);
  }
  else if (mod == "Ctrl" && keycode == 83) //Ctrl-s
  {
    if (CurrentFile == "")
    {
      FileSaveDialog();
    }
    OnFileSaveDialog(CurrentFile);
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
