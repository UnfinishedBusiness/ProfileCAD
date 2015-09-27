#ifndef DIALOG_
#define DIALOG_
#include <application.h>

void dialogAddButton(point_t, float, float, std::string , void (*c)(void));
void dialogAddTextBox(point_t, float, float, std::string id);
void dialogAddLabel(point_t, std::string);

std::string dialogTextboxGetString(std::string);

void dialogResize(int, int);
void dialogRender();
void dialogDestroy();
void dialogClose();
void dialogOpen();
void dialogKeyboard();
void dialogInit();

#endif
