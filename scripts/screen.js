function ScreenChangeColor()
{
  var yplace = 30;
  DialogClear();
  DialogAddStaticBox({ text: "Color", position: { x: 10, y: 0 }, size: { x: 280, y: 200 }});
  var Red = DialogAddRadioButton({ text: "Red", position: { x: 20, y: yplace }});
  xplace = 20;
  yplace += 25;
  var Green = DialogAddRadioButton({ text: "Green", position: { x: xplace, y: yplace }});
  yplace += 25;
  var Blue = DialogAddRadioButton({ text: "Blue", position: { x: xplace, y: yplace }});
  yplace += 25;
  var Purple = DialogAddRadioButton({ text: "Purple", position: { x: xplace, y: yplace }});
  yplace += 25;
  var Yellow = DialogAddRadioButton({ text: "Yellow", position: { x: xplace, y: yplace }});
  yplace = 30;
  xplace += 90;
  var Magenta = DialogAddRadioButton({ text: "Magenta", position: { x: xplace, y: yplace }});
  yplace += 25;
  var LightGrey = DialogAddRadioButton({ text: "Light Grey", position: { x: xplace, y: yplace }});
  yplace += 25;
  var DarkGrey = DialogAddRadioButton({ text: "Dark Grey", position: { x: xplace, y: yplace }});
  yplace += 25;

  DialogAddButton({ text: "OK", position: { x: 100, y: 100 }, size: { x: 70, y: 30 }, callback: function(){
    DialogClose();
    if (GetSelectedEntities() != undefined)
    {
      var Color = "green"; //Default Color
      if (DialogGetRadioButtonValue(Red))
      {
        Color = "red";
      }
      if (DialogGetRadioButtonValue(Green))
      {
        Color = "green";
      }
      if (DialogGetRadioButtonValue(Blue))
      {
        Color = "blue";
      }
      if (DialogGetRadioButtonValue(Purple))
      {
        Color = "purple";
      }
      if (DialogGetRadioButtonValue(Magenta))
      {
        Color = "magenta";
      }
      if (DialogGetRadioButtonValue(LightGrey))
      {
        Color = "lightgrey";
      }
      if (DialogGetRadioButtonValue(DarkGrey))
      {
        Color = "darkgrey";
      }
      if (DialogGetRadioButtonValue(Yellow))
      {
        Color = "yellow";
      }
      //print("Choose Color: " + Color);
      cliPrompt("Changed selected entities to " + Color);
      var selected = GetSelectedEntities();
      for (var i = 0; i < selected.length; i++)
      {
        var e = GetEntity(selected[i]);
        //print("Changed entity: " + selected[i]);
        //print(JSON.stringify(e));
        e.color = Color;
        //print(JSON.stringify(e));
        EditEntity(selected[i], e);
      }
      UnSelectAllEntities();
    }
    else
    {
      cliPrompt("Must have entities selected!");
    }
  }});
  DialogAddButton({ text: "Close", position: { x: 100, y: 200 }, size: { x: 70, y: 30 }, callback: function(){
    DialogClose();
  }});

  DialogShow({ title: "Change Color", size: { width: 300, height: 300 }});
}
