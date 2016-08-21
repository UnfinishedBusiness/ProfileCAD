function Read_PFCAD()
{
  if (CurrentFile == "")
  {
    //Open File Dialog
  }
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
        DrawArc(e.start, e.end, e.radius, e.direction);
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
  if (CurrentFile == "")
  {
    //Open File Dialog
  }
  if (OpenFile(CurrentFile, "w") == 0)
  {
    for (var i = 0; i < CountEntities(); i++)
    {
      var e = GetEntity(i);
      WriteFileLine(JSON.stringify(e));
    }
    CloseFile();
  }
  else
  {
      print("Cant open " + CurrentFile);
  }
}
