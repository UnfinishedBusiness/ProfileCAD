#define _USE_MATH_DEFINES

#include <sys/signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <fcntl.h>
#include <png.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
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
#include <CLI.h>
#include <Debug.h>
#include <Dialog.h>
#include <FileIO.h>
#include <Geometry.h>
#include <Keyboard.h>
#include <Lua.h>
#include <Machine.h>
#include <Menu.h>
#include <Mouse.h>
#include <Scene.h>
#include <UI.h>
#include <Window.h>

#include "zlib.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#ifndef APPLICATION_
#define  APPLICATION_

#define EXIT cout << "Bye!\n"; exit(0);

#define WINDOW_HEIGHT 800
#define WINDOW_WIDTH 1100
#define WINDOW_TITLE "ProfileCAD"

#define DEBUG

#ifdef DEBUG
 #define D
#else
 #define D for(;0;)
#endif

#define V if(mainVerbose())

#define CAD_ARC 0x00
#define CAD_LINE 0x01
#define CAD_NOTE 0x02
#define CAD_DIMENSION 0x03
#define DIMENSION_POINT 0x00
#define ARC_CW true
#define ARC_CCW false
#define CONTOUR_RIGHT true
#define CONTOUR_LEFT false

struct args_t{
  int argc;
  std::string args;
};

args_t mainGetArgs();
bool mainVerbose();
int set_interface_attribs (int, int, int);
void set_blocking (int, int);

struct point_t{
  float x,y,z;
  bool operator==(const point_t& rhs)
  {
    return x == rhs.x && y == rhs.y && z == rhs.z;
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
  point_t start_reference;
  std::vector<cadEntity> Entitys;
  std::vector<point_t> Points;
  bool isClosed;
};
#ifdef __APPLE__
  #define BACKGROUND color_t{0, 0, 0.10, 1}
#else
  #define BACKGROUND color_t{0, 0, 0.05, 1}
#endif

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
#endif
