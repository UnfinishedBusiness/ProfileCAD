#include "Engine.h"

Engine::Engine(char *File)
{
	//memcpy(Filename, File, sizeof(*File));
	Filename=File;
	FirstWrite = true;
	printf("==> Writing to %s\n", Filename); 
}
void Engine::Push(char *line)
{
	FILE *fp;
	if (FirstWrite)
	{
		FirstWrite = false;
		fp = fopen(Filename, "w");
	}
	else
	{
		fp = fopen(Filename, "a");
	}
	 
	if (fp == NULL)
	{
		printf("!!!! ==> Can't open %s\n", Filename);
	}
	else
	{
		fprintf(fp, "%s\n", line);
	}
	fclose(fp);
}
void Engine::Line(SDL_Renderer* r, float Start[2], float End[2])
{
	char str[2048];
	sprintf(str, "Line:%lf:%lf:%lf:%lf", Start[0], Start[1], End[0], End[1]);
	Push(str);

	float screen_point1[2];
	float screen_point2[2];
	
	screen_point1[0] = (Start[0] + OriginOffsetX);
	screen_point1[1] = (-1*(Start[1] - OriginOffsetY));
	
	screen_point2[0] = (End[0] + OriginOffsetX);
	screen_point2[1] = (-1*(End[1] - OriginOffsetY));
	
	SDL_SetRenderDrawColor( r, 0, 0, 0, 0 );
	SDL_RenderDrawLine(r, screen_point1[0], screen_point1[1], screen_point2[0], screen_point2[1]);
}
