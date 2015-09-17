#ifndef FILEIO_
#define FILEIO_
#include <application.h>

#define FILE_OK 0
#define FILE_WRITE_ERROR -1
#define FILE_READ_ERROR -2
#define FILE_OPEN_ERROR -3
#define FILE_UNKNOWN_FORMAT -4

int fileOpen(std::string);
int fileSave(std::string);
void fileReadDXF();
void fileWriteDXF();
#endif
