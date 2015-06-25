#ifndef _Engine_
#define _Engine_
#include "application.h"
#include "Config.h"


class Engine                // begin declaration of the class
{
	public:                    // begin public section
	char *Filename;
	bool FirstWrite;
	float ViewRatio;
	char *LineColor;
	Engine(char *File);     // constructor
	const char* GetField(char* line, int num);
	void Line(SDL_Renderer* r, float Start[2], float End[2]);
	void Push(char *line);
	void Pull(SDL_Renderer* r);
	void Trash();

};



#endif /* _Engine_ */
