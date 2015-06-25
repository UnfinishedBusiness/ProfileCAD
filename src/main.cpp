#include "application.h"
#include "Config.h"
#include "Engine.h"

int ActiveTool;
//Tools
#define NoTool 0
#define LineTool 1
//LineTool
int LineClickStep = 0;
float LineStart[2];
float LineEnd[2];

float MousePos[2];
float RealMousePos[2];

Engine *engine=new Engine("/tmp/Test.cad", MainWindowWidth, MainWindowWidth);

int main (int argc, char** argv)
{
    SDL_Window* window = NULL;
    window = SDL_CreateWindow
    (
        WindowTitle, SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        MainWindowWidth,
        MainWindowHeight,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
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
				if (e.type == SDL_WINDOWEVENT)
				{
					if (e.window.event == SDL_WINDOWEVENT_RESIZED)
					{
						//float OriginOffsetX = (e.window.data1/2);
						//float OriginOffsetY = (e.window.data2/2);
						engine->Pull(renderer);
						SDL_RenderPresent( renderer );
					}
				}
				if (e.type == SDL_KEYUP)
				{
					//quit = true;
					if (e.key.keysym.scancode == SDL_SCANCODE_S)
					{
							engine->Save();
					}
					if (e.key.keysym.scancode == SDL_SCANCODE_O)
					{
							engine->Open(renderer);
					}
					if (e.key.keysym.scancode == SDL_SCANCODE_L)
					{
						printf("==> Line Tool\n");
						if (ActiveTool == NoTool)
						{
								ActiveTool = LineTool;
						}
						else
						{
							ActiveTool = NoTool;
						}

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
				if (e.type == SDL_MOUSEMOTION)
				{
					engine->GetMousePos(MousePos);
					engine->GetRealXY(RealMousePos, MousePos);
					printf("\rX: %lf, Y: %lf |||||| rX: %lf, rY: %lf\0", MousePos[0], MousePos[1], RealMousePos[0], RealMousePos[1]);
				}
				if (e.type == SDL_MOUSEBUTTONUP)
				{
					if (e.button.button == SDL_BUTTON_X1)
					{
						printf("\r> ZoomIn ++ %f\b\b\b\b",engine->ZoomIn());
						printf("====> Rerendering Screen!\n");
						engine->Pull(renderer);
					}
					if (e.button.button == SDL_BUTTON_X2)
					{
						printf("\r> ZoomOut -- %f\b\b\b\b", engine->ZoomOut());
						printf("====> Rerendering Screen!\n");
						engine->Pull(renderer);
					}
					if (e.button.button == SDL_BUTTON_LEFT)
					{
						printf("=====> Left Click!\n");
						if (ActiveTool == LineTool)
						{
							if (LineClickStep == 1)
							{
								engine->GetMousePos(LineEnd);
								engine->Line(renderer, LineStart, LineEnd);
								printf("\t> Line End ==== x: %lf y: %lf\n", engine->GetX(LineStart), engine->GetY(LineEnd));
								LineClickStep = 0;
							}
							else if (LineClickStep == 0)
							{
								engine->GetMousePos(LineStart);
								LineClickStep = 1;
								printf("\t> Line Start ==== x: %lf y: %lf\n", (float) engine->GetX(LineStart), (float) engine->GetY(LineEnd));
							}
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
