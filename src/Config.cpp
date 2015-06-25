#include "Config.h"

Config::Config(SDL_Renderer* r)
{
    renderer = r;
}
void Config::ColorBlack()
{
  SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
}
void Config::ColorWhite()
{
  SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
}
void Config::Color(char *color)
{
  SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 ); //White Default
  if (strcmp(color, "White") == 0)
    SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
  if (strcmp(color, "Black") == 0)
    SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0 );
  if (strcmp(color, "Red") == 0)
    SDL_SetRenderDrawColor( renderer, 255, 0, 0, 0 );
  if (strcmp(color, "Green") == 0)
    SDL_SetRenderDrawColor( renderer, 0, 255, 0, 0 );
  if (strcmp(color, "Blue") == 0)
    SDL_SetRenderDrawColor( renderer, 0, 0, 255, 0 );
}
