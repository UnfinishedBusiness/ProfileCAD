#ifndef _Engine_
#define _Engine_
#include "application.h"
#include "Config.h"

class Engine                // begin declaration of the class
{
	public:                    // begin public section
	char *Filename;
	float OriginOffsetX;
	float OriginOffsetY;
	float ViewRatio;
	char *LineColor;

	GSList *Entitys;

	Engine(char *File, int WindowWidth, int WindowHeight);     // constructor
	const char* GetField(char* line, int num);
	void GetRealXY(float out[2], float in[2]); //Get Screen Cordinant plane fron Virtual Cordinant plane
	void GetXY(float out[2], float in[2]); //Get Virtual Cordinant plane from Screen Cordinant plane
	float GetX(float in[2]); //GetXY Wrapper Function
	float GetY(float in[2]); //GetXY Wrapper Function
	void GetMousePos(float out[2]); //Returns Virtual Cordinant plane
	void Line(SDL_Renderer* r, float Start[2], float End[2]);
	void Push(char *line); //Pushes Entitys to memmory
	void Pull(SDL_Renderer* r); //Pulls entitys from memory
	void Save(); //Copys entitys from memory to file
	void Open(SDL_Renderer* r); //Copys entitys from file to memory and updates screen
	void Trash();
	float ZoomIn();
	float ZoomOut();

};



#endif /* _Engine_ */
