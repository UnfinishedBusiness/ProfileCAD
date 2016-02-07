#ifndef SCRIPT_
#define SCRIPT_
#include <application.h>

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);

void *scriptInit();

void *scriptScreenSelectAll();
void *scriptScreenUnSelectAll();
void *scriptScreenDeleteSelected();

void scriptRegisterFunctions();
void scriptRun(std::string);
void scriptSource(std::string);
std::string scriptEval(std::string);

std::string scriptParseJSON(std::string, std::string);

#endif
