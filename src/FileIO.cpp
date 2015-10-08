#include <FileIO.h>

using namespace std;

ofstream ofs;
ifstream ifs;
gzFile gz;

bool isNumeric(const std::string& input) {
    return std::all_of(input.begin(), input.end(), ::isdigit);
}
int fileOpen(string f)
{
  if (f.find(".dxf") != std::string::npos)
  {
    ifs.open(f);
    fileReadDXF();
    ifs.close();
  }
  else if (f.find(".nc") != std::string::npos)
  {
    ifs.open(f);
    fileReadNC();
    ifs.close();
  }
  else if (f.find(".pfcad") != std::string::npos)
  {
    //ifs.open(f, std::ios::binary | std::ios::in );
    gz = gzopen(f.c_str(), "rb");
    fileReadPFCAD();
    gzclose(gz);
    //ifs.close();
  }
  else
  {
    return FILE_UNKNOWN_FORMAT;
  }
  cadUndoPushState();
  return FILE_OK;
}
int fileSave(string f)
{
  if (f.find("dxf") != std::string::npos)
  {
    ofs.open(f, std::ofstream::out );
    fileWriteDXF();
    ofs.close();
  }
  else if (f.find(".pfcad") != std::string::npos)
  {
    //ofs.open(f, std::ios::binary | std::ios::out );
    gz = gzopen(f.c_str(), "wb");
    fileWritePFCAD();
    gzclose(gz);
    //ofs.close();
  }
  else
  {
    return FILE_UNKNOWN_FORMAT;
  }
  return FILE_OK;
}
void fileWritePFCAD()
{
  if(!gz) {
    cout << "Cannot write file!\n";
    return;
  }
  Header_t Header;
  Header.NumberOfEntitys = cadGetEntityArrayIndex();
  Header.NumberOfToolpaths = cadGetToolpaths().size();
  gzwrite(gz, (char *) &Header, sizeof(struct Header_t));

  cadEntity e;
  for (int x = 0; x < Header.NumberOfEntitys; x++)
  {
    e = cadGetEntityArray(x);
    if (!e.Removed)
    {
      gzwrite(gz, (char *) &e, sizeof(struct cadEntity));
    }
  }
  cadToolpath_pod_t pod;
  vector<cadToolpath> t = cadGetToolpaths();
  for (int x = 0; x < Header.NumberOfToolpaths; x++)
  {
    V cout << "Writing toolpath " << x << endl;
    pod.Cycle = t[x].Cycle;
    pod.ToolNumber = t[x].ToolNumber;
    pod.ToolDiameter = t[x].ToolDiameter;
    pod.SpindleSpeed = t[x].SpindleSpeed;
    pod.Coolant = t[x].Coolant;
    pod.Side = t[x].Side;
    pod.ContourCycle.feed = t[x].ContourCycle.feed;
    pod.ContourCycle.plunge_feed = t[x].ContourCycle.plunge_feed;
    pod.ContourCycle.retract_feed = t[x].ContourCycle.retract_feed;
    pod.PathIsClosed = t[x].Path.isClosed;
    pod.NumberOfPathEntitys = t[x].Path.Entitys.size();
    gzwrite(gz, (char *) &pod, sizeof(struct cadToolpath_pod_t));
    for (int i = 0; i < pod.NumberOfPathEntitys; i++)
    {
      e = t[x].Path.Entitys[i];
      gzwrite(gz, (char *) &e, sizeof(struct cadEntity));
    }
  }

}
void fileReadPFCAD()
{
  if(!gz) {
    cout << "Cannot read file!\n";
    return;
  }
  cadEntity e;

  Header_t Header;
  gzread(gz, (char *) &Header, sizeof(struct Header_t));
  //cout << "Header.NumberOfEntitys => " << Header.NumberOfEntitys << endl;
  for (int x = 0; x < Header.NumberOfEntitys; x++)
  {
    gzread(gz, (char *) &e, sizeof(struct cadEntity));
    cadAppend(e, false);
  }
  cadToolpath_pod_t pod;
  cadToolpath t;
  for (int x = 0; x < Header.NumberOfToolpaths; x++)
  {
    gzread(gz, (char *) &pod, sizeof(struct cadToolpath_pod_t));
    t.Cycle = pod.Cycle;
    t.ToolNumber = pod.ToolNumber;
    t.ToolDiameter = pod.ToolDiameter;
    t.SpindleSpeed = pod.SpindleSpeed;
    t.Coolant = pod.Coolant;
    t.Side = pod.Side;
    t.ContourCycle.feed = pod.ContourCycle.feed;
    t.ContourCycle.plunge_feed = pod.ContourCycle.plunge_feed;
    t.ContourCycle.retract_feed = pod.ContourCycle.retract_feed;
    t.Path.isClosed = pod.PathIsClosed;
    for (int i = 0; i < pod.NumberOfPathEntitys; i++)
    {
      gzread(gz, (char *) &e, sizeof(struct cadEntity));
      t.Path.Entitys.push_back(e);
    }
    cadAppendToolpath(t);
  }
  /*while (gzread(gz, (char *) &e, sizeof(struct cadEntity)))
  {
       cadAppend(e, false);
  }*/

}
void fileWriteDXF()
{
  auto WriteColorCode = [](cadEntity e)
  {
    if (e.Color == BLACK) ofs << "0" << endl;
    if (e.Color == RED) ofs << "1" << endl;
    if (e.Color == YELLOW) ofs << "2" << endl;
    if (e.Color == GREEN) ofs << "3" << endl;
    if (e.Color == CYAN) ofs << "4" << endl;
    if (e.Color == BLUE) ofs << "5" << endl;
    if (e.Color == MAGENTA) ofs << "6" << endl;
    if (e.Color == DARKGREY) ofs << "7" << endl;
    if (e.Color == LIGHTGREY) ofs << "8" << endl;
  };
  ofs << "0" << endl << "SECTION" <<endl << "2" << endl << "ENTITIES" << endl << "0" << endl;

  cadEntity e;
  for (int x = 0; x < cadGetEntityArrayIndex(); x++)
  {
    e = cadGetEntityArray(x);
    if (!e.Removed)
    {
      if (e.Type == CAD_ARC)
      {
        if (e.Arc.start == e.Arc.end) //Were a circle
        {
          ofs << "CIRCLE" << endl;
          ofs << "8" << endl << "1" << endl; //Group Code 8 - Fixed to layer one
          ofs << "62" << endl; //Group Code - Entity Color
          WriteColorCode(e);
          ofs << "10" << endl; //X center
          ofs <<  std::setprecision(6) << e.Arc.center.x << endl;
          ofs << "20" << endl; //Y center
          ofs <<  std::setprecision(6) << e.Arc.center.y << endl;
          ofs << "30" << endl; //Z center
          ofs <<  std::setprecision(6) << e.Arc.center.z << endl;
          ofs << "40" << endl; //Radius
          ofs <<  std::setprecision(6) << e.Arc.radius << endl;
          ofs << "0" << endl; //End entity
        }
        else
        {
          //ofs << "#" << x << endl;
          ofs << "ARC" << endl;
          ofs << "8" << endl << "1" << endl; //Group Code 8 - Fixed to layer one
          ofs << "62" << endl; //Group Code - Entity Color
          WriteColorCode(e);
          ofs << "10" << endl; //X center
          ofs <<  std::setprecision(6) << e.Arc.center.x << endl;
          ofs << "20" << endl; //Y center
          ofs <<  std::setprecision(6) << e.Arc.center.y << endl;
          ofs << "30" << endl; //Z center
          ofs <<  std::setprecision(6) << e.Arc.center.z << endl;
          ofs << "40" << endl; //Radius
          ofs <<  std::setprecision(6) << e.Arc.radius << endl;
          float start_angle = geoRadiansToDegrees(geoGetArcStartAngle(e.Arc));
          float end_angle = geoRadiansToDegrees(geoGetArcEndAngle(e.Arc));
          ofs << "50" << endl; //start angle
          ofs <<  std::setprecision(6) << start_angle << endl;
          ofs << "51" << endl; //end angle
          ofs <<  std::setprecision(6) << end_angle << endl;
          ofs << "0" << endl; //End entity

        }
      }
      if (e.Type == CAD_LINE)
      {
        ofs << "LINE\n";
        ofs << "8" << endl << "1" << endl; //Group Code 8 - Fixed to layer one
        ofs << "62" << endl; //Group Code - Entity Color
        WriteColorCode(e);
        ofs << "10" << endl; //X start
        ofs <<  std::setprecision(6) << e.Line.start.x << endl;
        ofs << "20" << endl; //Y start
        ofs <<  std::setprecision(6) << e.Line.start.y << endl;
        ofs << "30" << endl; //Z start
        ofs <<  std::setprecision(6) << e.Line.start.z << endl;
        ofs << "11" << endl; //X end
        ofs <<  std::setprecision(6) << e.Line.end.x << endl;
        ofs << "21" << endl; //Y end
        ofs <<  std::setprecision(6) << e.Line.end.y << endl;
        ofs << "31" << endl; //Z end
        ofs <<  std::setprecision(6) << e.Line.end.z << endl;
        ofs << "0" << endl; //End entity
      }

    }
  }
  ofs << "ENDSEC\n0\nEOF\n";
}
void fileReadDXF()
{
  string line;
  //Moadal states
  bool InsideSection = false;
  string CurrentSection = "";
  int Group = 0;
  color_t CurrentColor = GREEN;
  int entity_count = 0;
  string CurrentEntity = "";
  line_t l;
  arc_t a;
  float arc_start_angle;
  float arc_end_angle;
  int cn;
  while (getline (ifs, line))
  {
    line.erase(std::remove(line.begin(), line.end(), ' '), line.end()); //Remove all whitespaces
    line.erase(std::remove(line.begin(), line.end(), '\n'), line.end()); //Remove all newline characters
    line.erase(std::remove(line.begin(), line.end(), '\r'), line.end()); //Remove all carage return characters
    //D cout << KGREEN << line << KNORMAL << '\n';
    //printf("line = \"%s\"\n", line.c_str() );
    if (line.find("ENTITIES") != std::string::npos)
    {
      InsideSection = true;
      CurrentSection = "ENTITIES";
      //D cout << "Found Enties Section!\n";
    }
    if (line.find("ENDSEC") != std::string::npos)
    {
      InsideSection = false;
      CurrentSection = "";
      //D cout << "Found End Enties Section!\n";
    }
    if (line.find("LINE") != std::string::npos)
    {
      CurrentEntity = "line";
      //D cout << "\tFound Line Entity!\n";
    }
    if (line.find("ARC") != std::string::npos)
    {
      CurrentEntity = "arc";
      //D cout << "\tFound Arc Entity!\n";
    }
    if (line.find("CIRCLE") != std::string::npos)
    {
      CurrentEntity = "circle";
      //D cout << "\tFound Circle Entity!\n";
    }
    if (InsideSection && CurrentSection == "ENTITIES" && CurrentEntity == "line")
    {
        if (isNumeric(line))
        {
          Group = atoi((char *)line.c_str());
          switch(Group)
          {
            case 0: //End of entity
                //D cout << KRED << "\tDrawing Entity!\n" << KNORMAL;
                cadSetColor(CurrentColor);
                cadDrawLine(l);
                cadRedraw();
                l.start.x = 0;
                l.start.y = 0;
                l.start.z = 0;
                l.end.x = 0;
                l.end.y = 0;
                l.end.z = 0;
                CurrentEntity = "";
              break;
            case 62: //Color
              //D cout << "\tSetting color!\n";
              getline (ifs, line);
              cn = atof((char *)line.c_str());
              if (cn == 0) CurrentColor = BLACK;
              if (cn == 1) CurrentColor = RED;
              if (cn == 2) CurrentColor = YELLOW;
              if (cn == 3) CurrentColor = GREEN;
              if (cn == 4) CurrentColor = CYAN;
              if (cn == 5) CurrentColor = BLUE;
              if (cn == 6) CurrentColor = MAGENTA;
              if (cn == 7) CurrentColor = DARKGREY;
              if (cn == 8) CurrentColor = LIGHTGREY;
              break;
            case 10: //Primary X coordinate
              //D cout << "\tFound start X coordinate!\n";
              getline (ifs, line);
              l.start.x = atof((char *)line.c_str());
              break;
            case 20: //Primary Y coordinat
              //D cout << "\tFound start Y coordinate!\n";
              getline (ifs, line);
              l.start.y = atof((char *)line.c_str());
              break;
            case 30: //Primary Z coordinat
                //D cout << "\tFound start Z coordinate!\n";
                getline (ifs, line);
                l.start.z = atof((char *)line.c_str());
                break;
            case 11: //Secondary X coordinat (11-18)
                //D cout << "\tFound end X coordinate!\n";
                getline (ifs, line);
                l.end.x = atof((char *)line.c_str());
                break;
            case 21: //Secondary Y coordinat (21-28)
                //D cout << "\tFound end Y coordinate!\n";
                getline (ifs, line);
                l.end.y = atof((char *)line.c_str());
                break;
            case 31: //Secondary Z coordinat (31-37)
              //D cout << "\tFound end Z coordinate!\n";
                getline (ifs, line);
                l.end.z = atof((char *)line.c_str());
                break;
          }
        }
    }
    if (InsideSection && CurrentSection == "ENTITIES" && CurrentEntity == "arc")
    {
        if (isNumeric(line))
        {
          Group = atoi((char *)line.c_str());
          switch(Group)
          {
            case 0: //End of entity
                //D cout << KRED << "\tDrawing Entity!\n" << KNORMAL;
                a.direction = geoGetArcDirection(arc_start_angle, arc_end_angle);
                a.start = geoGetArcPoint(a, arc_start_angle);
                a.end = geoGetArcPoint(a, arc_end_angle);
                //debugDumpArcStructure(a);
                cadSetColor(CurrentColor);
                cadDrawArc(a);
                cadRedraw();
                arc_start_angle = 0;
                arc_end_angle = 0;
                a.start.x = 0;
                a.start.y = 0;
                a.start.z = 0;
                a.end.x = 0;
                a.end.y = 0;
                a.end.z = 0;
                a.center.x = 0;
                a.center.y = 0;
                a.center.z = 0;
                CurrentEntity = "";
              break;
            case 62: //Color
              //D cout << "\tSetting color!\n";
              getline (ifs, line);
              cn = atof((char *)line.c_str());
              if (cn == 0) CurrentColor = BLACK;
              if (cn == 1) CurrentColor = RED;
              if (cn == 2) CurrentColor = YELLOW;
              if (cn == 3) CurrentColor = GREEN;
              if (cn == 4) CurrentColor = CYAN;
              if (cn == 5) CurrentColor = BLUE;
              if (cn == 6) CurrentColor = MAGENTA;
              if (cn == 7) CurrentColor = DARKGREY;
              if (cn == 8) CurrentColor = LIGHTGREY;
              break;
            case 10: //Primary X coordinate
              //D cout << "\tFound center X coordinate!\n";
              getline (ifs, line);
              a.center.x = atof((char *)line.c_str());
              break;
            case 20: //Primary Y coordinat
              //D cout << "\tFound center Y coordinate!\n";
              getline (ifs, line);
              a.center.y = atof((char *)line.c_str());
              break;
            case 30: //Primary Z coordinat
                //D cout << "\tFound center Z coordinate!\n";
                getline (ifs, line);
                a.center.z = atof((char *)line.c_str());
                break;
            case 40: //Arc radius
                //D cout << "\tFound arc radius!\n";
                getline (ifs, line);
                a.radius = atof((char *)line.c_str());
                break;
            case 50: //Start angle
                //D cout << "\tFound start angle!\n";
                getline (ifs, line);
                arc_start_angle = atof((char *)line.c_str());
                break;
            case 51: //End angle
                //D cout << "\tFound end angle!\n";
                getline (ifs, line);
                arc_end_angle = atof((char *)line.c_str());
                break;
          }
        }
    }
    if (InsideSection && CurrentSection == "ENTITIES" && CurrentEntity == "circle")
    {
        if (isNumeric(line))
        {
          Group = atoi((char *)line.c_str());
          switch(Group)
          {
            case 0: //End of entity
                //D cout << KRED << "\tDrawing Entity!\n" << KNORMAL;
                a.direction = ARC_CW;
                a.start = point_t{a.center.x + a.radius, a.center.y};
                a.end = point_t{a.center.x + a.radius, a.center.y};
                //D printf("Start: %.6f, %.6f\nEnd: %.6f, %.6f\n", a.start.x, a.start.y, a.end.x, a.end.y);
                //debugDumpArcStructure(a);
                cadSetColor(CurrentColor);
                cadDrawArc(a);
                cadRedraw();
                arc_start_angle = 0;
                arc_end_angle = 0;
                a.start.x = 0;
                a.start.y = 0;
                a.start.z = 0;
                a.end.x = 0;
                a.end.y = 0;
                a.end.z = 0;
                a.center.x = 0;
                a.center.y = 0;
                a.center.z = 0;
                CurrentEntity = "";
              break;
            case 62: //Color
              //D cout << "\tSetting color!\n";
              getline (ifs, line);
              cn = atof((char *)line.c_str());
              if (cn == 0) CurrentColor = BLACK;
              if (cn == 1) CurrentColor = RED;
              if (cn == 2) CurrentColor = YELLOW;
              if (cn == 3) CurrentColor = GREEN;
              if (cn == 4) CurrentColor = CYAN;
              if (cn == 5) CurrentColor = BLUE;
              if (cn == 6) CurrentColor = MAGENTA;
              if (cn == 7) CurrentColor = DARKGREY;
              if (cn == 8) CurrentColor = LIGHTGREY;
              break;
            case 10: //Primary X coordinate
              //D cout << "\tFound center X coordinate!\n";
              getline (ifs, line);
              a.center.x = atof((char *)line.c_str());
              break;
            case 20: //Primary Y coordinate
              //D cout << "\tFound center Y coordinate!\n";
              getline (ifs, line);
              a.center.y = atof((char *)line.c_str());
              break;
            case 30: //Primary Z coordinate
              //D cout << "\tFound center Z coordinate!\n";
              getline (ifs, line);
              a.center.z = atof((char *)line.c_str());
              break;
            case 40: //Circle Radius
              getline (ifs, line);
              a.radius = atof((char *)line.c_str());
              break;
          }
        }
    }
  }
}
void fileReadNC()
{
  string line;
  string Word = "";
  int Gword = -1;
  string Value;
  line_t l;
  bool PostLine = false;
  point_t CurrentPosition;
  while (getline (ifs, line))
  {
    line.erase(std::remove(line.begin(), line.end(), ' '), line.end()); //Remove all whitespaces
    line.erase(std::remove(line.begin(), line.end(), '\n'), line.end()); //Remove all newline characters
    line.erase(std::remove(line.begin(), line.end(), '\r'), line.end()); //Remove all carage return characters
    V cout << KRED << "(fileReadNC) ==> " << KGREEN << line << KNORMAL << endl;
    for (int x = 0; x < line.length(); x++)
    {
      if (Word != "")
      {
        while(x < line.length())
        {
          if (isdigit(line[x]) || line[x] == '.' || line[x] == '-')
          {
            Value.push_back(line[x]);
          }
          else
          {
            break;
          }
          x++;
        }
        V cout << KBLUE << "\t\t Word ==> " << KGREEN << Word << KNORMAL << endl;
        V cout << KBLUE << "\t\t Value ==> " << KGREEN << Value << KNORMAL << endl;
        if (Word == "X")
        {
          CurrentPosition.x = atof((char *)Value.c_str());
          V cout << KBLUE << "\t\t\t Found X ==> " << KGREEN << CurrentPosition.x << KNORMAL << endl;
        }
        if (Word == "Y")
        {
          CurrentPosition.y = atof((char *)Value.c_str());
          V cout << KBLUE << "\t\t\t Found Y ==> " << KGREEN << CurrentPosition.y << KNORMAL << endl;
        }
        if (Word == "Z")
        {
          CurrentPosition.z = atof((char *)Value.c_str());
          V cout << KBLUE << "\t\t\t Found Z ==> " << KGREEN << CurrentPosition.z << KNORMAL << endl;
        }
        Word = "";
        Value.erase();
      }
      if (!isdigit(line[x]) && line[x] != '.' && line[x] != '-')
      {
        Word = line[x];
        if (Word == "G")
        {
          V cout << KBLUE << "\t\t\t Found Gword! ==> " << KGREEN << Word << KRED << " With value ==> " <<  Value << KNORMAL << endl;
          if (Value == "00" || Value == "0") //Rapid
          {
            V cout << KMAGENTA << "\t\t\t\t Gword is Rapid!" << KNORMAL << endl;
            Gword = 0;
          }
          else if (Value == "01" || Value == "1") //Line
          {
            V cout << KMAGENTA << "\t\t\t\t Gword is Line!" << KNORMAL << endl;
            Gword = 1;
          }
          else
          {
            Gword = -1;
          }
        }
      }
    }
    if (Gword == 0)
    {
      cadSetColor(RED);
    }
    if (Gword == 1)
    {
      cadSetColor(GREEN);
    }

    if (PostLine)
    {
      V cout << KRED << "(fileReadNC) Posting Line" << KNORMAL << endl;
      l.end = CurrentPosition;
      cadDrawLine(l);
    }
    else
    {
      l.start = CurrentPosition;
    }
    PostLine = !PostLine;
  }
}
