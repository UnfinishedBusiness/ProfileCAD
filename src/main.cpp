#include "application.h"
#include "Config.h"
#include "Engine.h"

float MouseX, MouseY;
int ActiveTool;
//Tools
#define NoTool 0
#define LineTool 1
//LineTool
int LineClickStep = 0;
float LineStart[2];
float LineEnd[2];

Engine *engine=new Engine("/tmp/Test.cad");

void UpdateMouse()
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	MouseX = (x - OriginOffsetX);
	MouseY = ((y - OriginOffsetY)/-1);
}
int main (int argc, char** argv)
{
    SDL_Window* window = NULL;
    window = SDL_CreateWindow
    (
        WindowTitle, SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WindowWidth,
        WindowHeight,
        SDL_WINDOW_SHOWN
    );

    // Setup renderer
    SDL_Renderer* renderer = NULL;
		SDL_Renderer* screen = NULL;
    renderer =  SDL_CreateRenderer( window, 1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

		Config *config = new Config(renderer);
    config->ColorBlack();
		engine->Pull(renderer);
		SDL_RenderPresent( renderer );
    SDL_Event e;
		bool quit = false;
		while (!quit)
		{
			while (SDL_PollEvent(&e))
			{
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}
				if (e.type == SDL_KEYUP)
				{
					//quit = true;
					if (e.key.keysym.scancode == SDL_SCANCODE_L)
					{
						printf("==> Line Tool\n");
						ActiveTool = LineTool;
					}
					if (e.key.keysym.scancode == SDL_SCANCODE_TAB)
					{

						printf("====> Rerendering Screen!\n");
						engine->Pull(renderer);
						SDL_RenderPresent( renderer );
					}
					if (e.key.keysym.scancode == SDL_SCANCODE_BACKSPACE)
					{
						config->ColorBlack();
						SDL_RenderClear(renderer);
						SDL_RenderPresent( renderer );
						engine->Trash();
						printf("====> Deleted current changes!\n");
					}
					if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
					{
						printf("====> Bye!\n");
						quit = true;
					}
				}
				if (e.type == SDL_MOUSEBUTTONUP)
				{
					UpdateMouse();
					if (ActiveTool == LineTool)
					{
						if (LineClickStep == 1)
						{
							//LineEnd[0] = (MouseX + OriginOffsetX);
							//LineEnd[1] = (-1*(MouseY - OriginOffsetY));
							LineEnd[0] = MouseX;
							LineEnd[1] = MouseY;
							//ActiveTool = NoTool;

							//SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0 );
							//SDL_RenderDrawLine(renderer, LineStart[0], LineStart[1], LineEnd[0], LineEnd[1]);
							engine->Line(renderer, LineStart, LineEnd);
							SDL_RenderPresent( renderer );
							printf("\t> Line End ==== x: %lf y: %lf\n", MouseX, MouseY);
							LineClickStep = 0;
							//MouseX = (x - OriginOffsetX);
							//MouseY = ((y - OriginOffsetY)/-1);
						}
						else if (LineClickStep == 0)
						{
							//LineStart[0] = (MouseX + OriginOffsetX);
							//LineStart[1] = (-1*(MouseY - OriginOffsetY));
							LineStart[0] = MouseX;
							LineStart[1] = MouseY;
							LineClickStep = 1;
							printf("\t> Line Start ==== x: %lf y: %lf\n", MouseX, MouseY);
						}
					}

				}
			//printf("Physical %s key acting as %s key\n", SDL_GetScancodeName(e.key.keysym.scancode), SDL_GetKeyName(e.key.keysym.sym));
			}
      //SDL_RenderPresent( renderer );
			fflush(stdout);
		}
		SDL_RenderClear(renderer);
		SDL_RenderClear(screen);
		SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
