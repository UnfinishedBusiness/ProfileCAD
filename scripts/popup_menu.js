function Popup_LineMenu()
{
  ClearPopupMenu();
  AppendPopupMenu("Vertical Origin", "DrawLineVerticalFromOrigin();");
  AppendPopupMenu("Horizontal Origin", "DrawLineHorizontalFromOrigin();");
  AppendPopupMenu("Endpoints", "DrawLineEndpoints();");
  AppendPopupMenu("Back", "Popup_MainMenu();");
  ShowPopupMenu();
}

function Popup_MainMenu()
{
  ClearPopupMenu();
  AppendPopupMenu("Line", "Popup_LineMenu();");
  AppendPopupMenu("Repaint", "Repaint();");
}
