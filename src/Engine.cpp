#include "Engine.h"

Engine::Engine(char *File, int _WindowWidth, int _WindowHeight)
{
	//memcpy(Filename, File, sizeof(*File));
	Entitys = NULL;
	Filename=File;
	ViewRatio = 0.05;
	WindowWidth = _WindowWidth;
	WindowHeight = _WindowHeight;
	OriginOffsetX = (_WindowWidth/2); //Center Origin
	OriginOffsetY = (_WindowHeight/2); //Center Origin

	LineColor = "White";
	printf("==> Writing to %s\n", Filename);
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
}
void Engine::PanIncY(float p)
{
	OriginOffsetY = OriginOffsetY + p;
}
float Engine::ZoomIn()
{
	ViewRatio = ViewRatio - .001;
	return ViewRatio;
}
float Engine::ZoomOut()
{
	ViewRatio = ViewRatio + .001;
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
void Engine::Trash()
{
	while (Entitys != NULL)
	{
		Entitys = g_slist_next(Entitys);
	}
}
void Engine::Save()
{
	FILE *fp;
	fp = fopen(Filename, "w");
	if (fp == NULL)
	{
		printf("!!!! ==> Can't open %s\n", Filename);
	}
	else
	{
		GSList *tmp = Entitys;
		char *line;
		while (tmp != NULL)
	  {
			line = (char*) tmp->data;
	    printf("===> Saving %s\n", line);
			fprintf(fp, "%s\n", line);
	    tmp = g_slist_next(tmp);
		}
	}
	fclose(fp);
}
void Engine::Push(char *line)
{
	gchar* element = g_strdup(line);
	Entitys = g_slist_append(Entitys, element);
}
void Engine::Pull(SDL_Renderer *r)
{
	Config *config = new Config(r);
	config->ColorBlack();
	SDL_RenderClear(r);
	GSList *tmp = Entitys;
	char *line;
	while (tmp != NULL)
  {
		line = (char*) tmp->data;
    printf("===> Pulling %s\n", line);
    tmp = g_slist_next(tmp);
		//line[strlen(line) - 1] = '\0';
		if (strcmp(GetField(line, 1), "Line") == 0)
		{
			float Start[2];
			float End[2];
			Start[0] = atof(GetField(line, 2));
			Start[1] = atof(GetField(line, 3));
			End[0] = atof(GetField(line, 4));
			End[1] = atof(GetField(line, 5));
			LineColor = (char *)GetField(line, 6);
			printf("Pulling ===> Line (%lf, %lf) ---- (%lf, %lf) %s\n", Start[0], Start[1], End[0], End[1], LineColor);

			float screen_point1[2];
			float screen_point2[2];

			GetRealXY(screen_point1, Start);
			GetRealXY(screen_point2, End);

			config->Color(LineColor);
			SDL_RenderDrawLine(r, screen_point1[0], screen_point1[1], screen_point2[0], screen_point2[1]);
			SDL_RenderPresent( r );
		}
  }
}
void Engine::Open(SDL_Renderer* r)
{
	Config *config = new Config(r);
	config->ColorBlack();
	SDL_RenderClear(r);
	Trash();

	char line[2048];
	FILE *fp;
	fp = fopen(Filename, "r");
	if (fp == NULL)
	{
		printf("!!!! ==> Can't open %s\n", Filename);
	}
	else
	{
    while ( fgets ( line, sizeof(line), fp ) != NULL )
    {
			line[strlen(line) - 1] = '\0';
			Push(line);
    }
	}
	fclose(fp);
	Pull(r);
}
void Engine::Line(SDL_Renderer* r, float Start[2], float End[2])
{
	Config *config = new Config(r);
	char str[2048];
	sprintf(str, "Line:%lf:%lf:%lf:%lf:%s", Start[0], Start[1], End[0], End[1], LineColor);
	Push(str);

	float screen_point1[2];
	float screen_point2[2];

	GetRealXY(screen_point1, Start);
	GetRealXY(screen_point2, End);

	config->ColorWhite();
	SDL_RenderDrawLine(r, screen_point1[0], screen_point1[1], screen_point2[0], screen_point2[1]);
	SDL_RenderPresent( r );
}
const char* Engine::GetField(char* line, int num)
{
    const char* tok;
		char *str = strdup(line);
    for (tok = strtok(str, ":");
            tok && *tok;
            tok = strtok(NULL, ":\n"))
    {
        if (!--num)
            return tok;
    }
    return "";
}
