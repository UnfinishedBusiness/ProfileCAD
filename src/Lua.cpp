#include <Lua.h>

using namespace std;

void report_errors(lua_State *L, int status)
{
  if ( status!=0 ) {
    std::cerr << KRED << "(Lua Error)" << KMAGENTA << " ==> " << KGREEN << lua_tostring(L, -1) << std::endl;
    lua_pop(L, 1); // remove error message
  }
}
void luaInit()
{
  //
}
int luaDraw(lua_State *L)
{
  int argc = lua_gettop(L);

  std::cerr << "-- luaCadDrawLine called with " << argc
    << " arguments:" << std::endl;

  for ( int n=1; n<=argc; ++n ) {
    std::cerr << "-- argument " << n << ": "
      << lua_tostring(L, n) << std::endl;
  }
  lua_pushnumber(L, 123); // return value
  return 1; // number of return values
}

void luaExec(string f)
{
  lua_State *L = luaL_newstate();
  luaopen_io(L); // provides io.*
  luaopen_base(L);
  luaopen_table(L);
  luaopen_string(L);
  luaopen_math(L);
  luaL_openlibs(L);

  //Register Functions
  lua_register(L, "Draw", luaDraw);

  int s = luaL_loadfile(L, f.c_str());
  if (s == 0)
  {
      s = lua_pcall(L, 0, LUA_MULTRET, 0);
  }
  report_errors(L, s);
  lua_close(L);
}
string luaEval(string c)
{
  //string expresion = "return " + c;
  string expresion = "e = load('return " + c + "');";
  string r;
  lua_State *L = luaL_newstate();
  luaopen_io(L); // provides io.*
  luaopen_base(L);
  luaopen_table(L);
  luaopen_string(L);
  luaopen_math(L);
  luaL_openlibs(L);

  luaL_dostring(L, expresion.c_str());
  luaL_dostring(L, "out = e();");
  lua_getglobal(L, "out");
  if (!lua_isstring(L, -1))
  {
    lua_close(L);
    return "";
  }
  r = lua_tostring(L, -1);
  lua_close(L);
  return r;
}

lua_State *Lua;
void luaInit(std::string f)
{
  Lua = luaL_newstate();
  luaopen_io(Lua); // provides io.*
  luaopen_base(Lua);
  luaopen_table(Lua);
  luaopen_string(Lua);
  luaopen_math(Lua);
  luaL_openlibs(Lua);

  //Register Functions
  //lua_register(L, "Draw", luaDraw);

  luaL_dofile(Lua, f.c_str());
}
std::string luaCallFunction(std::string function)
{
  vector<string> args = split(function, ' ');
  //cout << "Lua Calling => " << args[0] << endl;
  lua_getglobal(Lua, args[0].c_str());
  for (int x = 1; x < args.size(); x++)
  {
    //cout << "Pushing argument " << x << endl;
    lua_pushstring(Lua, args[x].c_str());
  }
  lua_pcall(Lua, args.size()-1, LUA_MULTRET, 1);
  //lua_call(Lua, args.size()-1, 1);
  string r = lua_tostring(Lua, -1);
	lua_pop(Lua, 1);
  return r;
}
std::string luaCallCycle(std::string cycle, cadToolpath t)
{
  lua_getglobal(Lua, cycle.c_str());

  lua_pushstring(Lua, geoBoolToString(t.Coolant).c_str());
  lua_pushstring(Lua, geoSupressZeros(t.SpindleSpeed).c_str());
  if (t.Cycle == CAD_CYCLE_CONTOUR)
  {
    lua_pushinteger(Lua, t.ContourCycle.plunge_feed);
    lua_pushinteger(Lua, t.ContourCycle.feed);
    lua_pushinteger(Lua, t.ContourCycle.retract_feed);
  }
  lua_newtable(Lua);
  string s;
  point_t lastpoint;
  for (int x = 0; x < t.Path.Entitys.size(); x++)
  {
    if (t.Path.Entitys[x].Type == CAD_LINE)
    {
      s = "[LINE]";
      if (!geoInTolerance(t.Path.Entitys[x].Line.start, lastpoint, 0.001))
      {
        s = s + "X" + geoSupressZeros(t.Path.Entitys[x].Line.start.x)
          + "Y" + geoSupressZeros(t.Path.Entitys[x].Line.start.y)
          + "Z" + geoSupressZeros(t.Path.Entitys[x].Line.start.z);
        lastpoint = t.Path.Entitys[x].Line.start;
      }
      else
      {
        s = s + "X" + geoSupressZeros(t.Path.Entitys[x].Line.end.x)
          + "Y" + geoSupressZeros(t.Path.Entitys[x].Line.end.y)
          + "Z" + geoSupressZeros(t.Path.Entitys[x].Line.end.z);
        lastpoint = t.Path.Entitys[x].Line.end;
      }
    }
    if (t.Path.Entitys[x].Type == CAD_ARC)
    {
      if (t.Path.Entitys[x].Arc.direction == ARC_CW)
      {
        s = "[ARC CW]";
      }
      else
      {
        s = "[ARC CCW]";
      }
      if (!geoInTolerance(t.Path.Entitys[x].Arc.start, lastpoint, 0.001))
      {
        s = s + "X" + geoSupressZeros(t.Path.Entitys[x].Arc.start.x)
          + "Y" + geoSupressZeros(t.Path.Entitys[x].Arc.start.y)
          + "Z" + geoSupressZeros(t.Path.Entitys[x].Arc.start.z);
        lastpoint = t.Path.Entitys[x].Arc.start;
      }
      else
      {
        s = s + "X" + geoSupressZeros(t.Path.Entitys[x].Arc.end.x)
          + "Y" + geoSupressZeros(t.Path.Entitys[x].Arc.end.y)
          + "Z" + geoSupressZeros(t.Path.Entitys[x].Arc.end.z);
        lastpoint = t.Path.Entitys[x].Arc.end;
      }
      s = s + "R" + geoSupressZeros(t.Path.Entitys[x].Arc.radius);
    }
    lua_pushstring(Lua, s.c_str());
    lua_rawseti(Lua,-2, x+1);
  }
  //lua_pcall(Lua, 0, LUA_MULTRET, 0);
  lua_pcall(Lua, 6, LUA_MULTRET, 1);
  if (!lua_isstring(Lua, -1))
  {
    return "";
  }
  string r = lua_tostring(Lua, -1);
	lua_pop(Lua, 1);
  return r;
}
void luaClose()
{
  lua_close(Lua);
}
