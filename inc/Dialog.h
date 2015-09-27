#ifndef DIALOG_
#define DIALOG_
#include <application.h>

void dialogAddButton(point_t, float, float, std::string , void (*c)(void));
void dialogAddTextBox(point_t, float, float, std::string);
void dialogAddTextBox(point_t, float, float, std::string, std::string);
void dialogAddLabel(point_t, std::string);

std::string dialogTextboxGetString(std::string);
void dialogSetTitle(std::string);
void dialogSetPosition(int, int);

void dialogResize(int, int);
void dialogRender();
void dialogDestroy();
void dialogClose();
void dialogKeyboard();
void dialogInit();

void dialogOpen(std::string);
void dialogOpen();

#endif
