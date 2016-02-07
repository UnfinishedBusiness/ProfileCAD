#ifndef CLI_
#define CLI_
#include <application.h>

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);

void *cliInit();

void *cliScreenSelectAll();
void *cliScreenUnSelectAll();
void *cliScreenDeleteSelected();

void cliRegisterFunctions();
void cliRunScript(std::string);

#endif
