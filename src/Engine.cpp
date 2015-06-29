#include "Engine.h"

Engine::Engine(SDL_Window* w, SDL_Renderer* _r, Config *c)
{
	InitEntityArray();
	InitInstructionArray();
	InitEntityCurserPoints();

	//MouseX = 0;
	//MouseY = 0;
	//RealMouseX = 0;
	//RealMouseY = 0;
	CurserOverEntityId = -1;
	ViewRatio = 0.05;
	r = _r;
	window = w;
	//WindowWidth = _WindowWidth;
	//WindowHeight = _WindowHeight-150;
	config = c;
	OriginOffsetX = (config->GetWindowWidth()/2); //Center Origin
	OriginOffsetY = ((config->GetWindowHeight())/2); //Center Origin
	//config->UpdateWindowSize(WindowWidth, WindowHeight);
	//printf("==> Writing to %s\n", config->Filename);
}

/********** EntityCurserPoints ************/
void Engine::InitEntityCurserPoints()
{
	InitialEntityCurserSize = MaxEntities;
	EntityCurserPointsLength = 0;
	EntityCurserPointsX = (int	**)malloc(sizeof(int	**) * InitialEntityCurserSize);
	EntityCurserPointsY = (int	**)malloc(sizeof(int	**) * InitialEntityCurserSize);
}
void Engine::AppendCurserPoints(int *px, int *py, int n)
{
	if (EntityCurserPointsLength > InitialEntityArraySize) //Zero has already been allocated by InitInstructionArray()
	{
			//EntityCurserPoints = (EntityCurserStructure	**)realloc(EntityCurserPoints, sizeof(EntityCurserStructure	*)*(EntityCurserPointsSize+1));
	}
	else
	{
		EntityCurserPointsX[EntityCurserPointsLength] = (int	*)malloc(sizeof(int) * n);
		EntityCurserPointsY[EntityCurserPointsLength] = (int	*)malloc(sizeof(int) * n);
		//EntityCurserNumberOfPoints[EntityCurserPointsLength] = (int)malloc(sizeof(int));
		EntityCurserPointsX[EntityCurserPointsLength][0] = n; //First Element is number of points!
		EntityCurserPointsY[EntityCurserPointsLength][0] = n;
		for(int a = 1; a < n; a++)
		{
			//printf("appending points %d, %d\n", px[a], py[a]);
			EntityCurserPointsX[EntityCurserPointsLength][a] = px[a];
			EntityCurserPointsY[EntityCurserPointsLength][a] = py[a];
		}
		//memcpy(EntityCurserPointsX[EntityCurserPointsLength], px, sizeof(px) / sizeof(int));
		//EntityCurserPointsX[EntityCurserPointsLength] = px;
		//memcpy(EntityCurserPointsX[EntityCurserPointsLength], py, sizeof(py) / sizeof(int));
		//EntityCurserPointsY[EntityCurserPointsLength] = py;
		EntityCurserPointsLength++;
	}
}

void Engine::FreeEntityCurserPoints()
{
	for(int i=0;i<EntityCurserPointsLength; i++)
	{
			/*for(int j=0;j<EntityCurserNumberOfPoints[i]; j++)
			{
				free(EntityCurserPointsX[i][j]);
				free(EntityCurserPointsY[i][j]);
			}*/
			free(EntityCurserPointsX[i]);
			free(EntityCurserPointsY[i]);
			//free(EntityCurserNumberOfPoints[i]);
	}
	free(EntityCurserPointsX);
	free(EntityCurserPointsY);
}
/********** EntityCurserPoints ************/

/********** EntityArray ************/
void Engine::InitEntityArray()
{
	InitialEntityArraySize = MaxEntities;
	EntityArrayLength = 0;
	EntityArray = (SDL_Texture	**)malloc(sizeof(SDL_Texture	**) * InitialEntityArraySize);
}
void Engine::AppendEntityArray(SDL_Texture	*t)
{
	if (EntityArrayLength > InitialEntityArraySize) //Realocate another page!
	{
			printf("Maximum Entities (todo realloc by factor)!\n");
			//EntityArray = (SDL_Texture	**)realloc(EntityArray, sizeof(SDL_Texture	**) * (EntityArraySize+1));
	}
	else
	{
		EntityArray[EntityArrayLength] = (SDL_Texture	*)malloc(sizeof(t));
		EntityArray[EntityArrayLength] = t;
		EntityArrayLength++;
	}
}
void Engine::FreeEntityArray()
{
	for(int i=0;i<EntityArrayLength; i++)
	{
			SDL_DestroyTexture(EntityArray[i]);
	}
	free(EntityArray);
}
/********** EntityArray ************/

/********** EntityInstructionArray ************/
void Engine::InitInstructionArray()
{
	InitialEntityInstructionSize = MaxEntities;
	EntityInstructionLength = 0;
	EntityInstruction = (char	**)malloc(sizeof(char	**) * InitialEntityInstructionSize);
}
void Engine::AppendInstructionArray(char *i)
{
	if (EntityInstructionLength > InitialEntityInstructionSize) //Zero has already been allocated by InitInstructionArray()
	{
			//EntityInstruction = (char	**)realloc(EntityInstruction, sizeof(char **)*(EntityInstructionSize+1));
	}
	else
	{
		EntityInstruction[EntityInstructionLength] = strdup(i);
		EntityInstructionLength++;
	}
}
void Engine::FreeInstructionArray()
{
	for(int i=0;i<EntityInstructionLength; i++)
	{
			free(EntityInstruction[i]);
	}
	free(EntityInstruction);
}
/********** EntityInstructionArray ************/

/********** Standard instruction parsing *******/
float *Engine::ParseLineInstruction(std::string i)
{
	float *p = (float	*)malloc(sizeof(float *) * 4);
	std::size_t FirstX = i.find("x");
	std::size_t FirstY = i.find("y");
	std::size_t LastX = i.find("x", FirstX+1);
	std::size_t LastY = i.find("y", FirstY+1);
	std::string X1s = i.substr(FirstX+1, (FirstY-FirstX)-1);
	std::string Y1s = i.substr(FirstY+1, (LastX - FirstY) -1);
	std::string X2s = i.substr(LastX+1, (LastY - LastX)-1);
	std::string Y2s = i.substr(LastY+1, (i.length() - LastY)-1);
	p[0] = atof((char*)X1s.c_str());
	p[1] = atof((char*)Y1s.c_str());
	p[2] = atof((char*)X2s.c_str());
	p[3] = atof((char*)Y2s.c_str());
	//printf("x%f, y%f x%f, y%f\n", X1, Y1, X2, Y2);
	return p;
}
/********** Standard instruction parsing *******/
int Engine::GetCurserOverId()
{
	int x = CurserOverEntityId;
	//memcpy( (void*) &x, (void*) &CurserOverEntityId, sizeof(int) );
	CurserOverEntityId = -1;
	return x;
}
void Engine::UpdateWindowSize(int w, int h)
{
	WindowWidth = w;
	WindowHeight = h;
	config->UpdateWindowSize(w, h);
}
void Engine::GetWindowSize(int w, int h)
{
	w = WindowWidth;
	w = WindowHeight;
}
void Engine::GetMousePos(float out[2])
{
	int x, y;
	SDL_GetMouseState(&x, &y);
 	//out[0] = (x - OriginOffsetX);
	//out[1] = ((y - OriginOffsetY)/-1);
	//RealMouseX = x;
	//RealMouseY = y;
	float in[2];
	in[0] = x;
	in[1] = y;
	GetXY(out, in);
	//MouseX = out[0];
	//MouseY = out[1];
}
void Engine::PanXY(float pos[2])
{
	OriginOffsetX = (WindowWidth - pos[0]) / 2; //Center Origin
	OriginOffsetY = (WindowHeight - pos[1]) / 2; //Center Origin
}
void Engine::PanIncX(float p)
{
	OriginOffsetX = OriginOffsetX + p;
	EntityRedraw = true;
}
void Engine::PanIncY(float p)
{
	OriginOffsetY = OriginOffsetY + p;
	EntityRedraw = true;
}
float Engine::ZoomIn()
{
	float ZoomInc;
	if (ViewRatio < 0.005)
	{
		ZoomInc = 0.0002;
	}
	else
	{
		ZoomInc = 0.001;
	}
	if (ViewRatio > 0.0001)
	{
		ViewRatio = ViewRatio - ZoomInc;
		EntityRedraw = true;
	}
	else
	{
		ViewRatio = .0001;
		EntityRedraw = true;
	}
	return ViewRatio;
}
float Engine::ZoomOut()
{
	float ZoomInc;
	if (ViewRatio < 0.005)
	{
		ZoomInc = 0.0002;
	}
	else
	{
		ZoomInc = 0.001;
	}
	ViewRatio = ViewRatio + ZoomInc;
	EntityRedraw = true;
	return ViewRatio;
}
void Engine::GetDistance(float out, float p1[2], float p2[2])
{
	//out = sqrt(pow((p2[0] - p1[0]), 2) + pow((p2[1] - p1[1]), 2)); //dont work!
	out = sqrtf((p2[0] - p1[0])*(p2[0] - p1[0]) + (p2[1] - p1[1])*(p2[1] - p1[1])); //probably doesnt work, at least didnt idfk
}
void Engine::GetRealXY(float out[2], float in[2])
{
	out[0] = ((in[0] / ViewRatio) + OriginOffsetX);
	out[1] = (-1*((in[1] / ViewRatio) - OriginOffsetY));
}
void Engine::GetXY(float out[2], float in[2])
{
	out[0] = ((in[0] - OriginOffsetX) * ViewRatio);
	out[1] = (((in[1] - OriginOffsetY)/-1) * ViewRatio);
}
float Engine::GetX(float in[2])
{
	float out[2];
	GetXY(out, in);
	return out[0];
}
float Engine::GetY(float in[2])
{
	float out[2];
	GetXY(out, in);
	return out[1];
}
void Engine::Line(float Start[2], float End[2])
{
	float screen_point1[2];
	float screen_point2[2];

	GetRealXY(screen_point1, Start);
	GetRealXY(screen_point2, End);

	SDL_Texture *texture = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WindowWidth, WindowHeight);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureAlphaMod(texture, 255); //Make texture clear
	SDL_SetRenderTarget( r, texture );
	SDL_SetRenderDrawColor( r, 0, 0, 0, 0 ); //Make texture clear
	//SDL_RenderClear(r);
	config->Color((char*)config->LineColor);

	//SDL_RenderDrawLine(r, screen_point1[0], screen_point1[1], screen_point2[0], screen_point2[1]); //But we need actual points

	//double x = x2 - x1;
	//double y = y2 - y1;
	double x = screen_point2[0] - screen_point1[0];
	double y = screen_point2[1] - screen_point1[1];

	double length = sqrt( x*x + y*y );
	double addx = x / length;
	double addy = y / length;

	x = screen_point1[0];
	y = screen_point1[1];

	int pointsX[(int)length+1]; //for CurserPoints!
	int pointsY[(int)length+1]; //for CurserPoints!
	for(double i = 0; i < length; i += 1)
	{
		SDL_RenderDrawPoint(r, (int)x, (int)y);
		pointsX[(int)i] = (int)x;
		pointsY[(int)i] = (int)y;
	  x += addx;
	  y += addy;
	}
	SDL_RenderPresent( r );
	SDL_SetRenderTarget( r, NULL );
	AppendCurserPoints(pointsX, pointsY, length);
	AppendEntityArray(texture);
	if (EntityRedrawWithoutNewInstructions == false)
	{
		std::string Instruction = "lx" + std::to_string(Start[0]) + "y" + std::to_string(Start[1]) + "x" + std::to_string(End[0]) + "y" + std::to_string(End[1]);
		AppendInstructionArray((char *)Instruction.c_str());
	}
}
void Engine::ArcByCenter(float x, float y, float Radius)
{
	float pos[2];
	float r_pos[2];
	int centrex=x,centrey=y;// centre of circle in pixel coords
	float ypos, xpos;
	float two_pi=6.283f;
	float angle_inc=0.001f/Radius;
	int NumberOfPoints=0;
	for(float angle=0.0f; angle<= two_pi;angle+=angle_inc)
	{
		NumberOfPoints++;
	}
	int pointsX[NumberOfPoints];
	int pointsY[NumberOfPoints];
	//SDL_Surface *surface = SDL_CreateRGBSurface(0, WindowWidth, WindowHeight, 32, 0, 0, 0, 0);
	SDL_Texture *texture = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WindowWidth, WindowHeight);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureAlphaMod(texture, 255); //Make texture clear
	SDL_SetRenderTarget( r, texture );
	SDL_SetRenderDrawColor( r, 0, 0, 0, 0 ); //Make texture clear
	//SDL_RenderClear(r);
	config->Color((char*)config->LineColor);

	int count = 0;
	for(float angle=0.0f; angle<= two_pi;angle+=angle_inc)
	{
	    xpos=centrex+Radius*cos(angle);
	    ypos=centrey+Radius*sin(angle);
			pos[0] = xpos;
			pos[1] = ypos;
	    GetRealXY(r_pos, pos);
			//printf("Circle point: %d, %d\n", (int)r_pos[0], (int)r_pos[1]);
			//SDL_RenderDrawLine(r, r_pos[0], r_pos[1], r_pos[0]+1, r_pos[1]+1);
			pointsX[count] = r_pos[0];
			pointsY[count] = r_pos[1];

			SDL_RenderDrawPoint(r, r_pos[0], r_pos[1]);
			count++;
	}
	SDL_RenderPresent( r );
	SDL_SetRenderTarget( r, NULL );

	AppendEntityArray(texture);
	AppendCurserPoints(pointsX, pointsY, NumberOfPoints);
	if (EntityRedrawWithoutNewInstructions == false)
	{
		std::string Instruction = "acx" + std::to_string(x) + "y" + std::to_string(y) + "r" + std::to_string(Radius);
		AppendInstructionArray((char *)Instruction.c_str());
	}
}
const char* Engine::GetField(char* line, int num)
{
    const char* tok;
		char *str = strdup(line);
    for (tok = strtok(str, ":"); tok && *tok; tok = strtok(NULL, ":\n"))
    {
        if (!--num)
				{
					free(str);
					return tok;
				}
    }
		free(str);
    return "";
}
SDL_Texture* Engine::MakeText(char *Text, int Size)
{
	return MakeColorText(config->ColorWhite, Text, Size);
}
SDL_Texture* Engine::MakeColorText(SDL_Color Color, char *Text, int Size)
{

	TTF_Font *font = TTF_OpenFont(config->Font, Size);
	if (font == nullptr){
		printf("Cant open font %s\n", config->Font);
		return nullptr;
	}
	//We need to first render to a surface as that's what TTF_RenderText
	//returns, then load that surface into a texture
	SDL_Surface *surf = TTF_RenderText_Blended(font, Text, Color);
	if (surf == nullptr){
		TTF_CloseFont(font);
		printf("Error rendering text\n");
		return nullptr;
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(r, surf);
	if (texture == nullptr){
		printf("Error creating surface\n");
	}
	//Clean up the surface and font
	SDL_FreeSurface(surf);
	TTF_CloseFont(font);
	return texture;
}
void Engine::PutTexture(SDL_Texture *t, float x, float y)
{
	SDL_Rect texture_rect;
	int w, h;
	SDL_QueryTexture(t, NULL, NULL, &w, &h);
	texture_rect.x = x;  //the x coordinate
	texture_rect.y = y; // the y coordinate
	texture_rect.w = w; //the width of the texture
	texture_rect.h = h; //the height of the texture
	SDL_RenderCopy(r, t, NULL, &texture_rect);
	if (PutTextureAndDontFree == true)
	{
			PutTextureAndDontFree = false;
	}
	else
	{
		SDL_DestroyTexture(t);
	}
}
void Engine::UpdateScreen()
{
	//printf("EntityCount: %d\r", EntityArraySize);
	//SDL_ScaleSurface(SDL_Surface* Surface, Uint16 Width, Uint16 Height)
	bool CurserOverEntity = false;
	int TmpCurserOverEntityId = -1;
	int mX, mY;

	int x;

	if (EntityRedraw == true)
	{
		EntityRedraw = false;
		FreeEntityArray();
		InitEntityArray();

		FreeEntityCurserPoints();
		InitEntityCurserPoints();

		EntityRedrawWithoutNewInstructions = true;
		for(x = 0; x < EntityInstructionLength; x++)
		{
				if (EntityInstruction != NULL)
				{
						std::string i = std::string(EntityInstruction[x]);
						//printf("===>Entity Instruction: %s\n", i.c_str());
						if (i.find("ac") != std::string::npos)
						{
							std::size_t cXp = i.find("x");
							std::size_t cYp = i.find("y");
							std::size_t Rp  = i.find("r");

							std::string cX = i.substr(cXp+1, (cYp - cXp) -1);
							std::string cY = i.substr(cYp+1, (Rp - cYp)-1);
							std::string R = i.substr(Rp+1, (i.length() - Rp)-1);
							ArcByCenter(atof((char *)cX.c_str()), atof((char *)cY.c_str()), atof((char *)R.c_str()));
						}
						if (i.find("l") != std::string::npos)
						{
							std::size_t FirstX = i.find("x");
							std::size_t FirstY = i.find("y");
							std::size_t LastX = i.find("x", FirstX+1);
							std::size_t LastY = i.find("y", FirstY+1);
							std::string X1 = i.substr(FirstX+1, (FirstY-FirstX)-1);
							std::string Y1 = i.substr(FirstY+1, (LastX - FirstY) -1);
							std::string X2 = i.substr(LastX+1, (LastY - LastX)-1);
							std::string Y2 = i.substr(LastY+1, (i.length() - LastY)-1);
							//std::cout << "\t>Added Line X1: " + X1 + " Y1: " + Y1 + " X2: " + X2 + " Y2: " + Y2 + "\n";
							float LineStart[2];
							float LineEnd[2];
							LineStart[0] = atof((char*)X1.c_str());
							LineStart[1] = atof((char*)Y1.c_str());
							LineEnd[0] = atof((char*)X2.c_str());
							LineEnd[1] = atof((char*)Y2.c_str());
							Line(LineStart, LineEnd);
						}
				}
				else
				{
					printf("\t====> Entity %d is corrupt\n", x);
				}
				//printf("Puting Texture: %d\n", x);
		}
		EntityRedrawWithoutNewInstructions = false;
	}
	else
	{
		float distance;
		float p1[2];
		float p2[2];
		SDL_GetMouseState(&mX, &mY);
		for(x = 0; x < EntityArrayLength; x++)
		{
				if (EntityArray[x] != NULL)
				{
					SDL_Texture *t = EntityArray[x];
					//First Element is number of points
					for(int a = 1; a < EntityCurserPointsX[x][0]; a++)
					{
						if (isBetween(EntityCurserPointsX[x][a], mX+EntityCurserRange, mX-EntityCurserRange) && isBetween(EntityCurserPointsY[x][a], mY+EntityCurserRange, mY-EntityCurserRange))
						{
							CurserOverEntity = true;
						 	TmpCurserOverEntityId = x;
						}
						else
						{
							//SDL_SetTextureColorMod(t, 255, 255, 255);
						}

					}
					if (CurserOverEntity == true)
					{
						CurserOverEntity = false;
						SDL_SetTextureColorMod(t, 0, 255, 0);

					}
					else
					{
							//CurserOverEntityId = -1;
							SDL_SetTextureColorMod(t, 255, 255, 255);
					}
					PutTextureAndDontFree = true;
					PutTexture(t, 0, 0);

				}
				else
				{
					printf("\t====> Entity %d is corrupt\n", x);
				}
		}
	}
	CurserOverEntityId = TmpCurserOverEntityId;
}
void Engine::DrawPixel(SDL_Surface *screen, int x, int y, Uint8 R, Uint8 G, Uint8 B)
{
  Uint32 color = SDL_MapRGB(screen->format, R, G, B);
  switch (screen->format->BytesPerPixel)
  {
    case 1: // Assuming 8-bpp
      {
        Uint8 *bufp;
        bufp = (Uint8 *)screen->pixels + y*screen->pitch + x;
        *bufp = color;
      }
      break;
    case 2: // Probably 15-bpp or 16-bpp
      {
        Uint16 *bufp;
        bufp = (Uint16 *)screen->pixels + y*screen->pitch/2 + x;
        *bufp = color;
      }
      break;
    case 3: // Slow 24-bpp mode, usually not used
      {
        Uint8 *bufp;
        bufp = (Uint8 *)screen->pixels + y*screen->pitch + x * 3;
        if(SDL_BYTEORDER == SDL_LIL_ENDIAN)
        {
          bufp[0] = color;
          bufp[1] = color >> 8;
          bufp[2] = color >> 16;
        } else {
          bufp[2] = color;
          bufp[1] = color >> 8;
          bufp[0] = color >> 16;
        }
      }
      break;
    case 4: // Probably 32-bpp
      {
        Uint32 *bufp;
        bufp = (Uint32 *)screen->pixels + y*screen->pitch/4 + x;
        *bufp = color;
      }
      break;
  }
}

void Engine::UnInit()
{
	//g_slist_free(Entitys);
	//g_slist_free(SelectedEntitys);
	FreeEntityCurserPoints();
	FreeEntityArray();
	FreeInstructionArray();
	delete config;
}
