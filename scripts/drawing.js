function DrawLineVerticalFromOrigin()
{
  MouseCallback = function()
  {
    Live.type = "line";
    Live.start = { x: 0, y: 0 };
    Live.end = { x: 0, y: Mouse.y };
    ShowLiveEntity(Live);
  }
  MouseClickCallback = function()
  {
    DrawLine({x: 0, y: 0}, {x: 0, y: Mouse.y});
    ClearMouseCallback();
    cliReturn();
  }
  cliPrompt("Y? ", function(str){
    DrawLine({x: 0, y: 0}, {x: 0, y: eval(str)});
    ClearMouseCallback();
    HideLiveEntity();
  });
}
function DrawLineHorizontalFromOrigin()
{
  MouseCallback = function()
  {
    Live.type = "line";
    Live.start = { x: 0, y: 0 };
    Live.end = { x: Mouse.x, y: 0 };
    ShowLiveEntity(Live);
  }
  MouseClickCallback = function()
  {
    DrawLine({x: 0, y: 0}, {x: Mouse.x, y: 0});
    ClearMouseCallback();
    cliReturn();
  }
  cliPrompt("X? ", function(str){
    DrawLine({x: 0, y: 0}, {x: eval(str), y: 0});
    ClearMouseCallback();
    HideLiveEntity();
  });
}
