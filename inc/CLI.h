#ifndef CLI_
#define CLI_
#include <application.h>

void cliParse(std::string); //Depricated
void cliPush(std::string);
void cliMenu();
void cliBackup();
void cliReturn();
float cliGetInput();

void *cliFileExit();
void *cliCreateLineVerticalOrigin();
void *cliCreateLineHorizontalOrigin();
void *cliCreateLineVerticalEndpoints();
void *cliCreateLineEndpoints();
void *cliCreateLinePerpendicular();
void *cliCreateLineParallel();
void *cliScreenSelectAll();
void *cliScreenUnSelectAll();
void *cliScreenDeleteSelected();
void *cliViewPlaneXY();
void *cliViewPlaneYZ();
void *cliViewPlaneZX();
void *cliScreenColorRed();
void *cliScreenColorBlue();
void *cliScreenColorGreen();
void *cliViewPlaneOrtho();
#endif
