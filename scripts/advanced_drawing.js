
function AdvancedDrawingDrawGear()
{

  var yplace = 30;

  DialogClear();
  DialogAddStaticBox({ text: "Gear", position: { x: 10, y: 0 }, size: { x: 280, y: 200 }});

  DialogAddStaticText({ text: "Angle", position: { x: 20, y: yplace } });
  var angle_box = DialogAddTextBox({ default_text: "1", position: { x: 90, y: (yplace - 2) } });
  yplace += 30;

  DialogAddStaticText({ text: "Pitch", position: { x: 20, y: yplace } });
  var pitch_box = DialogAddTextBox({ default_text: "1", position: { x: 90, y: (yplace - 2) } });
  yplace += 30;

  DialogAddStaticText({ text: "Teeth", position: { x: 20, y: yplace } });
  var pitch_box = DialogAddTextBox({ default_text: "10", position: { x: 90, y: (yplace - 2) } });
  yplace += 30;

  DialogAddButton({ text: "OK", position: { x: 100, y: 100 }, size: { x: 70, y: 30 }, callback: function(){
    print("Text Box says: " + DialogGetTextboxValue(textbox));
  }});
  DialogAddButton({ text: "Close", position: { x: 100, y: 200 }, size: { x: 70, y: 30 }, callback: function(){
    DialogClose();
  }});

  DialogShow({ title: "Create Gear", size: { width: 300, height: 300 }});
}
