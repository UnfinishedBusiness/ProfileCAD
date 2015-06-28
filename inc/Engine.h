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
	public:
	#define LINE 0
	#define CIRCLE	 1
	struct _EntityCurserStructure
	{
		int EntityType;
	  int LineWidthMax;
		int LineWidthMin;
	  int LineHeightMax;
		int LineHeightMin;
		int *CirclePoints;
	};
	typedef struct _EntityCurserStructure *EntityCurserStructure;
	// begin public section
	float OriginOffsetX;
	float OriginOffsetY;
	float ViewRatio;
	int WindowWidth;
	int WindowHeight;
	SDL_Window* window;
	SDL_Renderer* r;
	Config *config;

	int MaxEntities = 500;

	int InitialEntityArraySize;
	int EntityArrayLength;
	SDL_Texture	**EntityArray;

	char **EntityInstruction;
	int InitialEntityInstructionSize;
	int EntityInstructionLength;

	EntityCurserStructure **EntityCurserPoints;
	int EntityCurserPointsSize;

	bool EntityRedraw = false;
	bool EntityRedrawWithoutNewInstructions = false;
	bool PutTextureAndDontFree = false;

	Engine(SDL_Window* w, SDL_Renderer* _r, Config *c, int _WindowWidth, int _WindowHeight);     // constructor
	const char* GetField(char* line, int num);

	void InitEntityArray();
	void AppendEntityArray(SDL_Texture	*t);
	void FreeEntityArray();

	void InitInstructionArray();
	void AppendInstructionArray(char *i);
	void FreeInstructionArray();

	void InitEntityCurserPoints();
	void AppentLineCurserPoints(int maxw, int minw, int maxh, int minh);
	void AppendCicrleCurserPoints(int *points);
	void FreeEntityCurserPoints();

	void UpdateWindowSize(int w, int h);
	void GetWindowSize(int w, int h);
	void GetRealXY(float out[2], float in[2]); //Get Screen Cordinant plane fron Virtual Cordinant plane
	void GetXY(float out[2], float in[2]); //Get Virtual Cordinant plane from Screen Cordinant plane
	float GetX(float in[2]); //GetXY Wrapper Function
	float GetY(float in[2]); //GetXY Wrapper Function
	void GetMousePos(float out[2]); //Returns Virtual Cordinant plane
	void Line(float Start[2], float End[2]);
	void ArcByCenter(float x, float y, float Radius);
	void DrawPixel(SDL_Surface *screen, int x, int y, Uint8 R, Uint8 G, Uint8 B);
	float ZoomIn();
	float ZoomOut();
	void PanIncX(float p);
	void PanIncY(float p);
	void PanXY(float pos[2]);
	void GetDistance(float out, float p1[2], float p2[2]);
	SDL_Texture* MakeText(char *Text, int Size);
	SDL_Texture* MakeColorText(SDL_Color Color, char *Text, int Size);
	void PutTexture(SDL_Texture* t, float x, float y);
	void UpdateScreen();
	void ClearScreen();
	void UnInit();
};



#endif /* _Engine_ */
