#ifndef _Engine_
#define _Engine_
#include "application.h"



class Engine                // begin declaration of the class
{
	public:                    // begin public section
	char *Filename;
	bool FirstWrite;
	Engine(char *File);     // constructor
	void Line(SDL_Renderer* r, float Start[2], float End[2]);
	void Push(char *line);
};



#endif /* _Engine_ */
