#include "Config.h"

Config::Config(SDL_Renderer* r)
{
    renderer = r;
    //struct passwd *pw = getpwuid(getuid());
    //homedir = pw->pw_dir;
    //std::string cf(homedir);
    ConfigFile = (char *)"Config.profile";
    //cf.append(ConfigFile);
    ParseConfig();
}
void Config::ParseConfig()
{
  std::size_t start;
  std::size_t stop;

  //Defaults
  Font = (char *)"Fonts/monaco.ttf";
  Filename = (char *)"/tmp/Test.cad";
  LineColor = (char *)"White";
  BackgroundColor = (char *)"Black";

  FILE *fp;
  char l[2048];
  std::string line = "";
  std::string data = "";
  std::string key = "";
  fp = fopen(ConfigFile, "r");
  if (fp == NULL)
  {
    printf("!!!! ==> Can't open %s\nUsing default values!\n", ConfigFile);
  }
  else
  {
    while ( fgets ( l, sizeof(l), fp ) != NULL )
    {
      l[strlen(l) - 1] = '\0'; //Strip new line
      line.assign(l, strlen(l));
      printf("Config: %s\n", (char *)line.c_str());
      key = "BackgroundColor";
      if (line.find(key) != std::string::npos)
      {
          start = line.find(key);
          stop = line.find('\n');
          data = line.substr(start+key.length()+2, (stop - start));
          data = trim(data);
          BackgroundColor = strdup(data.c_str());
          printf("=> Set Background to %s\n", BackgroundColor);
      }
    }
    fclose(fp);
  }
}
void Config::SaveConfig()
{
	FILE *fp;
  printf("==> Config path %s\n", ConfigFile);
	fp = fopen(ConfigFile, "w");
	if (fp == NULL)
	{
		printf("!!!! ==> Can't write %s\n", ConfigFile);
	}
	else
	{
    fprintf(fp, "Filename: %s\n", Filename);
    fprintf(fp, "Font: %s\n", Font);
		fprintf(fp, "BackgroundColor: %s\n", BackgroundColor);
    fprintf(fp, "WindowHeight: %d\n", WindowHeight);
    fprintf(fp, "WindowWidth: %d\n", WindowWidth);
		fclose(fp);
	}
}
void Config::Color(char *color)
{
  SDL_Color tmp = ColorWhite;
  SDL_SetRenderDrawColor( renderer, tmp.r, tmp.g, tmp.b, tmp.a ); //White Default
  if (strcmp(color, "White") == 0)
  {
      tmp = ColorWhite;
      SDL_SetRenderDrawColor( renderer, tmp.r, tmp.g, tmp.b, tmp.a );
  }
  if (strcmp(color, "Black") == 0)
  {
      tmp = ColorBlack;
      SDL_SetRenderDrawColor( renderer, tmp.r, tmp.g, tmp.b, tmp.a );
  }
  if (strcmp(color, "Red") == 0)
  {
      tmp = ColorRed;
      SDL_SetRenderDrawColor( renderer, tmp.r, tmp.g, tmp.b, tmp.a );
  }
  if (strcmp(color, "Green") == 0)
  {
      tmp = ColorGreen;
      SDL_SetRenderDrawColor( renderer, tmp.r, tmp.g, tmp.b, tmp.a );
  }
  if (strcmp(color, "Blue") == 0)
  {
      tmp = ColorBlue;
      SDL_SetRenderDrawColor( renderer, tmp.r, tmp.g, tmp.b, tmp.a );
  }
  if (strcmp(color, "Yellow") == 0)
  {
      tmp = ColorYellow;
      SDL_SetRenderDrawColor( renderer, tmp.r, tmp.g, tmp.b, tmp.a );
  }
  if (strcmp(color, "Purple") == 0)
  {
      tmp = ColorPurple;
      SDL_SetRenderDrawColor( renderer, tmp.r, tmp.g, tmp.b, tmp.a );
  }
}
