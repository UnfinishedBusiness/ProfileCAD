#ifndef _Engine_
#define _Engine_
#include "application.h"


#ifndef _Config_
#include "Config.h"
#endif

class Engine                // begin declaration of the class
{
	private:
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	    Uint32 rmask = 0xff000000;
	    Uint32 gmask = 0x00ff0000;
	    Uint32 bmask = 0x0000ff00;
	    Uint32 amask = 0x000000ff;
	#else
	    Uint32 rmask = 0x000000ff;
	    Uint32 gmask = 0x0000ff00;
	    Uint32 bmask = 0x00ff0000;
	    Uint32 amask = 0xff000000;
	#endif
	public:                    // begin public section
	float OriginOffsetX;
	float OriginOffsetY;
	float ViewRatio;
	int WindowWidth;
	int WindowHeight;
	SDL_Window* window;
	SDL_Renderer* r;
	GSList *SelectedEntitys;
	Config *config;

	int EntityArraySize;
	SDL_Texture	**EntityArray;

	Engine(SDL_Window* w, SDL_Renderer* _r, Config *c, int _WindowWidth, int _WindowHeight);     // constructor
	const char* GetField(char* line, int num);
	void UpdateWindowSize(int w, int h);
	void GetWindowSize(int w, int h);
	void GetRealXY(float out[2], float in[2]); //Get Screen Cordinant plane fron Virtual Cordinant plane
	void GetXY(float out[2], float in[2]); //Get Virtual Cordinant plane from Screen Cordinant plane
	float GetX(float in[2]); //GetXY Wrapper Function
	float GetY(float in[2]); //GetXY Wrapper Function
	void GetMousePos(float out[2]); //Returns Virtual Cordinant plane
	void Line(float Start[2], float End[2]);
	void Trash();
	float ZoomIn();
	float ZoomOut();
	void PanIncX(float p);
	void PanIncY(float p);
	void PanXY(float pos[2]);
	void GetDistance(float out, float p1[2], float p2[2]);
	SDL_Texture* MakeText(char *Text, int Size);
	SDL_Texture* MakeColorText(SDL_Color Color, char *Text, int Size);
	void PutTexture(SDL_Texture* t, float x, float y);
	void AddEntity(SDL_Texture	*t);
	void UpdateScreen();
	void UnInit();
};



#endif /* _Engine_ */
