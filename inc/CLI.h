#ifndef CLI_
#define CLI_
#include <application.h>

void cliParse(std::string); //Depricated
void cliPush(std::string);
void cliMenu();
void cliBackup();
void cliReturn();

void *cliCreateLineVerticalOrigin();
void *cliCreateLineHorizontalOrigin();
void *cliViewPlaneXY();
void *cliViewPlaneYZ();
void *cliViewPlaneZX();
void *cliViewPlaneOrtho();
#endif
