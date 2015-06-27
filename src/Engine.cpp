#include "Engine.h"

Engine::Engine(SDL_Window* w, SDL_Renderer* _r, Config *c, int _WindowWidth, int _WindowHeight)
{
	//memcpy(config->Filename, File, sizeof(*File));
	EntityArraySize = 0;
	EntityArray = (SDL_Texture	**)malloc(sizeof(SDL_Texture	*));
	EntityInstruction = (char	**)malloc(sizeof(char	*));

	ViewRatio = 0.05;
	r = _r;
	window = w;
	WindowWidth = _WindowWidth;
	WindowHeight = _WindowHeight-150;
	OriginOffsetX = (_WindowWidth/2); //Center Origin
	OriginOffsetY = (_WindowHeight/2); //Center Origin
	config = c;
	config->UpdateWindowSize(WindowWidth, WindowHeight);
	//printf("==> Writing to %s\n", config->Filename);
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
	float in[2];
	in[0] = x;
	in[1] = y;
	GetXY(out, in);
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
	ViewRatio = ViewRatio - .001;
	EntityRedraw = true;
	return ViewRatio;
}
float Engine::ZoomOut()
{
	ViewRatio = ViewRatio + .001;
	EntityRedraw = true;
	return ViewRatio;
}
void Engine::GetDistance(float out, float p1[2], float p2[2])
{
	out = sqrt(pow((p2[0] - p1[0]), 2) + pow((p2[1] - p1[1]), 2));
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
	SDL_RenderClear(r);
	config->Color((char*)config->LineColor);
	SDL_RenderDrawLine(r, screen_point1[0], screen_point1[1], screen_point2[0], screen_point2[1]);
	SDL_RenderPresent( r );
	SDL_SetRenderTarget( r, NULL );
	//EntityArray[EntityArraySize] = (SDL_Texture	*)malloc(sizeof(SDL_Texture	*));
	if (EntityArraySize > 0)
	{
			EntityArray = (SDL_Texture	**)realloc(EntityArray, sizeof(SDL_Texture	*)*(EntityArraySize+1));
			EntityInstruction = (char	**)realloc(EntityInstruction, sizeof(char	*)*(EntityArraySize+1));
	}
	EntityArray[EntityArraySize] = texture;
	std::string Instruction = "lx" + std::to_string(Start[0]) + "y" + std::to_string(Start[1]) + "x" + std::to_string(End[0]) + "y" + std::to_string(End[1]);
	EntityInstruction[EntityArraySize] = strdup(Instruction.c_str());
	//printf("Added Instruction: %s\n", EntityInstruction[EntityArraySize]);
	EntityArraySize++;
}
void Engine::ArcByCenter(float x, float y, float Radius)
{
	float pos[2];
	float r_pos[2];
	int centrex=x,centrey=y;// centre of circle in pixel coords
	float ypos, xpos;
	float two_pi=6.283f;
	float angle_inc=0.01f/Radius;
	//SDL_Surface *surface = SDL_CreateRGBSurface(0, WindowWidth, WindowHeight, 32, 0, 0, 0, 0);
	SDL_Texture *texture = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WindowWidth, WindowHeight);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureAlphaMod(texture, 255); //Make texture clear
	SDL_SetRenderTarget( r, texture );
	SDL_SetRenderDrawColor( r, 0, 0, 0, 0 ); //Make texture clear
	SDL_RenderClear(r);
	config->Color((char*)config->LineColor);

	for(float angle=0.0f; angle<= two_pi;angle+=angle_inc)
	{
	    xpos=centrex+Radius*cos(angle);
	    ypos=centrey+Radius*sin(angle);
			pos[0] = xpos;
			pos[1] = ypos;
	    GetRealXY(r_pos, pos);
			//printf("Circle point: %d, %d\n", (int)r_pos[0], (int)r_pos[1]);
			//SDL_RenderDrawLine(r, r_pos[0], r_pos[1], r_pos[0]+1, r_pos[1]+1);
			SDL_RenderDrawPoint(r, r_pos[0], r_pos[1]);
	}
	SDL_RenderPresent( r );
	SDL_SetRenderTarget( r, NULL );
	EntityArray[EntityArraySize] = (SDL_Texture	*)malloc(sizeof(SDL_Texture	*));
	if (EntityArraySize > 0)
	{
			EntityArray = (SDL_Texture	**)realloc(EntityArray, sizeof(SDL_Texture	*)*(EntityArraySize+1));
			EntityInstruction = (char	**)realloc(EntityInstruction, sizeof(char	*)*(EntityArraySize+1));
	}
	EntityArray[EntityArraySize] = texture;
	std::string Instruction = "acx" + std::to_string(x) + "y" + std::to_string(y) + "r" + std::to_string(Radius);
	EntityInstruction[EntityArraySize] = strdup(Instruction.c_str());
	//printf("Added Instruction: %s\n", EntityInstruction[EntityArraySize]);
	EntityArraySize++;
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
	int x;
	if (EntityRedraw == true)
	{
		EntityRedraw = false;

		int EntityInstructionSize = EntityArraySize; //Make copy because when drawing depent on EntityArraySize
		char **EntityInstructionCopy = (char	**)malloc(sizeof(EntityInstruction));

		for(int i=0;i<EntityArraySize; i++)
		{
				EntityInstructionCopy = (char	**)realloc(EntityInstructionCopy, sizeof(char	*)*(i+1));
				EntityInstructionCopy[i] = strdup(EntityInstruction[i]);
				SDL_DestroyTexture(EntityArray[i]);
				free(EntityInstruction[i]);
		}
		free(EntityArray);
		free(EntityInstruction);

		EntityArraySize = 0;
		EntityArray = (SDL_Texture	**)malloc(sizeof(SDL_Texture	*));
		EntityInstruction = (char	**)malloc(sizeof(char	*));

		for(x = 0; x < EntityInstructionSize; x++)
		{
				if (EntityInstructionCopy != NULL)
				{
						std::string i = std::string(EntityInstructionCopy[x]);
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
		for(int i=0;i<EntityInstructionSize; i++)
		{
				free(EntityInstructionCopy[i]);
		}
		free(EntityInstructionCopy);
	}
	else
	{
		for(x = 0; x < EntityArraySize; x++)
		{
				if (EntityArray[x] != NULL)
				{
					SDL_Texture *t = EntityArray[x];
					PutTextureAndDontFree = true;
					PutTexture(t, 0, 0);
				}
				else
				{
					printf("\t====> Entity %d is corrupt\n", x);
				}
		}
	}
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
	for(int i=0;i<EntityArraySize; i++)
	{
			SDL_DestroyTexture(EntityArray[i]);
			free(EntityInstruction[i]);
	}
	free(EntityArray);
	free(EntityInstruction);
	delete config;
}
