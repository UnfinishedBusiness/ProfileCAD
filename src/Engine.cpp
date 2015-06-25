#include "Engine.h"

Engine::Engine(char *File)
{
	//memcpy(Filename, File, sizeof(*File));
	Filename=File;
	FirstWrite = false;
	ViewRatio = 0.05;
	LineColor = "White";
	printf("==> Writing to %s\n", Filename);
}
void Engine::Trash()
{
	FirstWrite = true;
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
void Engine::Pull(SDL_Renderer* r)
{
	Config *config = new Config(r);
	config->ColorBlack();
	SDL_RenderClear(r);
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
				screen_point1[0] = (Start[0] + OriginOffsetX);
				screen_point1[1] = (-1*(Start[1] - OriginOffsetY));

				screen_point2[0] = (End[0] + OriginOffsetX);
				screen_point2[1] = (-1*(End[1] - OriginOffsetY));

				config->Color(LineColor);
				SDL_RenderDrawLine(r, screen_point1[0], screen_point1[1], screen_point2[0], screen_point2[1]);
			}
			//printf(" Pull ===> Read: %s, Field 2: %s\n", line, GetField(line, 2));
    }
	}
	fclose(fp);

	//SDL_RenderDrawLine(r, 0, 0, 0, 0);
}
void Engine::Line(SDL_Renderer* r, float Start[2], float End[2])
{
	Config *config = new Config(r);
	char str[2048];
	sprintf(str, "Line:%lf:%lf:%lf:%lf:%s", Start[0], Start[1], End[0], End[1], LineColor);
	Push(str);

	float screen_point1[2];
	float screen_point2[2];

	screen_point1[0] = (Start[0] + OriginOffsetX);
	screen_point1[1] = (-1*(Start[1] - OriginOffsetY));

	screen_point2[0] = (End[0] + OriginOffsetX);
	screen_point2[1] = (-1*(End[1] - OriginOffsetY));

	config->ColorWhite();
	SDL_RenderDrawLine(r, screen_point1[0], screen_point1[1], screen_point2[0], screen_point2[1]);
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
