#ifndef _Engine_
#define _Engine_
#include "application.h"


#ifndef _Config_
#include "Config.h"
#endif
typedef struct{
		float x,y;
}point;

typedef struct{
		point start;
		point end;
		point center1;
		point center2;
		float radius;
		int possible;
}circle;

#define CIRCLE true
#define ARC false
#define ARC_CCW -1
#define ARC_CW 1
typedef struct{
		point start;
		point end;
		point center;
		float radius;
		int direction;
	  bool type;
}arc;

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
	float OriginOffsetX;
	float OriginOffsetY;
	float ViewRatio;
	int WindowWidth;
	int WindowHeight;
	SDL_Window* window;
	SDL_Renderer* r;
	Config *config;

	int CurserOverEntityId;
	int MaxEntities = 100;
	int EntityCurserRange = 10;

	int InitialEntityArraySize;
	int EntityArrayLength;
	SDL_Texture	**EntityArray;

	char **EntityInstruction;
	int InitialEntityInstructionSize;
	int EntityInstructionLength;

	int **EntityCurserPointsX;
	int **EntityCurserPointsY;
	int InitialEntityCurserSize;
	int EntityCurserPointsLength;

	bool EntityRedraw = false;
	bool EntityRedrawWithoutNewInstructions = false;
	bool PutTextureAndDontFree = false;

	Engine(SDL_Window* w, SDL_Renderer* _r, Config *c);     // constructor
	const char* GetField(char* line, int num);

	void InitEntityArray();
	void AppendEntityArray(SDL_Texture	*t);
	void FreeEntityArray();

	void InitInstructionArray();
	void AppendInstructionArray(char *i);
	void FreeInstructionArray();

	void InitEntityCurserPoints();
	void AppendCurserPoints(int *px, int *py, int n);
	void FreeEntityCurserPoints();

	/*Standard Instruction Parsing*/
 	float *ParseLineInstruction(std::string i);
	float *ParseArcInstruction(std::string i);
	float *ParseArcByCenterInstruction(std::string i);
	/*Standard Instruction Parsing*/

	void ParseAndExecuteInstructions(std::string i);

	int Open();
	int Save();

	int GetCurserOverId();
	void UpdateWindowSize(int w, int h);
	void GetWindowSize(int w, int h);

	void GetRealXY(float out[2], float in[2]); //Get Screen Cordinant plane fron Virtual Cordinant plane
	point GetRealXY(point in); //Wrapper function for point structure

	void GetXY(float out[2], float in[2]); //Get Virtual Cordinant plane from Screen Cordinant plane
	float GetX(float in[2]); //GetXY Wrapper Function
	float GetY(float in[2]); //GetXY Wrapper Function
	float GetX(float x); //GetXY Wrapper Function
	float GetY(float y); //GetXY Wrapper Function
	void GetMousePos(float out[2]); //Returns Virtual Cordinant plane

	/********* Draw to renderer functions ************/
	void Line(float Start[2], float End[2]);
	void Arc(arc data);
	/********* Draw to renderer functions ************/
	float *GetPointAlongLine(float x1, float y1, float x2, float y2, float len);
	float *GetPointAlong45Line(float x1, float y1, float len, int dir);
	float *GetPointAlongSlope(float x, float y, float rise, float run, float len, int dir);
	void DrawPixel(SDL_Surface *screen, int x, int y, Uint8 R, Uint8 G, Uint8 B);
	float ZoomIn();
	float ZoomOut();
	void PanIncX(float p);
	void PanIncY(float p);
	void PanXY(float pos[2]);

	float GetDistance(float x1, float y1, float x2, float y2);
	float GetDistance(point p1, point p2);
	circle GetCircleCenters(point p1, point p2, float radius);

	SDL_Texture* MakeText(char *Text, int Size);
	SDL_Texture* MakeColorText(SDL_Color Color, char *Text, int Size);
	void PutTexture(SDL_Texture* t, float x, float y);
	void UpdateScreen();
	void ClearScreen();
	void UnInit();
};



#endif /* _Engine_ */
