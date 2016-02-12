#define _USE_MATH_DEFINES

#include <sys/signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <fcntl.h>
//#include <png.h>
//#include <GL/freeglut.h>
//#include <GL/freeglut_ext.h>

#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif

#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <cctype>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <array>
#include <iomanip>
#include <locale>

#include <CAD.h>
#include <Mouse.h>
#include <Scene.h>
#include <Debug.h>
#include <FileIO.h>
#include <Geometry.h>
#include <Script.h>
#include <Dialog.h>
#include <duktape.h>

#include "zlib.h"

#ifndef APPLICATION_
#define  APPLICATION_


void PostRedisplay();

#define VERBOSE true

#define WINDOW_TITLE "ProfileCAD"

#define DEBUG

#ifdef DEBUG
 #define D
#else
 #define D for(;0;)
#endif

#define V if(VERBOSE)

#define CAD_REAL_WORLD_CLOSE 0.00005 //Half a tenth
#define CAD_ARC 0x00
#define CAD_LINE 0x01
#define CAD_NOTE 0x02
#define CAD_DIMENSION 0x03
#define DIMENSION_POINT 0x00
#define ARC_CW true
#define ARC_CCW false
#define CONTOUR_RIGHT true
#define CONTOUR_LEFT false

struct point_t{
  float x,y,z;
  bool operator==(const point_t& rhs)
  {
    //return x == rhs.x && y == rhs.y && z == rhs.z;
    auto t = [](float a, float b)
    {
      float diff;
      if (a > b)
      {
        diff = a - b;
      }
      else
      {
        diff = b - a;
      }
      //printf("(geoInTolerance) Difference: %.6f, Plus: %.6f, Minus: %.6f\n", diff, fabs(t), -fabs(t));
      if (diff <= fabs(CAD_REAL_WORLD_CLOSE) && diff >= -fabs(CAD_REAL_WORLD_CLOSE))
      {
        return true;
      }
      else
      {
        return false;
      }
    };
    return t(x, rhs.x) && t(y, rhs.y) && t(z, rhs.z);
  }
  bool operator!=(const point_t& rhs)
  {
    return x != rhs.x && y != rhs.y && z != rhs.z;
  }
  int parrentIndex;
  float tmp;
};
struct line_t{
  point_t start;
  point_t end;

  //Operation data
  int parentIndex;
};
struct arc_t{
  point_t start;
  point_t end;
  point_t center;
  float radius;
  bool direction;

  //Operation data
  int parentIndex;
};
struct note_t{
  int size;
  point_t pos;
  char text[2048];

  int parentIndex;
};
struct dimension_point_t{
  int size;
  point_t snap_pos;
  point_t text_pos;
  char text[2048];
};
struct dimension_t{
  int Type;
  int Entity;
  dimension_point_t Point;

  int parentIndex;
};
struct color_t{
  float r, g, b, a;
  bool operator==(const color_t& rhs)
  {
    return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
  }
  bool operator!=(const color_t& rhs)
  {
    return r != rhs.r && g != rhs.g && b != rhs.b && a != rhs.a;
  }
};
struct cadEntity{
  int Type; //Line or Arc
  color_t Color;
  line_t Line;
  arc_t Arc;
  note_t Note;
  dimension_t Dimension;
  float LineWidth;
  //Operational data
  bool Selected;
  bool MouseOver;
  point_t SelectedAt;
  bool Removed;
  int Index;
  int SelectionIndex;
};
struct contour_t{
  std::vector<cadEntity> Entitys;
  bool isClosed;
};
#define CAD_CYCLE_CONTOUR 0

struct cad_cycle_countour_t{
  float feed;
  float plunge_feed;
  float retract_feed;
  float start_z;
};
struct cadToolpath{
  int Cycle;
  float ToolNumber;
  float ToolDiameter;
  float SpindleSpeed;
  bool Coolant;
  contour_t Path;
  bool Side;
  cad_cycle_countour_t ContourCycle;
};
struct PopupMenu_t{
  std::string label;
  std::string callback;
};
#define BACKGROUND color_t{0, 0, 0.05, 1}

#define WHITE color_t{1, 1, 1, 1}
#define BLACK color_t{0, 0, 0, 1}
#define RED   color_t{1, 0, 0, 1}
#define GREEN color_t{0, 1, 0, 1}
#define BLUE  color_t{0, 0, 1, 1}
#define YELLOW  color_t{1, 1, 0.5, 1}
#define PURPLE  color_t{0.5, 0.1, 0.8, 1}
#define CYAN  color_t{0, 1, 1, 1}
#define MAGENTA color_t{1, 0, 1, 1}
#define LIGHTGREY color_t{0.3, 0.3, 0.3, 1}
#define DARKGREY color_t{0.8, 0.8, 0.8, 1}

#define SELECTED_COLOR CYAN
#define MOUSEOVER_COLOR WHITE
#define TOOLPATH_COLOR MAGENTA
#define KNORMAL  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGREEN  "\x1B[32m"
#define KYELLOW  "\x1B[33m"
#define KBLUE  "\x1B[34m"
#define KMAGENTA  "\x1B[35m"
#define KCYAN  "\x1B[36m"
#define KWHITE  "\x1B[37m"


#define ZERO 1e-10
#define SMALL 0.1
#define DELTA 1e-10
#define isZero(A) ( (A < ZERO) && (A > -ZERO) )
#define isSmall(A) ( (A < SMALL) && (A > -SMALL) )
#define isSame(A, B) ( ((A-B) < ZERO) && ((A-B) > -ZERO) )
#define isSimilar(A, B) ( ((A-B) < SMALL) && ((A-B) > -SMALL) )
#define isBetween(A, B, C) ( ((A-B) > -ZERO) && ((A-C) < ZERO) )
#define POSITIVE(n) ((n) < 0 ? 0 - (n) : (n))

#define MOUSE_LEFT_BUTTON 0
#define MOUSE_DOWN 1
#define MOUSE_RIGHT_BUTTON 2
#define MOUSE_ACTIVE_CTRL 3

/* Keyboard Modifier structure */
struct KbMods_t{
  bool Ctrl = false;
  bool Alt = false;
  bool Shift = false;
};
extern KbMods_t KbMods;
/* Global Operation variables */
extern std::vector<PopupMenu_t> PopupMenuStack;
extern point_t MousePosition;
extern bool PostRedisplay_Register;
extern duk_context *ctx;
extern std::string StatusText;
/* End Global Variables */

void CleanupAndExit();
void ShowPopupMenu();
#define POPUP_MENU_ID_OFFSET 1000

#endif
