function ModifyTrim1()
{
  selected = GetSelectedEntities();
  if (selected != undefined && selected.length == 2) //Make sure we have selected two entities!
  {
    var first_line = GetEntity(selected[0]);
    var second_line = GetEntity(selected[1]);
    var intersection_point = geoGetLineIntersection(first_line, second_line);
    if (intersection_point != "None")
    {
        //Figure out what end of our first line is closest to the intersection point
        if (geoGetLineLength({ start: first_line.start, end: intersection_point }) < geoGetLineLength({ start: first_line.end, end: intersection_point }))
        {
          //Starting point is closest, means our starting point needs to be replaced with intersection point!
          first_line.start = intersection_point;
        }
        else
        {
          //Endpoint is closest!
          first_line.end = intersection_point;
        }
        EditEntity(selected[0], first_line);
        UnSelectAllEntities();
    }
    else
    {
        cliPrompt("Lines do not intersect!");
    }
  }
  else
  {
      cliPrompt("Select base line then line to trim too!");
  }
}
function ModifyTrim2()
{
  selected = GetSelectedEntities();
  if (selected != undefined && selected.length == 2) //Make sure we have selected two entities!
  {
    var first_line = GetEntity(selected[0]);
    var second_line = GetEntity(selected[1]);
    var intersection_point = geoGetLineIntersection(first_line, second_line);
    if (intersection_point != "None")
    {
        //Figure out what end of our first line is closest to the intersection point
        if (geoGetLineLength({ start: first_line.start, end: intersection_point }) < geoGetLineLength({ start: first_line.end, end: intersection_point }))
        {
          //Starting point is closest, means our starting point needs to be replaced with intersection point!
          first_line.start = intersection_point;
        }
        else
        {
          //Endpoint is closest!
          first_line.end = intersection_point;
        }
        EditEntity(selected[0], first_line);
        if (geoGetLineLength({ start: second_line.start, end: intersection_point }) < geoGetLineLength({ start: second_line.end, end: intersection_point }))
        {
          second_line.start = intersection_point;
        }
        else
        {
          second_line.end = intersection_point;
        }
        EditEntity(selected[1], second_line);
        UnSelectAllEntities();
    }
    else
    {
        cliPrompt("Lines do not intersect!");
    }
  }
  else
  {
      cliPrompt("Select base line then line to trim too!");
  }
}
function ModifyFillet()
{
  cliPrompt("Radius?", function(str){
    radius = Number(str);
  });
}
