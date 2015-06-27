#ifndef _application_
#define _application_

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <glib.h>
#include <iostream>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <pwd.h>

#define WindowTitle "ProfileCAD"
#define Version "1"
#define MainWindowWidth 800
#define MainWindowHeight 640

inline std::string trim(const std::string &s)
{
   auto wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
   auto wsback=std::find_if_not(s.rbegin(),s.rend(),[](int c){return std::isspace(c);}).base();
   return (wsback<=wsfront ? std::string() : std::string(wsfront,wsback));
}

#endif
