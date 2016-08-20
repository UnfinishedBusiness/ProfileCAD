function DrawLineVerticalFromOrigin()
{
  MouseMotionCallback = function()
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
  MouseMotionCallback = function()
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
function DrawLineVerticalFromEndPoint()
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
      MouseMotionCallback = function()
      {
        Live.type = "line";
        Live.start = { x: p.x, y: p.y };
        Live.end = { x: p.x, y: Mouse.y };
        ShowLiveEntity(Live);
      }
      MouseClickCallback = function()
      {
        DrawLine(Live.start, { x: Live.start.x, y: Mouse.y });
        cliPrompt("");
        HideLiveEntity();
        ClearMouseCallback();
        UnSelectAllEntities();
        DrawLineEndpoints();
      }
    }
  }
}
function DrawLineHorizontalFromEndPoint()
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
      MouseMotionCallback = function()
      {
        Live.type = "line";
        Live.start = { x: p.x, y: p.y };
        Live.end = { x: Mouse.x, y: p.y };
        ShowLiveEntity(Live);
      }
      MouseClickCallback = function()
      {
        DrawLine(Live.start, { x: Mouse.x, y: Live.start.y });
        cliPrompt("");
        HideLiveEntity();
        ClearMouseCallback();
        UnSelectAllEntities();
        DrawLineEndpoints();
      }
    }
  }
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
      MouseMotionCallback = function()
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
function DrawLinePerpendicular()
{
  MouseClickCallback = function()
  {
    if (GetSelectedEntities() != undefined)
    {
      var selected = GetSelectedEntities();
      if (selected.length == 1)
      {
        var base_line = GetEntity(selected[0]);
        MouseMotionCallback = function()
        {
          var distance = geoGetPerpendicularDistance(base_line, Mouse);
          var line = geoGetPerpendicularLine(base_line, distance, Mouse);
          //print("Distance: " + distance);
          Live.type = "line";
          Live.start = { x: line.start.x, y: line.start.y };
          Live.end = { x: line.end.x, y: line.end.y };
          //print("Live - " + VarDump(Live));
          ShowLiveEntity(Live);
        }
        cliPrompt("Distance?", function(str){
          var distance = geoGetPerpendicularDistance(base_line, Mouse);
          var line = geoGetPerpendicularLine(base_line, Number(str), Mouse);
          DrawLine(line.start, line.end);
          cliPrompt("");
          HideLiveEntity();
          ClearMouseCallback();
          UnSelectAllEntities();
          DrawLinePerpendicular();
        });
        MouseClickCallback = function()
        {
          var p = MouseGetSnap();
          if (p == "None")
          {
            DrawLine(Live.start, Live.end);
          }
          else
          {
            var hover = MouseGetHoverOver();
            var intersection = geoGetLineIntersection(GetEntity(hover), Live);
            //print("Intersection: " + VarDump(intersection));
            if (intersection == "None" || geoGetLineLength({start: Live.start, end: intersection}) > 1000)
            {
              SetStatusText("Lines never cross!");
            }
            else
            {
              //print("Intersection point: " + VarDump(intersection));
              //print("Line - " + VarDump(Live));
              DrawLine(Live.start, intersection);
            }
          }
          cliPrompt("");
          HideLiveEntity();
          ClearMouseCallback();
          UnSelectAllEntities();
          DrawLinePerpendicular();
        }
        //geoGetParallelLine(base_line, 0.5, Mouse);
      }
      else
      {
          SetStatusText("Only select on base line!");
      }
    }
    else
    {
      SetStatusText("Select Base Line!");
    }
  }
  if (GetSelectedEntities() != undefined) //We already have our line selected!
  {
    MouseClickCallback();
  }
  else
  {
    SetStatusText("Select Base Line");
  }
}
function DrawLineParallel()
{
  MouseClickCallback = function()
  {
    if (GetSelectedEntities() != undefined)
    {
      var selected = GetSelectedEntities();
      if (selected.length == 1)
      {
        var base_line = GetEntity(selected[0]);
        MouseMotionCallback = function()
        {
          var distance = geoGetPerpendicularDistance(base_line, Mouse);
          var line = geoGetParallelLine(base_line, distance, Mouse);
          //print("Distance: " + distance);
          Live.type = "line";
          Live.start = { x: line.start.x, y: line.start.y };
          Live.end = { x: line.end.x, y: line.end.y };
          //print("Live - " + VarDump(Live));
          ShowLiveEntity(Live);
        }
        cliPrompt("Distance?", function(str){
          var distance = geoGetPerpendicularDistance(base_line, Mouse);
          var line = geoGetParallelLine(base_line, Number(str), Mouse);
          DrawLine(line.start, line.end);
          cliPrompt("");
          HideLiveEntity();
          ClearMouseCallback();
          UnSelectAllEntities();
          DrawLineParallel();
        });
        MouseClickCallback = function()
        {
          DrawLine(Live.start, Live.end);
          cliPrompt("");
          HideLiveEntity();
          ClearMouseCallback();
          UnSelectAllEntities();
          DrawLineParallel();
        }
        //geoGetParallelLine(base_line, 0.5, Mouse);
      }
      else
      {
          SetStatusText("Only select on base line!");
      }
    }
    else
    {
      SetStatusText("Select Base Line!");
    }
  }
  if (GetSelectedEntities() != undefined) //We already have our line selected!
  {
    MouseClickCallback();
  }
  else
  {
    SetStatusText("Select Base Line");
  }
}
