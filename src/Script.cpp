#include <Script.h>

using namespace std;

duk_context *ctx;
string color;

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

/***** Javascript functions **************/
int OpenFile(duk_context *ctx)
{
	int n = duk_get_top(ctx);  /* #args */
  string file = duk_to_string(ctx, 0);
  fileOpen(file);
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
int DrawLine2D(duk_context *ctx)
{
  line_t line;
	int i;
	int n = duk_get_top(ctx);  /* #args */
	double res = 0.0;
  line.start.x = duk_to_number(ctx, 0);
  line.start.y = duk_to_number(ctx, 1);
  line.end.x = duk_to_number(ctx, 2);
  line.end.y = duk_to_number(ctx, 3);

	if (color == "black") cadSetColor(BLACK);
  if (color == "red") cadSetColor(RED);
  if (color == "yellow") cadSetColor(YELLOW);
  if (color == "green") cadSetColor(GREEN);
  if (color == "cyan") cadSetColor(CYAN);
  if (color == "blue") cadSetColor(BLUE);
  if (color == "magenta") cadSetColor(MAGENTA);
  if (color == "darkgrey") cadSetColor(DARKGREY);
  if (color == "lightgrey") cadSetColor(LIGHTGREY);
  cadDrawLine(line);
  PostRedisplay();

	duk_push_number(ctx, res);
	return 1;  /* one return value */
}
int RemoveSelectedEntities(duk_context *ctx)
{
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
			json.append("\"end\": {\"x\": \"" + to_string(e.Line.end.x) + "\", \"y\": \"" + to_string(e.Line.end.y) + "\" }");
		}
		if (e.Type == CAD_ARC)
		{
			json.append("\"type\": \"arc\",");
			json.append("\"start\": {\"x\": \"" + to_string(e.Arc.start.x) + "\", \"y\": \"" + to_string(e.Arc.start.y) + "\" },");
			json.append("\"end\": {\"x\": \"" + to_string(e.Arc.end.x) + "\", \"y\": \"" + to_string(e.Arc.end.y) + "\" },");
			json.append("\"radius\": \"" + to_string(e.Arc.radius) + "\",");
			if (e.Arc.direction == ARC_CW) json.append("\"direction\": \"cw\"");
			if (e.Arc.direction == ARC_CCW) json.append("\"direction\": \"ccw\"");
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
int EditEntity(duk_context *ctx)
{
	duk_get_top(ctx);  /* #args */
	int i = duk_to_int(ctx, 0);
  string json = duk_to_string(ctx, 1);
	printf("Json = %s, start = %s\n", json.c_str(),scriptParseJSON("start.x", json).c_str());
	/*if (scriptParseJSON("type", json) == "line")
	{
		cadEntity e = cadGetEntityArray(i);
	}*/

	duk_push_number(ctx, 0);
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
int DumptEntityStack(duk_context *ctx)
{
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
/***** End Javascript functions **************/

void scriptRegisterFunctions()
{
  ctx = duk_create_heap_default();

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, OpenFile, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "OpenFile");
	duk_pop(ctx);

  duk_push_global_object(ctx);
	duk_push_c_function(ctx, adder, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "adder");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, SetDrawColor, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "SetDrawColor");
	duk_pop(ctx);

  duk_push_global_object(ctx);
	duk_push_c_function(ctx, DrawLine2D, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "NativeDrawLine2D");
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
	duk_push_c_function(ctx, GetEntity, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "NativeGetEntity");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, EditEntity, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "NativeEditEntity");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, Source, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "Source");
	duk_pop(ctx);

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, DumptEntityStack, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "DumptEntityStack");
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
        printf("Error: %s\n", duk_safe_to_string(ctx, -1));
  }
  duk_pop(ctx);  /* ignore result */
}

string scriptEval(string s)
{
	duk_push_string(ctx, s.c_str());
  if (duk_peval(ctx) != 0) {
    printf("[scriptEval] eval failed: %s\n", duk_safe_to_string(ctx, -1));
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
