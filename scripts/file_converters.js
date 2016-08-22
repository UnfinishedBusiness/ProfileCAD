function Read_PFCAD()
{
  if (OpenFile(CurrentFile, "r") == 0)
  {
    while(1)
    {
      var line = ReadFileLine();
      if (line == "NULL")
      {
        CloseFile();
        print("Found end of file!");
        break;
      }
      var e = JSON.parse(line);
      SetDrawColor(e.color);
      if (e.type == "line")
      {
        DrawLine(e.start, e.end);
      }
      if (e.type == "arc")
      {
        DrawArc(e.start, e.end, e.center, e.radius, e.direction);
      }
    }
    SetDrawColor("green");
  }
  else
  {
    print("Cant open " + CurrentFile);
  }
}
function Write_PFCAD()
{
  print("Write_PFCAD()");
  if (OpenFile(CurrentFile, "w") == 0)
  {
    print("File Open!");
    for (var i = 0; i < CountEntities(); i++)
    {
      var e = GetEntity(i);
      print("Line(" + i + ")> " + JSON.stringify(e));
      WriteFileLine(JSON.stringify(e));
    }
    CloseFile();
  }
  else
  {
      print("Cant open " + CurrentFile);
  }
}

function DXF_Color_Map(color)
{
  var c = "green";
  if (color == 0) c = "black";
  if (color == 1) c = "red";
  if (color == 2) c = "yellow";
  if (color == 3) c = "green";
  if (color == 4) c = "cyan";
  if (color == 5) c = "blue";
  if (color == 6) c = "magenta";
  if (color == 7) c = "darkgrey";
  if (color == 8) c = "lightgrey";
  return c;
}
function Read_DXF()
{
  if (OpenFile(CurrentFile, "r") == 0)
  {
    //print("Opened file!");
    e = {};
    e.start = {};
    e.end = {};
    e.center = {};
    CurrentEntity = "";
    while(1)
    {
      var line = ReadFileLine();
      //print("Line: " + line);
      if (line == "NULL")
      {
        CloseFile();
        //print("Found end of file!");
        break;
      }
      if (line.indexOf("LINE") >= 0)
      {
        while(1)
        {
          var line = ReadFileLine();
          //print("LINE->Line: " + line);
          if (line == "NULL")
          {
            CloseFile();
            //print("Found end of file!");
            break;
          }
          //print("\tGroup Number: " + line);
          var GroupNumber = parseInt(line);
          if (GroupNumber == 0) //End of Entity
          {
            SetDrawColor(e.color);
            DrawLine(e.start, e.end);
            //print("Drawing line: " + JSON.stringify(e));
            break;
          }
          if (GroupNumber == 62) //Color
          {
            var color = DXF_Color_Map(parseInt(ReadFileLine()));
            //print("\tColor = " + color);
          }
          if (GroupNumber == 10) //Start X
          {
            e.start.x = parseFloat(ReadFileLine());
          }
          if (GroupNumber == 20) //Start Y
          {
            e.start.y = parseFloat(ReadFileLine());
          }
          if (GroupNumber == 30) //Start Z
          {
            e.start.z = parseFloat(ReadFileLine());
          }
          if (GroupNumber >= 11 && GroupNumber <= 18) //End X
          {
            e.end.x = parseFloat(ReadFileLine());
          }
          if (GroupNumber >= 21 && GroupNumber <= 28) //End Y
          {
            e.end.y = parseFloat(ReadFileLine());
          }
          if (GroupNumber >= 31 && GroupNumber <= 37) //End Z
          {
            e.end.z = parseFloat(ReadFileLine());
          }
        }
      }
      if (line.indexOf("ARC") >= 0)
      {
        while(1)
        {
          var line = ReadFileLine();
          //print("ARC->Line: " + line);
          if (line == "NULL")
          {
            CloseFile();
            //print("Found end of file!");
            break;
          }
          //print("\tGroup Number: " + line);
          var GroupNumber = parseInt(line);
          if (GroupNumber == 0) //End of Entity
          {
            if (e.start_angle < e.end_angle)
            {
              //print("\tArc is Counter-Clock-wise!");
              e.direction = "ccw";
            }
            else
            {
              //print("\tArc is Clock-Wise!");
              e.direction = "cw";
            }
            e.start = GetArcPointFromAngle(e.center.x, e.center.y, e.radius, e.direction, e.start_angle);
            e.end = GetArcPointFromAngle(e.center.x, e.center.y, e.radius, e.direction, e.end_angle);
            SetDrawColor(e.color);
            DrawArc(e.start, e.end, e.center, e.radius, e.direction);
            print("Drawing Arc: " + JSON.stringify(e));
            break;
          }
          if (GroupNumber == 62) //Color
          {
            var color = DXF_Color_Map(parseInt(ReadFileLine()));
            //print("\tColor = " + color);
          }
          if (GroupNumber == 10) //Center X
          {
            e.center.x = parseFloat(ReadFileLine());
          }
          if (GroupNumber == 20) //Center Y
          {
            e.center.y = parseFloat(ReadFileLine());
          }
          if (GroupNumber == 30) //Center Z
          {
            e.center.z = parseFloat(ReadFileLine());
          }
          if (GroupNumber == 40) //Radius
          {
            e.radius = parseFloat(ReadFileLine());
          }
          if (GroupNumber == 50) //Start angle
          {
            e.start_angle = parseFloat(ReadFileLine());
          }
          if (GroupNumber == 51) //End angle
          {
            e.end_angle = parseFloat(ReadFileLine());
          }

        }
      }
      if (line.indexOf("CIRCLE") >= 0)
      {
        while(1)
        {
          var line = ReadFileLine();
          //print("CIRCLE->Line: " + line);
          if (line == "NULL")
          {
            CloseFile();
            print("Found end of file!");
            break;
          }
          //print("\tGroup Number: " + line);
          var GroupNumber = parseInt(line);
          if (GroupNumber == 0) //End of Entity
          {
            e.direction = "cw";
            e.start.x = e.center.x + e.radius;
            e.start.y = e.center.y;

            e.end.x = e.center.x + e.radius;
            e.end.y = e.center.y;

            SetDrawColor(e.color);
            DrawArc(e.start, e.end, e.center, e.radius, e.direction);
            print("Drawing Circle: " + JSON.stringify(e));
            break;
          }
          if (GroupNumber == 62) //Color
          {
            var color = DXF_Color_Map(parseInt(ReadFileLine()));
            //print("\tColor = " + color);
          }
          if (GroupNumber == 10) //Center X
          {
            e.center.x = parseFloat(ReadFileLine());
          }
          if (GroupNumber == 20) //Center Y
          {
            e.center.y = parseFloat(ReadFileLine());
          }
          if (GroupNumber == 30) //Center Z
          {
            e.center.z = parseFloat(ReadFileLine());
          }
          if (GroupNumber == 40) //Radius
          {
            e.radius = parseFloat(ReadFileLine());
          }
        }
      }

    }
    SetDrawColor("green");
  }
  else
  {
    print("Cant open " + CurrentFile);
  }
}
