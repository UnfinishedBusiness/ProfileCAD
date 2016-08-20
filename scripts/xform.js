function XformScale()
{
  var yplace = 30;
  DialogClear();
  DialogAddStaticBox({ text: "Scale Factor", position: { x: 10, y: 0 }, size: { x: 280, y: 200 }});
  var textbox = DialogAddTextBox({ default_text: "1", position: { x: 90, y: (yplace - 2) } });
  yplace += 55;

  var move_option = DialogAddRadioButton({ text: "Move", position: { x: 20, y: yplace }});
  yplace += 30;

  var copy_option = DialogAddRadioButton({ text: "Copy", position: { x: 20, y: yplace }});
  yplace += 30;



  //var move_option = DialogAddRadioButton({ text: "Move", position: { x: 20, y: yplace });
  //yplace += 30;

  DialogAddButton({ text: "OK", position: { x: 100, y: 100 }, size: { x: 70, y: 30 }, callback: function(){
    print("Scale Factor = " + DialogGetTextboxValue(textbox) + " Copy = " + DialogGetRadioButtonValue(copy_option));
    DialogClose();
    if (GetSelectedEntities() != undefined)
    {
      if (DialogGetRadioButtonValue(copy_option) == false)
      {
        var scale = parseFloat(DialogGetTextboxValue(textbox));
        var selected = GetSelectedEntities();
        print("Getting ready to scale " + selected.length + " selected entities!");
        for (var i = 0; i < selected.length; i++)
        {
          var e = GetEntity(selected[i]);
          if (e.type == "line")
          {
            print(" - Scaling Entity " + i + " which is a line");
            e.start.x = e.start.x * scale;
            e.end.x = e.end.x * scale;

            e.start.y = e.start.y * scale;
            e.end.y = e.end.y * scale;
          }
          if (e.type == "arc")
          {
            print(" - Scaling Entity " + i + " which is a line");
            e.start.x = e.start.x * scale;
            e.end.x = e.end.x * scale;

            e.start.y = e.start.y * scale;
            e.end.y = e.end.y * scale;

            e.radius = e.radius * scale;
          }
          EditEntity(i, e);
        }
      }
      print('Done!');
    }
  }});
  DialogAddButton({ text: "Close", position: { x: 100, y: 200 }, size: { x: 70, y: 30 }, callback: function(){
    DialogClose();
  }});

  DialogShow({ title: "Scale", size: { width: 300, height: 300 }});
}
