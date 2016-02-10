function Popup_LineMenu()
{
  ClearPopupMenu();
  AppendPopupMenu("Vertical Origin", "DrawLineVerticalFromOrigin();");
  AppendPopupMenu("Horizontal Origin", "DrawLineHorizontalFromOrigin();");
  AppendPopupMenu("Endpoints", "DrawLineEndpoints();");
  AppendPopupMenu("Perpendicular", "DrawLinePerpendicular();");
  AppendPopupMenu("Parallel", "DrawLineParallel();");
  AppendPopupMenu("Back", "Popup_MainMenu();");
  ShowPopupMenu();
}

function Popup_MainMenu()
{
  ClearPopupMenu();
  AppendPopupMenu("Line", "Popup_LineMenu();");
  AppendPopupMenu("Repaint", "Repaint();");
}
