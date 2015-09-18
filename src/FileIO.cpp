#include <FileIO.h>

using namespace std;

ofstream ofs;
ifstream ifs;

bool isNumeric(const std::string& input) {
    return std::all_of(input.begin(), input.end(), ::isdigit);
}
int fileOpen(string f)
{
  ifs.open(f);
  if (ifs.is_open())
  {
    if (f.find("dxf") != std::string::npos)
    {
      fileReadDXF();
    }
    else
    {
      return FILE_UNKNOWN_FORMAT;
    }
    ifs.close();
    return FILE_OK;
  }
  else
  {
    return FILE_OPEN_ERROR;
  }
}
int fileSave(string f)
{
  ofs.open(f, std::ofstream::out );
  if (ofs.is_open())
  {
    if (f.find("dxf") != std::string::npos)
    {
      fileWriteDXF();
    }
    else
    {
      return FILE_UNKNOWN_FORMAT;
    }
    ofs.close();
    return FILE_OK;
  }
  else
  {
    return FILE_OPEN_ERROR;
  }
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
        if (e.Arc.start.x == e.Arc.end.x && e.Arc.start.y == e.Arc.end.y) //Were a circle
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
          ofs << "50" << endl; //start angle
          ofs <<  std::setprecision(6) << geoRadiansToDegrees(geoGetArcStartAngle(e.Arc)) << endl;
          ofs << "51" << endl; //end angle
          ofs <<  std::setprecision(6) << geoRadiansToDegrees(geoGetArcEndAngle(e.Arc)) << endl;
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
                if (arc_start_angle < arc_end_angle)
                {
                  a.direction = ARC_CCW;
                }
                else
                {
                  a.direction = ARC_CW;
                }
                a.start = geoGetArcPoint(a, arc_start_angle);
                a.end = geoGetArcPoint(a, arc_end_angle);
                debugDumpArcStructure(a);
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
                a.center.x = 0;
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
                debugDumpArcStructure(a);
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
                a.center.x = 0;
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
