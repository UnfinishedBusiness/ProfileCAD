function DrawLine(start, end)
{
  NativeDrawLine2D(start.x,start.y,end.x,end.y);
}

function main()
{
  DrawLine({x: 0, y: 0}, {x: -4, y: 0});
}
