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
    scale = parseFloat(DialogGetTextboxValue(textbox));
    copy_op = DialogGetRadioButtonValue(copy_option);
    print("Scale Factor = " + scale + " Copy = " + copy_op);
    DialogClose();
    if (GetSelectedEntities() != undefined)
    {
      if (copy_op == false)
      {

        selected = GetSelectedEntities();
        print("Getting ready to scale " + selected.length + " selected entities!");
        for (var i = 0; i < selected.length; i++)
        {
          var e = GetEntity(selected[i]);
          print("\tFrom - " + JSON.stringify(e));
          if (e.type == "line")
          {
            print(" - Scaling Entity " + i + " which is a line by " + scale);
            e.start.x = e.start.x * scale;
            e.start.y = e.start.y * scale;

            e.end.x = e.end.x * scale;
            e.end.y = e.end.y * scale;
          }
          if (e.type == "arc")
          {
            print(" - Scaling Entity " + i + " which is a arc by " + scale);
            e.start.x = e.start.x * scale;
            e.start.y = e.start.y * scale;

            e.end.x = e.end.x * scale;
            e.end.y = e.end.y * scale;

            e.center.x = e.center.x * scale;
            e.center.y = e.center.y * scale;

            e.radius = e.radius * scale;
          }
          print("\tTo - " + JSON.stringify(e));
          EditEntity(selected[i], e);
        }
      }
      print('Done!');
      UnSelectAllEntities();
    }
  }});
  DialogAddButton({ text: "Close", position: { x: 100, y: 200 }, size: { x: 70, y: 30 }, callback: function(){
    DialogClose();
  }});

  DialogShow({ title: "Scale", size: { width: 300, height: 300 }});
}
