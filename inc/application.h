#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
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

#include <CAD.h>
#include <CLI.h>
#include <Debug.h>
#include <FileIO.h>
#include <Geometry.h>
#include <Keyboard.h>
#include <Menu.h>
#include <Mouse.h>
#include <Scene.h>
#include <UI.h>
#include <Window.h>

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

#define ARC_CW true
#define ARC_CCW false

struct args_t{
  int argc;
  std::string args;
};

args_t mainGetArgs();
bool mainVerbose();

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
#define KCYNAN  "\x1B[36m"
#define KWHITE  "\x1B[37m"


#define ZERO 1e-10
#define SMALL 0.1
#define DELTA 1e-10
#define isZero(A) ( (A < ZERO) && (A > -ZERO) )
#define isSmall(A) ( (A < SMALL) && (A > -SMALL) )
#define isSame(A, B) ( ((A-B) < ZERO) && ((A-B) > -ZERO) )
#define isSimilar(A, B) ( ((A-B) < SMALL) && ((A-B) > -SMALL) )
#define isBetween(A, B, C) ( ((A-B) > -ZERO) && ((A-C) < ZERO) )
#endif
