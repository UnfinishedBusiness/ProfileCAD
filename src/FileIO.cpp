#include <FileIO.h>

using namespace std;

ofstream ofs;
ifstream ifs;

bool isNumeric(const std::string& input) {
    return std::all_of(input.begin(), input.end(), ::isdigit);
}
int fileOpen(string f)
{
  string line;
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
    //D cout << KGREEN << line << KNORMAL << '\n';
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
        //D cout << KBLUE << ">>line = " << line << "!\n" << KNORMAL;
        line.pop_back(); //Remove newline
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
        //D cout << KBLUE << ">>arc = " << line << "!\n" << KNORMAL;
        line.pop_back(); //Remove newline
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
        //D cout << KBLUE << ">>circle = " << line << "!\n" << KNORMAL;
        line.pop_back(); //Remove newline
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
