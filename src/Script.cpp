#include <Script.h>

using namespace std;

Dialog *script_dialog;
duk_context *ctx;
string color;
FILE *FilePointer;

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}
std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

std::string GetColorString(color_t c)
{
  if (c == BLACK) return "black";
  if (c == RED) return "red";
  if (c == YELLOW) return "yellow";
  if (c == GREEN) return "green";
  if (c == CYAN) return "cyan";
  if (c == BLUE) return "blue";
  if (c == PURPLE) return "purple";
  if (c == MAGENTA) return "magenta";
  if (c == DARKGREY) return "darkgrey";
  if (c == LIGHTGREY) return "lightgrey";
  return "green"; //Defaylt Color
}
color_t GetColorStructure(string c)
{
  if (c == "black") return BLACK;
  if (c == "red") return RED;
  if (c == "yellow") return YELLOW;
  if (c == "green") return GREEN;
  if (c == "cyan") return CYAN;
  if (c == "blue") return BLUE;
  if (c == "purple") return PURPLE;
  if (c == "magenta") return MAGENTA;
  if (c == "darkgrey") return DARKGREY;
  if (c == "lightgrey") return LIGHTGREY;
  return GREEN; //Default Color
}

/***** Javascript functions **************/
int OpenFile(duk_context *ctx)
{
	int n = duk_get_top(ctx);  /* #args */
  string file = duk_to_string(ctx, 0);
  string rw = duk_to_string(ctx, 1);
  //fileOpen(file);
  int ret = 1; //Return 0 on successful open, 1 on unsucessful
  if (rw == "w")
  {
    FilePointer = fopen (file.c_str(), "w");
  }
  else
  {
    FilePointer = fopen (file.c_str(), "r");
  }

  if (FilePointer != NULL)
  {
    ret = 0;
  }
  PostRedisplay();
	duk_push_number(ctx, ret);
	return 1;  /* one return value */
}
int WriteFileLine(duk_context *ctx)
{
	int n = duk_get_top(ctx);  /* #args */
  string line = duk_to_string(ctx, 0);
  if (FilePointer != NULL)
  {
    fprintf(FilePointer, "%s\n", line.c_str());
  }
  //PostRedisplay();
	duk_push_number(ctx, 0);
	return 1;  /* one return value */
}
int ReadFileLine(duk_context *ctx)
{
	int n = duk_get_top(ctx);  /* #args */
  int ret = 1; //Return 1 if we have reached end of file
  char line[2048];
  string line_string;
  if (FilePointer != NULL)
  {
    if (fgets(line, sizeof(line), FilePointer) == NULL)
    {
      ret = 0;
    }
  }
  if (ret == 0)
  {
    line_string = "NULL";
  }
  else
  {
    line[strlen(line) - 1] = '\0';
    line_string = string(line);
  }
  //PostRedisplay();
	duk_push_string(ctx, line_string.c_str());
	return 1;  /* one return value */
}
int CloseFile(duk_context *ctx)
{
  int n = duk_get_top(ctx);  /* #args */
  fclose(FilePointer);
  PostRedisplay();
	duk_push_number(ctx, 0);
	return 1;  /* one return value */
}
int FileOpenDialog(duk_context *ctx)
{
  int n = duk_get_top(ctx);  /* #args */
  wxFileDialog* OpenDialog = new wxFileDialog( MainWindow, _("Choose a file to open"), wxEmptyString, wxEmptyString, _("Profile CAD (*.pfcad)|*.pfcad;*.PFCAD|Autocad (*.dxf)|*.dxf;*.DXF"),wxFD_OPEN, wxDefaultPosition);

	// Creates a "open file" dialog with 4 file types
	if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "Cancel"
	{
		wxString CurrentDocPath = OpenDialog->GetPath();
    scriptEval(string("OnFileOpenDialog(\"" + CurrentDocPath + "\")"));
	}
	// Clean up after ourselves
	OpenDialog->Destroy();
  PostRedisplay();
	duk_push_number(ctx, 0);
	return 1;  /* one return value */
}
int FileSaveDialog(duk_context *ctx)
{
  int n = duk_get_top(ctx);  /* #args */
  wxFileDialog* SaveDialog = new wxFileDialog( MainWindow, _("Choose save location"), wxEmptyString, wxEmptyString, _("Profile CAD (*.pfcad)|*.pfcad;*.PFCAD|Autocad (*.dxf)|*.dxf;*.DXF"),wxFD_SAVE, wxDefaultPosition);

	// Creates a "open file" dialog with 4 file types
	if (SaveDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "Cancel"
	{
		wxString CurrentDocPath = SaveDialog->GetPath();
		scriptEval(string("OnFileSaveDialog(\"" + CurrentDocPath + "\")"));
	}
	// Clean up after ourselves
	SaveDialog->Destroy();
  PostRedisplay();
	duk_push_number(ctx, 0);
	return 1;  /* one return value */
}
int adder(duk_context *ctx)
{
	int i;
	int n = duk_get_top(ctx);  /* #args */
	double res = 0.0;

	for (i = 0; i < n; i++) {
		res += duk_to_number(ctx, i);
	}

	duk_push_number(ctx, res);
	return 1;  /* one return value */
}
int SetDrawColor(duk_context *ctx)
{
	duk_get_top(ctx);  /* #args */
  color = duk_to_string(ctx, 0);
	duk_push_number(ctx, 0);
	return 1;  /* one return value */
}
int DrawPointDimension(duk_context *ctx)
{
  dimension_t dimension;
  dimension_point_t point;
	int i;
	int n = duk_get_top(ctx);  /* #args */

  point.snap_pos.x = duk_to_number(ctx, 0);
  point.snap_pos.y = duk_to_number(ctx, 1);
  point.text_pos.x = duk_to_number(ctx, 2);
  point.text_pos.y = duk_to_number(ctx, 3);
  point.size = duk_to_number(ctx, 4);

  sprintf(point.text, "X: %0.4f Y: %0.4f Z: %0.4f", point.snap_pos.x, point.snap_pos.y, point.snap_pos.z);

  cadSetColor(GetColorStructure(color));

  dimension.Type = DIMENSION_POINT;
  dimension.Point = point;
  cadDrawDimension(dimension);

	int res = cadCountEntitys() -1;
  PostRedisplay();

	duk_push_int(ctx, res);
	return 1;  /* one return value */
}
int DrawLine2D(duk_context *ctx)
{
  line_t line;
	int i;
	int n = duk_get_top(ctx);  /* #args */
  line.start.x = duk_to_number(ctx, 0);
  line.start.y = duk_to_number(ctx, 1);
  line.end.x = duk_to_number(ctx, 2);
  line.end.y = duk_to_number(ctx, 3);

  cadSetColor(GetColorStructure(color));
  cadDrawLine(line);
	int res = cadCountEntitys() -1;
  PostRedisplay();

	duk_push_int(ctx, res);
	return 1;  /* one return value */
}
int DrawArc2D(duk_context *ctx)
{
  arc_t arc;
	int i;
	int n = duk_get_top(ctx);  /* #args */
  arc.start.x = duk_to_number(ctx, 0);
  arc.start.y = duk_to_number(ctx, 1);
  arc.end.x = duk_to_number(ctx, 2);
  arc.end.y = duk_to_number(ctx, 3);
  arc.center.x = duk_to_number(ctx, 4);
  arc.center.y = duk_to_number(ctx, 5);
	arc.radius = duk_to_number(ctx, 6);
	string direction = duk_to_string(ctx, 7);
	if (direction == "ccw")
	{
		arc.direction = ARC_CCW;
	}
	else
	{
		arc.direction = ARC_CW;
	}
	cadSetColor(GetColorStructure(color));
  cadDrawArc(arc);
	int res = cadCountEntitys() -1;
  PostRedisplay();

	duk_push_int(ctx, res);
	return 1;  /* one return value */
}
int GetArcAnglesFromPoint(duk_context *ctx)
{
	duk_get_top(ctx);  /* #args */
  string json = duk_to_string(ctx, 0);
	//printf("Json = %sn", json.c_str());
	arc_t a;
	a.center.x = atof(scriptParseJSON("center.x", json).c_str());
  a.center.y = atof(scriptParseJSON("center.y", json).c_str());
  a.start.x = atof(scriptParseJSON("start.x", json).c_str());
  a.start.y = atof(scriptParseJSON("start.y", json).c_str());
  a.end.x = atof(scriptParseJSON("end.x", json).c_str());
  a.end.y = atof(scriptParseJSON("end.y", json).c_str());
  a.radius = atof(scriptParseJSON("radius", json).c_str());
  if (scriptParseJSON("direction", json) == "cw")
  {
    a.direction = ARC_CW;
  }
  else
  {
    a.direction = ARC_CCW;
  }

  float start_angle = geoRadiansToDegrees(geoGetArcStartAngle(a));
  float end_angle = geoRadiansToDegrees(geoGetArcEndAngle(a));

  string ret_json = "{ \"start\":\"" + to_string(start_angle) + "\", \"end\":\"" + to_string(end_angle) + "\" }";


	PostRedisplay();
	duk_push_string(ctx, ret_json.c_str());
	return 1;  /* one return value */
}
int GetArcPointFromAngle(duk_context *ctx)
{
  duk_get_top(ctx);
  arc_t a;
  a.center.x = duk_to_number(ctx, 0);
  a.center.y = duk_to_number(ctx, 1);
  a.radius = duk_to_number(ctx, 2);
  if (duk_to_string(ctx, 3) == "cw")
  {
    a.direction = ARC_CW;
  }
  else
  {
    a.direction = ARC_CCW;
  }
  float angle = duk_to_number(ctx, 4);
  point_t end_point = geoGetArcPoint(a, angle);

  string json = "{ \"x\":\"" + to_string(end_point.x) + "\", \"y\":\"" + to_string(end_point.y) + "\"  }";

  PostRedisplay();
	duk_push_string(ctx, json.c_str());
	return 1;  /* one return value */
}

int RemoveSelectedEntities(duk_context *ctx)
{
  duk_get_top(ctx);
	cadRemoveSelected();
  PostRedisplay();
	duk_push_number(ctx, 0);
	return 1;  /* one return value */
}
int SelectAllEntities(duk_context *ctx)
{
	int m = cadGetEntityArrayIndex();
  cadEntity e;
  for (int a = 0; a < m; a++)
  {
    e = cadGetEntityArray(a);
    e.Selected = true;
    e.SelectionIndex = a;
    cadEdit(a, e, false);
  }
  PostRedisplay();
	duk_push_number(ctx, 0);
	return 1;  /* one return value */
}
int UnSelectAllEntities(duk_context *ctx)
{
	int m = cadGetEntityArrayIndex();
  cadEntity e;
  for (int a = 0; a < m; a++)
  {
    e = cadGetEntityArray(a);
    e.Selected = false;
    cadEdit(a, e, false);
  }
  PostRedisplay();
	duk_push_number(ctx, 0);
	return 1;  /* one return value */
}

int CountEntities(duk_context *ctx)
{
  int m = cadGetEntityArrayIndex();
	duk_push_number(ctx, m);
	return 1;  /* one return value */
}
int GetSelectedEntities(duk_context *ctx)
{
	vector<cadEntity> selected = cadGetSelected();
	string return_json;
	if (selected.size() == 0)
	{
		return_json = "None";
	}
	else
	{
		return_json = "[";
		for (int i = 0; i < selected.size(); i++)
		{
			return_json.append(to_string(selected[i].Index));
			if ((i + 1) != selected.size())
			{
				return_json.append(",");
			}
		}
		return_json.append("]");
	}

	duk_push_string(ctx, return_json.c_str());
	return 1;  /* one return value */
}
int GetEntity(duk_context *ctx)
{
	duk_get_top(ctx);  /* #args */
  int i = duk_to_number(ctx, 0);
	int m = cadGetEntityArrayIndex();
	string json;
	cadEntity e;
	if (m > i)
	{
		json.append("{");
		e = cadGetEntityArray(i);
    if (e.Type == CAD_LINE)
		{
			json.append("\"type\": \"line\",");
			json.append("\"start\": {\"x\": \"" + to_string(e.Line.start.x) + "\", \"y\": \"" + to_string(e.Line.start.y) + "\" },");
			json.append("\"end\": {\"x\": \"" + to_string(e.Line.end.x) + "\", \"y\": \"" + to_string(e.Line.end.y) + "\" },");
      json.append("\"color\": \"" + GetColorString(e.Color) + "\"");
		}
		if (e.Type == CAD_ARC)
		{
			json.append("\"type\": \"arc\",");
			json.append("\"start\": {\"x\": \"" + to_string(e.Arc.start.x) + "\", \"y\": \"" + to_string(e.Arc.start.y) + "\" },");
			json.append("\"end\": {\"x\": \"" + to_string(e.Arc.end.x) + "\", \"y\": \"" + to_string(e.Arc.end.y) + "\" },");
      json.append("\"center\": {\"x\": \"" + to_string(e.Arc.center.x) + "\", \"y\": \"" + to_string(e.Arc.center.y) + "\" },");
			json.append("\"radius\": \"" + to_string(e.Arc.radius) + "\",");
			if (e.Arc.direction == ARC_CW) json.append("\"direction\": \"cw\",");
			if (e.Arc.direction == ARC_CCW) json.append("\"direction\": \"ccw\",");
      json.append("\"color\": \"" + GetColorString(e.Color) + "\"");
		}
		json.append("}");
	}
	else
	{
		json = "{\"type\": \"None\"}";
	}
	duk_push_string(ctx, json.c_str());
	return 1;  /* one return value */
}
int RemoveEntity(duk_context *ctx)
{
	duk_get_top(ctx);  /* #args */
	int i = duk_to_int(ctx, 0);
  cadEntity e = cadGetEntityArray(i);
  e.Removed = true;
  cadEdit(i, e, false);
  PostRedisplay();
	duk_push_number(ctx, 0);
	return 1;  /* one return value */
}
int EditEntity(duk_context *ctx)
{
	duk_get_top(ctx);  /* #args */
	int i = duk_to_int(ctx, 0);
  string json = duk_to_string(ctx, 1);
	//printf("Json = %s, start = %s\n", json.c_str(),scriptParseJSON("start.x", json).c_str());
	if (scriptParseJSON("type", json) == "line")
	{
		cadEntity e = cadGetEntityArray(i);
		e.Line.start.x = atof(scriptParseJSON("start.x", json).c_str());
		e.Line.start.y = atof(scriptParseJSON("start.y", json).c_str());
		e.Line.end.x = atof(scriptParseJSON("end.x", json).c_str());
		e.Line.end.y = atof(scriptParseJSON("end.y", json).c_str());

    color = scriptParseJSON("color", json);
    e.Color = GetColorStructure(color);
		//debugDumpEntityStructure(e);
		cadEdit(i, e, false);
	}
	if (scriptParseJSON("type", json) == "arc")
	{
		cadEntity e = cadGetEntityArray(i);
		e.Arc.start.x = atof(scriptParseJSON("start.x", json).c_str());
		e.Arc.start.y = atof(scriptParseJSON("start.y", json).c_str());
		e.Arc.end.x = atof(scriptParseJSON("end.x", json).c_str());
		e.Arc.end.y = atof(scriptParseJSON("end.y", json).c_str());
    e.Arc.center.x = atof(scriptParseJSON("center.x", json).c_str());
		e.Arc.center.y = atof(scriptParseJSON("center.y", json).c_str());
		e.Arc.radius = atof(scriptParseJSON("radius", json).c_str());
		if (scriptParseJSON("direction", json) == "cw") e.Arc.direction = ARC_CW;
		if (scriptParseJSON("direction", json) == "ccw") e.Arc.direction = ARC_CCW;
		//debugDumpEntityStructure(e);

    color = scriptParseJSON("color", json);
    e.Color = GetColorStructure(color);

		cadEdit(i, e, false);
	}
	PostRedisplay();
	duk_push_number(ctx, 0);
	return 1;  /* one return value */
}
int ShowLiveEntity(duk_context *ctx)
{
	duk_get_top(ctx);  /* #args */
  string json = duk_to_string(ctx, 0);
	//printf("Json = %sn", json.c_str());
	if (scriptParseJSON("type", json) == "line")
	{
		vector<cadEntity> v;
		cadEntity e;
		e.Type = CAD_LINE;
		e.Line.start.x = atof(scriptParseJSON("start.x", json).c_str());
		e.Line.start.y = atof(scriptParseJSON("start.y", json).c_str());
		e.Line.end.x = atof(scriptParseJSON("end.x", json).c_str());
		e.Line.end.y = atof(scriptParseJSON("end.y", json).c_str());
		//debugDumpEntityStructure(e);
		v.push_back(e);
		cadShowLiveEntity(v);
	}
  if (scriptParseJSON("type", json) == "arc")
	{
		vector<cadEntity> v;
		cadEntity e;
    e.Type = CAD_ARC;
    e.Arc.start.x = atof(scriptParseJSON("start.x", json).c_str());
		e.Arc.start.y = atof(scriptParseJSON("start.y", json).c_str());
		e.Arc.end.x = atof(scriptParseJSON("end.x", json).c_str());
		e.Arc.end.y = atof(scriptParseJSON("end.y", json).c_str());
    e.Arc.center.x = atof(scriptParseJSON("center.x", json).c_str());
		e.Arc.center.y = atof(scriptParseJSON("center.y", json).c_str());
		e.Arc.radius = atof(scriptParseJSON("radius", json).c_str());
		if (scriptParseJSON("direction", json) == "cw") e.Arc.direction = ARC_CW;
		if (scriptParseJSON("direction", json) == "ccw") e.Arc.direction = ARC_CCW;
		//debugDumpEntityStructure(e);
		v.push_back(e);
		cadShowLiveEntity(v);
	}
	PostRedisplay();
	duk_push_number(ctx, 0);
	return 1;  /* one return value */
}
int HideLiveEntity(duk_context *ctx)
{
	duk_get_top(ctx);  /* #args */
	cadHideLiveEntity();
	duk_push_number(ctx, 0);
	return 1;  /* one return value */
}
int Repaint(duk_context *ctx)
{
	duk_get_top(ctx);  /* #args */
	PostRedisplay();
	duk_push_number(ctx, 0);
	return 1;  /* one return value */
}
int MouseGetSnap(duk_context *ctx)
{
	duk_get_top(ctx);  /* #args */
	point_t p = mouseCadLastSnapClick();
	string json;
	if (geoInTolerance(MousePosition, p, 0.050))
	{
		json = "{\"x\":\"" + to_string(p.x) + "\",\"y\":\"" + to_string(p.y) + "\"}";
	}
	else
	{
		json = "None";
	}

	duk_push_string(ctx, json.c_str());
	return 1;  /* one return value */
}
int MouseGetHoverOver(duk_context *ctx)
{
	duk_get_top(ctx);  /* #args */
	int id = mouseLastMouseOverEntity.Index;
	duk_push_number(ctx, id);
	return 1;  /* one return value */
}
int Source(duk_context *ctx)
{
	duk_get_top(ctx);  /* #args */
  string file = duk_to_string(ctx, 0);
	scriptSource(file);
	duk_push_number(ctx, 0);
	return 1;  /* one return value */
}
int AppendPopupMenu(duk_context *ctx)
{
	duk_get_top(ctx);  /* #args */
	PopupMenu_t p;
	p.label = duk_to_string(ctx, 0);
	p.callback = duk_to_string(ctx, 1);
	PopupMenuStack.push_back(p);
	duk_push_number(ctx, 0);
	return 1;  /* one return value */
}
int ShowPopupMenu(duk_context *ctx)
{
	duk_get_top(ctx);  /* #args */
	ShowPopupMenu();
	duk_push_number(ctx, 0);
	return 1;  /* one return value */
}
int ClearPopupMenu(duk_context *ctx)
{
	duk_get_top(ctx);  /* #args */
	PopupMenuStack.clear();
	duk_push_number(ctx, 0);
	return 1;  /* one return value */
}
int SetStatusText(duk_context *ctx)
{
	duk_get_top(ctx);  /* #args */
  StatusText = duk_to_string(ctx, 0);
	duk_push_number(ctx, 0);
	return 1;  /* one return value */
}
int DumpEntityStack(duk_context *ctx)
{
  duk_get_top(ctx);  /* #args */
  int m = cadGetEntityArrayIndex();
  cadEntity e;
  for (int a = 0; a < m; a++)
  {
    e = cadGetEntityArray(a);
		if (e.Removed == false)
		{
			cout << KGREEN << "(" << KRED << a << KGREEN << "/" << KRED << m << KGREEN << ") ";
	    debugDumpEntityStructure(e);
		}
  }
	duk_push_number(ctx, 0);
	return 1;  /* one return value */
}
int ClearEntityStack(duk_context *ctx)
{
  duk_get_top(ctx);  /* #args */
  cadClearEntityStack();
	duk_push_number(ctx, 0);
	return 1;  /* one return value */
}
int DialogAddButton(duk_context *ctx)
{
	duk_get_top(ctx);  /* #args */
  string json = duk_to_string(ctx, 0);

	dialog_t d;
	d.type = DIALOG_BUTTON;
	d.button.text = scriptParseJSON("text", json);
	d.button.position.x = atoi(scriptParseJSON("position.x", json).c_str());
	d.button.position.y = atoi(scriptParseJSON("position.y", json).c_str());
	d.button.size.x = atoi(scriptParseJSON("size.x", json).c_str());
	d.button.size.y = atoi(scriptParseJSON("size.y", json).c_str());

	DialogStack.push_back(d);

	duk_push_number(ctx, DialogStack.size() - 1);
	return 1;  /* one return value */
}
int DialogAddStaticBox(duk_context *ctx)
{
	duk_get_top(ctx);  /* #args */
  string json = duk_to_string(ctx, 0);

	dialog_t d;
	d.type = DIALOG_STATIC_BOX;
	d.static_box.text = scriptParseJSON("text", json);
	d.static_box.position.x = atoi(scriptParseJSON("position.x", json).c_str());
	d.static_box.position.y = atoi(scriptParseJSON("position.y", json).c_str());
	d.static_box.size.x = atoi(scriptParseJSON("size.x", json).c_str());
	d.static_box.size.y = atoi(scriptParseJSON("size.y", json).c_str());

	DialogStack.push_back(d);

	duk_push_number(ctx, 0);
	return 1;  /* one return value */
}
int DialogAddStaticText(duk_context *ctx)
{
	duk_get_top(ctx);  /* #args */
  string json = duk_to_string(ctx, 0);

	dialog_t d;
	d.type = DIALOG_STATIC_TEXT;
	d.static_text.text = scriptParseJSON("text", json);
	d.static_text.position.x = atoi(scriptParseJSON("position.x", json).c_str());
	d.static_text.position.y = atoi(scriptParseJSON("position.y", json).c_str());

	DialogStack.push_back(d);

	duk_push_number(ctx, 0);
	return 1;  /* one return value */
}
int DialogAddTextBox(duk_context *ctx)
{
	duk_get_top(ctx);  /* #args */
  string json = duk_to_string(ctx, 0);

	dialog_t d;
	d.type = DIALOG_TEXT_BOX;
	d.textbox.default_text = scriptParseJSON("default_text", json);
	d.textbox.position.x = atoi(scriptParseJSON("position.x", json).c_str());
	d.textbox.position.y = atoi(scriptParseJSON("position.y", json).c_str());

	DialogStack.push_back(d);

	duk_push_number(ctx, DialogStack.size() - 1);
	return 1;  /* one return value */
}
int DialogAddRadioButton(duk_context *ctx)
{
	duk_get_top(ctx);  /* #args */
  string json = duk_to_string(ctx, 0);

	dialog_t d;
	d.type = DIALOG_RADIO_BUTTON;
	d.radio_button.text = scriptParseJSON("text", json);
	d.radio_button.position.x = atoi(scriptParseJSON("position.x", json).c_str());
	d.radio_button.position.y = atoi(scriptParseJSON("position.y", json).c_str());

	DialogStack.push_back(d);

	duk_push_number(ctx, DialogStack.size() - 1);
	return 1;  /* one return value */
}
int DialogShow(duk_context *ctx)
{
	duk_get_top(ctx);  /* #args */
  string json = duk_to_string(ctx, 0);

	string title = scriptParseJSON("title", json);
	int width = atoi(scriptParseJSON("size.width", json).c_str());
	int height = atoi(scriptParseJSON("size.height", json).c_str());

	script_dialog = new Dialog(title.c_str(), wxSize(width, height));
  script_dialog->Show();
	duk_push_number(ctx, 0);
	return 1;  /* one return value */
}
int DialogClear(duk_context *ctx)
{
	duk_get_top(ctx);  /* #args */
  DialogStack.clear();
	duk_push_number(ctx, 0);
	return 1;  /* one return value */
}
int DialogClose(duk_context *ctx)
{
	duk_get_top(ctx);  /* #args */
	script_dialog->CloseDialog();
	duk_push_number(ctx, 0);
	return 1;  /* one return value */
}
int DialogGetTextboxValue(duk_context *ctx)
{
	duk_get_top(ctx);  /* #args */
  int id = duk_to_int(ctx, 0);
	string value = script_dialog->GetTextBoxValue(id);
	duk_push_string(ctx, value.c_str());
	return 1;  /* one return value */
}
int DialogGetRadioButtonValue(duk_context *ctx)
{
	duk_get_top(ctx);  /* #args */
  int id = duk_to_int(ctx, 0);
	string value = script_dialog->GetRadioButtonValue(id);
	duk_push_string(ctx, value.c_str());
	return 1;  /* one return value */
}
/***** End Javascript functions **************/

void scriptRegisterFunctions()
{
  ctx = duk_create_heap_default();

  duk_push_global_object(ctx);
	duk_push_c_function(ctx, FileOpenDialog, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "FileOpenDialog");
	duk_pop(ctx);

  duk_push_global_object(ctx);
	duk_push_c_function(ctx, FileSaveDialog, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "FileSaveDialog");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, OpenFile, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "OpenFile");
	duk_pop(ctx);

  duk_push_global_object(ctx);
	duk_push_c_function(ctx, ReadFileLine, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "ReadFileLine");
	duk_pop(ctx);

  duk_push_global_object(ctx);
	duk_push_c_function(ctx, WriteFileLine, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "WriteFileLine");
	duk_pop(ctx);

  duk_push_global_object(ctx);
	duk_push_c_function(ctx, CloseFile, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "CloseFile");
	duk_pop(ctx);

  duk_push_global_object(ctx);
	duk_push_c_function(ctx, adder, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "adder");
	duk_pop(ctx);

  duk_push_global_object(ctx);
	duk_push_c_function(ctx, ClearEntityStack, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "ClearEntityStack");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, SetDrawColor, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "SetDrawColor");
	duk_pop(ctx);

  duk_push_global_object(ctx);
	duk_push_c_function(ctx, DrawPointDimension, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "NativeDrawPointDimension");
	duk_pop(ctx);

  duk_push_global_object(ctx);
	duk_push_c_function(ctx, DrawLine2D, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "NativeDrawLine2D");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, DrawArc2D, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "NativeDrawArc2D");
	duk_pop(ctx);

  duk_push_global_object(ctx);
	duk_push_c_function(ctx, GetArcAnglesFromPoint, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "NativeGetArcAnglesFromPoint");
	duk_pop(ctx);

  duk_push_global_object(ctx);
	duk_push_c_function(ctx, GetArcPointFromAngle, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "NativeGetArcPointFromAngle");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, RemoveSelectedEntities, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "RemoveSelectedEntities");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, SelectAllEntities, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "SelectAllEntities");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, UnSelectAllEntities, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "UnSelectAllEntities");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, CountEntities, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "CountEntities");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, MouseGetSnap, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "NativeMouseGetSnap");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, GetEntity, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "NativeGetEntity");
	duk_pop(ctx);

  duk_push_global_object(ctx);
	duk_push_c_function(ctx, RemoveEntity, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "RemoveEntity");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, GetSelectedEntities, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "NativeGetSelectedEntities");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, EditEntity, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "NativeEditEntity");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, ShowLiveEntity, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "NativeShowLiveEntity");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, HideLiveEntity, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "HideLiveEntity");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, MouseGetHoverOver, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "MouseGetHoverOver");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, Repaint, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "Repaint");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, Source, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "Source");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, AppendPopupMenu, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "AppendPopupMenu");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, ClearPopupMenu, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "ClearPopupMenu");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, ShowPopupMenu, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "ShowPopupMenu");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, SetStatusText, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "SetStatusText");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, DumpEntityStack, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "DumpEntityStack");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, DialogAddButton, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "NativeDialogAddButton");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, DialogAddStaticBox, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "NativeDialogAddStaticBox");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, DialogAddStaticText, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "NativeDialogAddStaticText");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, DialogAddTextBox, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "NativeDialogAddTextBox");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, DialogAddRadioButton, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "NativeDialogAddRadioButton");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, DialogShow, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "NativeDialogShow");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, DialogClear, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "NativeDialogClear");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, DialogClose, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "DialogClose");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, DialogGetTextboxValue, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "DialogGetTextboxValue");
	duk_pop(ctx);

  duk_push_global_object(ctx);
	duk_push_c_function(ctx, DialogGetRadioButtonValue, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "NativeDialogGetRadioButtonValue");
	duk_pop(ctx);

	scriptRun("scripts/main.js");

}

void scriptRun(string file)
{
  if (duk_peval_file(ctx, file.c_str()) != 0)
  {
        printf("Error: %s\n", duk_safe_to_string(ctx, -1));
  }
  duk_pop(ctx);  /* ignore result */

  duk_push_string(ctx, "main();");
  if (duk_peval(ctx) != 0) {
    printf("eval failed: %s\n", duk_safe_to_string(ctx, -1));
  } else {
    if (!strcmp(duk_safe_to_string(ctx, -1), "undefined") == 0)
    {
      printf("result is: %s\n", duk_safe_to_string(ctx, -1));
    }
  }
  duk_pop(ctx);  /* pop result */

}
void scriptSource(string file)
{
  if (duk_peval_file(ctx, file.c_str()) != 0)
  {
        printf("Error (%s): %s\n",file.c_str(), duk_safe_to_string(ctx, -1));
  }
  duk_pop(ctx);  /* ignore result */
}

string scriptEval(string s)
{
	duk_push_string(ctx, s.c_str());
  if (duk_peval(ctx) != 0) {
    printf("[scriptEval] (%s) eval failed: %s\n",s.c_str(), duk_safe_to_string(ctx, -1));
  } else {
    if (!strcmp(duk_safe_to_string(ctx, -1), "undefined") == 0)
    {
      //makV printf("[scriptEval] result is: %s\n", duk_safe_to_string(ctx, -1));
    }
  }
	string ret = string(duk_get_string(ctx, -1));
  duk_pop(ctx);  /* pop result */
	return ret;
}
string scriptParseJSON(string key, string json)
{
	return scriptEval("ParseHelper('" + key + "', '" + json +"');");
}
