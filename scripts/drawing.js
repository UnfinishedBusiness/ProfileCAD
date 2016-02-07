function DrawLineVerticalFromOrigin()
{
  cliPrompt("Y? ", function(str){
    DrawLine({x: 0, y: 0}, {x: 0, y: str});
  });
}
function DrawLineHorizontalFromOrigin()
{
  cliPrompt("X? ", function(str){
    DrawLine({x: 0, y: 0}, {x: str, y: 0});
  });
}
