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

    DialogClear();
    DialogAddStaticBox({ text: "Translate", position: { x: 50, y: 0 }, size: { x: 200, y: 100 }});
    var textbox = DialogAddTextBox({ default_text: "test", position: { x: 50, y: 125 } });
    DialogAddRadioButton({ text: "Move", position: { x: 50, y: 100 } });
    DialogAddStaticText({ text: "Static Text", position: { x: 50, y: 60 }});
    DialogAddButton({ text: "OK", position: { x: 100, y: 100 }, size: { x: 70, y: 30 }, callback: function(){
      print("Text Box says: " + DialogGetTextboxValue(textbox));
    }});
    DialogAddButton({ text: "Close", position: { x: 100, y: 200 }, size: { x: 70, y: 30 }, callback: function(){
      print("Close Clicked!");
      DialogClose();
    }});

    DialogShow({ title: "Test Dialog", size: { width: 300, height: 300 }});
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
