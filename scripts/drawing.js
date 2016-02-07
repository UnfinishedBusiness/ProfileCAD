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
function DrawLineEndpoints()
{
  cliPrompt("Click first endpoint");
  MouseClickCallback = function()
  {
    var p = MouseGetSnap();
    if (p == "None")
    {
      cliPrompt("Must be a snap point!");
    }
    else
    {
      cliPrompt("Click second endpoint");
      ClearMouseCallback();
      MouseCallback = function()
      {
        Live.type = "line";
        Live.start = { x: p.x, y: p.y };
        Live.end = { x: Mouse.x, y: Mouse.y };
        ShowLiveEntity(Live);
      }
      MouseClickCallback = function()
      {
        var p = MouseGetSnap();
        if (p == "None")
        {
          cliPrompt("Must be a snap point!");
        }
        else
        {
          DrawLine(Live.start, { x: p.x, y: p.y });
          cliPrompt("");
          HideLiveEntity();
          ClearMouseCallback();
          UnSelectAllEntities();
          DrawLineEndpoints();
        }

      }
    }
  }
}
