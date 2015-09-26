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
