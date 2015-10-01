#ifndef CLI_
#define CLI_
#include <application.h>

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);

void cliInit();

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

void *cliCreateCircleCenterDiameter();
void *cliCreateCircleCenterRadius();
void *cliCreateCircleTwoPoints();

void *cliScreenSelectAll();
void *cliScreenUnSelectAll();
void *cliScreenDeleteSelected();

void *cliScreenColorWhite();
void *cliScreenColorBlack();
void *cliScreenColorRed();
void *cliScreenColorGreen();
void *cliScreenColorBlue();
void *cliScreenColorYellow();
void *cliScreenColorCyan();
void *cliScreenColorMagenta();
void *cliScreenColorGrey();
void *cliScreenAnalyzeEntity();

void *cliViewPlaneXY();
void *cliViewPlaneYZ();
void *cliViewPlaneZX();
void *cliViewPlaneOrtho();

void *cliXformTrim1();
void *cliXformTrim2();
void *cliXformTrimCircle();
void *cliXformFilletRadius();

void *cliXformRotateSelected();
void *cliXformRotateAll();
void cliXformRotate_Callback(); //We use the same callback for both methods

void *cliXformTranslateSelected();
void *cliXformTranslateAll();
void cliXformTranslate_Callback(); //We use the same callback for both methods

#endif
