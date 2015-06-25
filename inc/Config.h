#ifndef _Config_
#define _Config_
#include "application.h"



class Config                 // begin declaration of the class
{
	public:                    // begin public section
	SDL_Renderer* renderer;
	char *Font;
	SDL_Color White = { 255, 255, 255, 255 };
	Config(SDL_Renderer* r);     // constructor
	void ColorBlack();
	void ColorWhite();
	void Color(char *color);
};



#endif /* _Config_ */
