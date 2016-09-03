function DimensionPoint()
{
  cliPrompt("Click snap point to dimension!");
  MouseClickCallback = function()
  {
    var p = MouseGetSnap();
    if (p == "None")
    {
      cliPrompt("Must be a snap point!");
    }
    else
    {
      t = { x: p.x +1, y: p.y + 1};
      DrawPointDimension(p, t, 10);
    }
  }
}
