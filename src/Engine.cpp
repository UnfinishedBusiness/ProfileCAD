#include "Engine.h"

Engine::Engine(SDL_Window* w, SDL_Renderer* _r, Config *c)
{
	InitEntityArray();
	InitInstructionArray();
	InitEntityCurserPoints();

	CurserOverEntityId = -1;
	ViewRatio = 0.05;
	r = _r;
	window = w;
	config = c;
	OriginOffsetX = (config->GetWindowWidth()/2); //Center Origin
	OriginOffsetY = ((config->GetWindowHeight())/2); //Center Origin

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
		EntityCurserPointsX[EntityCurserPointsLength][0] = n; //First Element is number of points!
		EntityCurserPointsY[EntityCurserPointsLength][0] = n;
		for(int a = 1; a < n; a++)
		{
			EntityCurserPointsX[EntityCurserPointsLength][a] = px[a];
			EntityCurserPointsY[EntityCurserPointsLength][a] = py[a];
		}
		EntityCurserPointsLength++;
	}
}

void Engine::FreeEntityCurserPoints()
{
	for(int i=0;i<EntityCurserPointsLength; i++)
	{
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
float *Engine::ParseArcInstruction(std::string i)
{
	float *p = (float	*)malloc(sizeof(float *) * 5);
	std::size_t X1p = i.find("x");
	std::size_t Y1p = i.find("y");
	std::size_t X2p = i.find("x", X1p+1);
	std::size_t Y2p = i.find("y", Y1p+1);
	std::size_t Rp  = i.find("r");
	std::size_t Dp  = i.find("d");

	std::string X1 = i.substr(X1p+1, (Y1p-X1p)-1);
	std::string Y1 = i.substr(Y1p+1, (X2p - Y1p) -1);
	std::string X2 = i.substr(X2p+1, (Y2p - X2p)-1);
	std::string Y2 = i.substr(Y2p+1, (Rp - Y2p)-1);
	std::string R = i.substr(Rp+1, (Dp - Rp)-1);
	std::string D = i.substr(Dp+1, (i.length() - Dp)-1);
	p[0] = atof((char*)X1.c_str());
	p[1] = atof((char*)Y1.c_str());
	p[2] = atof((char*)X2.c_str());
	p[3] = atof((char*)Y2.c_str());
	p[4] = atof((char*)R.c_str());
	p[5] = atof((char*)D.c_str());
	return p;
}
float *Engine::ParseArcByCenterInstruction(std::string i)
{
	float *p = (float	*)malloc(sizeof(float *) * 3);
	std::size_t cXp = i.find("x");
	std::size_t cYp = i.find("y");
	std::size_t Rp  = i.find("r");

	std::string cX = i.substr(cXp+1, (cYp - cXp) -1);
	std::string cY = i.substr(cYp+1, (Rp - cYp)-1);
	std::string R = i.substr(Rp+1, (i.length() - Rp)-1);

	p[0] = atof((char*)cX.c_str());
	p[1] = atof((char*)cY.c_str());
	p[2] = atof((char*)R.c_str());
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
	float ZoomInc;
	if (ViewRatio < 0.005)
	{
		ZoomInc = 0.0002;
	}
	else
	{
		ZoomInc = 0.001;
	}
	if (ViewRatio > 0.0005)
	{
		ViewRatio = ViewRatio - ZoomInc;
		EntityRedraw = true;
	}
	else
	{
		ViewRatio = .0005;
		EntityRedraw = true;
	}
	//printf("Zoom: %f\n", ViewRatio);
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
float Engine::GetDistance(float x1, float y1, float x2, float y2)
{
	return sqrtf((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
}
float Engine::GetDistance(point p1,point p2)
{
	return sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
}
void Engine::GetRealXY(float out[2], float in[2])
{
	out[0] = ((in[0] / ViewRatio) + OriginOffsetX);
	out[1] = (-1*((in[1] / ViewRatio) - OriginOffsetY));
}
point Engine::GetRealXY(point in)
{
	float input[2];
	input[0] = in.x;
	input[1] = in.y;
	float output[2];
	GetRealXY(output, input);
	point out;
	out.x = output[0];
	out.y = output[1];
	return out;
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
float Engine::GetX(float x)
{
	float in[2];
	in[0] = x;
	in[1] = 0;
	float out[2];
	GetXY(out, in);
	return out[0];
}
float Engine::GetY(float y)
{
	float in[2];
	in[0] = 0;
	in[1] = y;
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
	config->Color((char*)config->LineColor);

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
void Engine::Arc(arc data)
{
	if (data.type == CIRCLE)
	{
		float pos[2];
		float r_pos[2];
		int centrex=data.center.x;
		int centrey=data.center.y;// centre of circle in pixel coords
		int x = centrex;
		int y = centrey;
		float Radius = data.radius;
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
	else if (data.type == ARC)
	{
		//printf("(ARC) --start: %f, %f --stop: %f, %f --radius: %f\n", data.start.x, data.start.y, data.end.x, data.end.y, data.radius);
		point real_point;
		point screen_point;
		float angle_inc = 0.001f/data.radius;
		float two_pi = 6.283f;
		int NumberOfPoints=0;
		for(float angle=0.0f; angle<= two_pi;angle+=angle_inc)
		{
			NumberOfPoints++;
		}
		int pointsX[NumberOfPoints];
		int pointsY[NumberOfPoints];

		SDL_Texture *texture = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WindowWidth, WindowHeight);
		SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(texture, 255);
		SDL_SetRenderTarget( r, texture );
		SDL_SetRenderDrawColor( r, 0, 0, 0, 0 );

		config->Color((char*)config->LineColor);

		int count = 0;
		bool FoundStartPoint = false;
		bool FoundEndPoint = false;

		if (data.direction == ARC_CW)
		{
			printf("(Arc) Going Clockwise\n");
			for(float angle=6.283f; angle>=0; angle-=angle_inc)
			{
					real_point.x=data.center.x+data.radius*cos(angle);
					real_point.y=data.center.y+data.radius*sin(angle);
					if (isSimilar(real_point.x, data.start.x) && isSimilar(real_point.y, data.start.y))
					{
						printf("(Arc CW) Found Start point at point# %d!\n", count);
						FoundStartPoint = true;
					}
					if (FoundStartPoint == true && isSimilar(real_point.x, data.end.x) && isSimilar(real_point.y, data.end.y))
					{
						printf("(Arc CW) Found End point at point# %d!\n", count);
						FoundEndPoint = true;
					}
					if (FoundStartPoint == true && FoundEndPoint == false)
					{
							screen_point = GetRealXY(real_point);
							pointsX[count] = screen_point.x;
							pointsY[count] = screen_point.y;
							SDL_RenderDrawPoint(r, screen_point.x, screen_point.y);
							//printf("%f, %f\n", screen_point.x, screen_point.y);
					}
					count++;
			}
		}
		else
		{
			printf("(Arc) Going CounterClockwise\n");
			for(float angle=0; angle<=two_pi; angle+=angle_inc)
			{
					real_point.x=data.center.x+data.radius*cos(angle);
					real_point.y=data.center.y+data.radius*sin(angle);
					if (isSimilar(real_point.x, data.start.x) && isSimilar(real_point.y, data.start.y))
					{
						printf("(Arc CCW) Found Start point at point# %d!\n", count);
						FoundStartPoint = true;
					}
					if (FoundStartPoint == true && isSimilar(real_point.x, data.end.x) && isSimilar(real_point.y, data.end.y))
					{
						printf("(Arc CCW) Found End point at point# %d!\n", count);
						FoundEndPoint = true;
					}
					if (FoundStartPoint == true && FoundEndPoint == false)
					{
							screen_point = GetRealXY(real_point);
							pointsX[count] = screen_point.x;
							pointsY[count] = screen_point.y;
							SDL_RenderDrawPoint(r, screen_point.x, screen_point.y);
							//printf("%f, %f\n", screen_point.x, screen_point.y);
					}
					count++;
			}
		}
		SDL_RenderPresent( r );
		SDL_SetRenderTarget( r, NULL );
		AppendEntityArray(texture);
		AppendCurserPoints(pointsX, pointsY, NumberOfPoints);
		if (EntityRedrawWithoutNewInstructions == false)
		{
			std::string Instruction = "ax" + std::to_string(data.start.x) + "y" + std::to_string(data.start.y) + "x" + std::to_string(data.end.x) + "y" + std::to_string(data.end.y) + "r" + std::to_string(data.radius) + "d" + std::to_string(data.direction);
			AppendInstructionArray((char *)Instruction.c_str());
		}
	}
}
circle Engine::GetCircleCenters(point p1,point p2,float radius)
{
	float separation = GetDistance(p1,p2),mirrorDistance;
	circle circles;
	if(separation == 0.0)
	{
		if(radius == 0.0)
		{
			//printf("No circles can be drawn through (%.4f,%.4f)\n",p1.x,p1.y);
			circles.possible = 0;
			return circles;
		}
		else
		{
				//printf("Infinitely many circles can be drawn through (%.4f,%.4f)\n",p1.x,p1.y);
				circles.possible = -1;
				return circles;
		}
	}
	else if(separation == 2*radius)
	{
		circles.possible = 1;
		circles.start.x = p1.x;
		circles.start.y = p1.y;
		circles.end.x = p2.x;
		circles.end.y = p2.y;
		circles.center1.x = (p1.x+p2.x)/2;
		circles.center1.y = (p1.y+p2.y)/2;
		circles.radius = radius;
		//printf("Given points are opposite ends of a diameter of the circle with center (%.4f,%.4f) and radius %.4f\n",(p1.x+p2.x)/2,(p1.y+p2.y)/2,radius);
		return circles;
	}
	else if(separation > 2*radius)
	{
		//printf("Given points are farther away from each other than a diameter of a circle with radius %.4f\n",radius);
		circles.possible = 0;
		return circles;
	}
	else
	{
		circles.possible = 2;
		mirrorDistance =sqrt(pow(radius,2) - pow(separation/2,2));
		//printf("Two circles are possible.\n");
		//printf("Circle C1 with center (%.4f,%.4f), radius %.4f and Circle C2 with center (%.4f,%.4f), radius %.4f\n",(p1.x+p2.x)/2 + mirrorDistance*(p1.y-p2.y)/separation,(p1.y+p2.y)/2 + mirrorDistance*(p2.x-p1.x)/separation,radius,(p1.x+p2.x)/2 - mirrorDistance*(p1.y-p2.y)/separation,(p1.y+p2.y)/2 - mirrorDistance*(p2.x-p1.x)/separation,radius);
		circles.start.x = p1.x;
		circles.start.y = p1.y;
		circles.end.x = p2.x;
		circles.end.y = p2.y;
		circles.center1.x = (p1.x+p2.x)/2 + mirrorDistance*(p1.y-p2.y)/separation;
		circles.center1.y = (p1.y+p2.y)/2 + mirrorDistance*(p2.x-p1.x)/separation;
		circles.center2.x = (p1.x+p2.x)/2 - mirrorDistance*(p1.y-p2.y)/separation;
		circles.center2.y = (p1.y+p2.y)/2 - mirrorDistance*(p2.x-p1.x)/separation;
		circles.radius = radius;
		return circles;

	}
}
float *Engine::GetPointAlongLine(float x1, float y1, float x2, float y2, float len)
{
	float *p = (float	*)malloc(sizeof(float *) * 2);

	float x = x2 - x1;
	float y = y2 - y1;

	float inc = 0.0005f;

	float length = sqrt( x*x + y*y );
	float addx = (x / length) * inc;
	float addy = (y / length) * inc;

	x = x1;
	y = y1;

	for(float i = 0.000f; i < len; i += inc)
	{
	  x += addx;
	  y += addy;
		//printf("%f - %f,%f\n", i, x, y);
	}
	p[0] = x;
	p[1] = y;
	return p;
}
float *Engine::GetPointAlong45Line(float x, float y, float len, int dir)
{
	float StartX = x;
	float StartY = y;
	float *p = (float	*)malloc(sizeof(float *) * 2);
	float i = 0.0000f;
	while(i += 0.0005f)
	{
		if (GetDistance(StartX, StartY, x, y) >= len)
		{
			break;
		}
		if (dir == 2)
		{
			x += 0.0005f;
			y += 0.0005f;
		}
		if (dir == 4)
		{
			x -= 0.0005f;
			y -= 0.0005f;
		}
		if (dir == 1)
		{
			x += 0.0005f;
			y -= 0.0005f;
		}
		if (dir == 3)
		{
			x -= 0.0005f;
			y += 0.0005f;
		}
		//printf("%f\n", i);
	}
	p[0] = x;
	p[1] = y;
	return p;
}
float *Engine::GetPointAlongSlope(float x, float y, float rise, float run, float len, int dir)
{
	float StartX = x;
	float StartY = y;
	float *p = (float	*)malloc(sizeof(float *) * 2);
	float rise_inc = rise*0.0005f;
	float run_inc = run*0.0005f;

	float i = 0.0000f;
	while(i += 0.0005f)
	{
		if (GetDistance(StartX, StartY, x, y) >= len)
		{
			break;
		}
		if (dir == 2)
		{
			x += run_inc;
			y += rise_inc;
		}
		if (dir == 4)
		{
			x -= run_inc;
			y -= rise_inc;
		}
		if (dir == 1)
		{
			x += run_inc;
			y -= rise_inc;
		}
		if (dir == 3)
		{
			x -= run_inc;
			y += rise_inc;
		}
		//printf("%f\n", i);
	}
	p[0] = x;
	p[1] = y;
	return p;
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
						if (i.find("ax") != std::string::npos)
						{
							float *p = ParseArcInstruction(i);
							point start, end;
							start.x = p[0];
							start.y = p[1];
							end.x = p[2];
							end.y = p[3];
							float radius = p[4];
							circle circles = GetCircleCenters(start, end, p[4]);
							if (circles.possible > 0)
							{
								arc arc1;
								arc1.center = circles.center1;
								arc1.start = start;
								arc1.end = end;
								arc1.direction = p[5];
								arc1.radius = radius;
								arc1.type = ARC;
								Arc(arc1);
							}
						}
						else if (i.find("ac") != std::string::npos)
						{
							float *p = ParseArcByCenterInstruction(i);
							arc circle;
							circle.center.x = p[0];
							circle.center.y = p[1];
							circle.radius = p[2];
							circle.type = CIRCLE;
							Arc(circle);
						}
						else if (i.find("l") != std::string::npos)
						{
							float *p = ParseLineInstruction(i);
							//std::cout << "\t>Added Line X1: " + X1 + " Y1: " + Y1 + " X2: " + X2 + " Y2: " + Y2 + "\n";
							float LineStart[2];
							float LineEnd[2];
							LineStart[0] = p[0];
							LineStart[1] = p[1];
							LineEnd[0] = p[2];
							LineEnd[1] = p[3];
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
int Engine::Open()
{
	FILE *fp;
	fp = fopen(config->Filename, "r");
	if (fp == NULL)
	{
	  fprintf(stderr, "!!> Cant\'t open file: %s\n", config->Filename);
	  return 1;
	}
	FreeInstructionArray();
	InitInstructionArray();
	char buf[2048];
  while (fgets (buf, sizeof(buf), fp))
	{
			std::string i = std::string(buf);
			//printf("===>Entity Instruction: %s\n", i.c_str());
			if (i.find("ac") != std::string::npos)
			{
				float *p = ParseArcByCenterInstruction(i);
				arc circle;
				circle.center.x = p[0];
				circle.center.y = p[1];
				circle.radius = p[2];
				circle.type = CIRCLE;
				Arc(circle);
			}
			else if(i.find("ax") != std::string::npos)
			{
					float *p = ParseArcInstruction(i);
					point start, end;
					start.x = p[0];
					start.y = p[1];
					end.x = p[2];
					end.y = p[3];
					float radius = p[4];
					circle circles = GetCircleCenters(start, end, radius);
					arc arc1;
					arc1.center = circles.center1;
					arc1.start = start;
					arc1.end = end;
					arc1.radius = radius;
					arc1.direction = p[5];
					//printf("direction: %f\n", p[5]);
					arc1.type = ARC;
					Arc(arc1);
			}
			else if (i.find("l") != std::string::npos)
			{
				float *p = ParseLineInstruction(i);
				//std::cout << "\t>Added Line X1: " + X1 + " Y1: " + Y1 + " X2: " + X2 + " Y2: " + Y2 + "\n";
				float LineStart[2];
				float LineEnd[2];
				LineStart[0] = p[0];
				LineStart[1] = p[1];
				LineEnd[0] = p[2];
				LineEnd[1] = p[3];
				Line(LineStart, LineEnd);
			}
  }
	EntityRedraw=true;
	fclose(fp);
	return 0;
}
int Engine::Save()
{
	FILE *fp;
	fp = fopen(config->Filename, "w");
	if (fp == NULL)
	{
	  fprintf(stderr, "!!> Cant\'t write file: %s\n", config->Filename);
	  return 1;
	}
	for(int x = 0; x < EntityInstructionLength; x++)
	{
		std::string i = std::string(EntityInstruction[x]);
		//Only save valid entities
		if (i.find("a") != std::string::npos) //Includes all arcs
		{
			fprintf(fp, "%s\n", (char*)i.c_str());
		}
		if (i.find("l") != std::string::npos)
		{
			fprintf(fp, "%s\n", (char*)i.c_str());
		}
	}
	fclose(fp);
	return 0;
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
