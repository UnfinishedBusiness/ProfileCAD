#ifndef _Config_
#define _Config_
#include "application.h"

//#define _Engine_
//#include "Engine.h"

class Config                 // begin declaration of the class
{
	public:                    // begin public section
	char *homedir;
	char *ConfigFile;

	char *BackgroundColor;
	int WindowWidth;
	int WindowHeight;
	SDL_Window* window;
	char *Filename;
	SDL_Renderer* renderer;
	char *Font;
	//Engine *Engine;

	char *LineColor;
	SDL_Color ColorWhite = { 255, 255, 255, 255 };
	SDL_Color ColorBlack = { 0, 0, 0, 255 };
	SDL_Color ColorRed = { 255, 0, 0, 255 };
	SDL_Color ColorGreen = { 0, 255, 0, 255 };
	SDL_Color ColorBlue = { 0, 0, 255, 255 };
	SDL_Color ColorYellow = { 255, 255, 0, 255 };
	SDL_Color ColorPurple = { 128, 0, 128, 255 };
	SDL_Color ColorClear = { 0, 0, 0, 0 };

	Config(SDL_Window* w, SDL_Renderer* r);     // constructor
	void Color(char *color);
	void ParseConfig();
	void SaveConfig();
	void UpdateWindowSize(int w, int h);
};



#endif /* _Config_ */
