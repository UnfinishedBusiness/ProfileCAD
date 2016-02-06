#ifndef FILEIO_
#define FILEIO_
#include <application.h>

struct Header_t{
  int NumberOfEntitys;
  int NumberOfToolpaths;
};
struct cadToolpath_pod_t{
  int Cycle;
  float ToolNumber;
  float ToolDiameter;
  float SpindleSpeed;
  bool Coolant;
  //contour_t Path;
  bool Side;
  cad_cycle_countour_t ContourCycle;

  bool PathIsClosed;
  int NumberOfPathEntitys;
};

#define FILE_OK 0
#define FILE_WRITE_ERROR -1
#define FILE_READ_ERROR -2
#define FILE_OPEN_ERROR -3
#define FILE_UNKNOWN_FORMAT -4

int fileOpen(std::string);
int fileSave(std::string);

//Dxf Methods
void fileReadDXF();
void fileWriteDXF();

//NC Methods
void fileReadNC();

//PFCAD Methods
void fileReadPFCAD();
void fileWritePFCAD();
#endif
