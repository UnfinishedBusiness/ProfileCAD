#ifndef LUA_
#define LUA_
#include <application.h>

void report_errors(lua_State*, int);
void luaInit();
void luaExec(std::string);
std::string luaEval(std::string c);

void luaInit(std::string);
std::string luaCallFunction(std::string);
std::string luaCallCycle(std::string, cadToolpath);
void luaClose();
#endif
