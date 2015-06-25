#ifndef _Engine_
#define _Engine_
#include "application.h"
#include "Config.h"


class Engine                // begin declaration of the class
{
	public:                    // begin public section
	char *Filename;
	bool FirstWrite;
	float OriginOffsetX;
	float OriginOffsetY;
	float ViewRatio;
	char *LineColor;
	Engine(char *File, int WindowWidth, int WindowHeight);     // constructor
	const char* GetField(char* line, int num);
	void GetRealXY(float out[2], float in[2]); //Screen Cordinant plane
	void GetXY(float out[2], float in[2]); //Virtual Cordinant plane
	float GetX(float in[2]); //GetXY Wrapper Function
	float Gety(float in[2]); //GetXY Wrapper Function
	void GetMousePos(float out[2]); //Returns Virtual Cordinant plane
	void Line(SDL_Renderer* r, float Start[2], float End[2]);
	void Push(char *line);
	void Pull(SDL_Renderer* r);
	void Trash();
	float ZoomIn();
	float ZoomOut();

};



#endif /* _Engine_ */
