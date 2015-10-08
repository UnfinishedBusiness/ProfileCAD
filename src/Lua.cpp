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
int Index = 1;
point_t lastpoint;
vector<point_t> UsedPoints;
void luaPushEntity(cadEntity e)
{
  if (e.Type == CAD_LINE)
  {
    if (!geoInTolerance(e.Line.start, lastpoint, 0.001))
    {
      if (std::find(UsedPoints.begin(), UsedPoints.end(), e.Line.start) == UsedPoints.end())
      {
        lua_newtable(Lua); /* upper table */

        lua_pushstring(Lua, "Line");
        lua_setfield(Lua, -2, "Type"); /* T[four] = 4 */

        lua_pushstring(Lua, geoSupressZeros(e.Line.start.x).c_str());
        lua_setfield(Lua, -2, "X");
        lua_pushstring(Lua, geoSupressZeros(e.Line.start.y).c_str());
        lua_setfield(Lua, -2, "Y");
        lua_pushstring(Lua, geoSupressZeros(e.Line.start.z).c_str());
        lua_setfield(Lua, -2, "Z");
        UsedPoints.push_back(e.Line.start);
        lastpoint = e.Line.start;

        lua_rawseti(Lua,-2, Index++);
        return;
      }
    }
    else
    {
      if (std::find(UsedPoints.begin(), UsedPoints.end(), e.Line.end) == UsedPoints.end())
      {
        lua_newtable(Lua); /* upper table */

        lua_pushstring(Lua, "Line");
        lua_setfield(Lua, -2, "Type"); /* T[four] = 4 */

        lua_pushstring(Lua, geoSupressZeros(e.Line.end.x).c_str());
        lua_setfield(Lua, -2, "X");
        lua_pushstring(Lua, geoSupressZeros(e.Line.end.y).c_str());
        lua_setfield(Lua, -2, "Y");
        lua_pushstring(Lua, geoSupressZeros(e.Line.end.z).c_str());
        lua_setfield(Lua, -2, "Z");
        UsedPoints.push_back(e.Line.end);
        lastpoint = e.Line.end;

        lua_rawseti(Lua,-2, Index++);
        return;
      }
    }
  }
  if (e.Type == CAD_ARC)
  {
    if (!geoInTolerance(e.Arc.start, lastpoint, 0.001))
    {
        if (std::find(UsedPoints.begin(), UsedPoints.end(), e.Arc.start) == UsedPoints.end())
        {
          lua_newtable(Lua); /* upper table */

          lua_pushstring(Lua, "Arc");
          lua_setfield(Lua, -2, "Type"); /* T[four] = 4 */

          lua_pushstring(Lua, geoSupressZeros(e.Arc.start.x).c_str());
          lua_setfield(Lua, -2, "X");
          lua_pushstring(Lua, geoSupressZeros(e.Arc.start.y).c_str());
          lua_setfield(Lua, -2, "Y");
          lua_pushstring(Lua, geoSupressZeros(e.Arc.start.z).c_str());
          lua_setfield(Lua, -2, "Z");
          lua_pushstring(Lua, geoSupressZeros(e.Arc.radius).c_str());
          lua_setfield(Lua, -2, "R");

          if (e.Arc.direction == ARC_CW)
          {
            lua_pushstring(Lua, "CW");
            lua_setfield(Lua, -2, "Direction");
          }
          else
          {
            lua_pushstring(Lua, "CCW");
            lua_setfield(Lua, -2, "Direction");
          }

          UsedPoints.push_back(e.Arc.start);
          lastpoint = e.Arc.start;

          lua_rawseti(Lua,-2, Index++);
          return;
        }
    }
    else
    {
      if (std::find(UsedPoints.begin(), UsedPoints.end(), e.Arc.end) == UsedPoints.end())
      {
        lua_newtable(Lua); /* upper table */

        lua_pushstring(Lua, "Arc");
        lua_setfield(Lua, -2, "Type"); /* T[four] = 4 */

        lua_pushstring(Lua, geoSupressZeros(e.Arc.end.x).c_str());
        lua_setfield(Lua, -2, "X");
        lua_pushstring(Lua, geoSupressZeros(e.Arc.end.y).c_str());
        lua_setfield(Lua, -2, "Y");
        lua_pushstring(Lua, geoSupressZeros(e.Arc.end.z).c_str());
        lua_setfield(Lua, -2, "Z");
        lua_pushstring(Lua, geoSupressZeros(e.Arc.radius).c_str());
        lua_setfield(Lua, -2, "R");

        if (e.Arc.direction == ARC_CW)
        {
          lua_pushstring(Lua, "CW");
          lua_setfield(Lua, -2, "Direction");
        }
        else
        {
          lua_pushstring(Lua, "CCW");
          lua_setfield(Lua, -2, "Direction");
        }

        UsedPoints.push_back(e.Arc.end);
        lastpoint = e.Arc.end;

        lua_rawseti(Lua,-2, Index++);
        return;
      }
    }
  }

  //lua_setfield(Lua, -2, to_string(x+1).c_str());  /* name upper table field T of bottom table */
}
std::string luaCallCycle(std::string cycle, cadToolpath t)
{
  lua_getglobal(Lua, cycle.c_str());

  lua_pushstring(Lua, geoBoolToString(t.Coolant).c_str());
  lua_pushstring(Lua, geoSupressZeros(t.SpindleSpeed).c_str());
  if (t.Cycle == CAD_CYCLE_CONTOUR)
  {
    lua_pushstring(Lua, geoSupressZeros(t.ContourCycle.start_z).c_str());
    lua_pushstring(Lua, geoSupressZeros(t.ContourCycle.plunge_feed).c_str());
    lua_pushstring(Lua, geoSupressZeros(t.ContourCycle.feed).c_str());
    lua_pushstring(Lua, geoSupressZeros(t.ContourCycle.retract_feed).c_str());
  }
  lua_newtable(Lua); //First table


  //luaPushEntity(cadEntity{});

  string s;

  for (int x = 0; x < t.Path.Entitys.size(); x++)
  {
    luaPushEntity(t.Path.Entitys[x]); //Push twice to post start and endpoints
    luaPushEntity(t.Path.Entitys[x]);
  }
  if (t.Path.isClosed)
  {
    UsedPoints.clear(); //Release used points and push first start point again
    luaPushEntity(t.Path.Entitys[0]);
  }
  UsedPoints.clear();
  Index = 1;
  //lua_pcall(Lua, 0, LUA_MULTRET, 0);
  lua_pcall(Lua, 7, LUA_MULTRET, 1);
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
