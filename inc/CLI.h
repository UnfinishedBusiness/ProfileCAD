#ifndef CLI_
#define CLI_
#include <application.h>

void cliParse(std::string); //Depricated
void cliPush(std::string);
void cliMenu();
void cliBackup();
void cliReturn();
float cliGetInput();
std::string cliGetTextInput();
std::string cliGetCurrentFile();

void *cliFileExit();
void *cliFileGetNew();
void *cliFileGetMerge();
void *cliFilePutCurrent();
void *cliFilePutNew();

void *cliCreateLineVerticalOrigin();
void *cliCreateLineHorizontalOrigin();
void *cliCreateLineVerticalEndpoints();
void *cliCreateLineEndpoints();
void *cliCreateLinePerpendicular();
void *cliCreateLineParallel();

void *cliScreenSelectAll();
void *cliScreenUnSelectAll();
void *cliScreenDeleteSelected();
void *cliScreenColorRed();
void *cliScreenColorBlue();
void *cliScreenColorGreen();

void *cliViewPlaneXY();
void *cliViewPlaneYZ();
void *cliViewPlaneZX();
void *cliViewPlaneOrtho();

void *cliXformTrim1();
#endif
