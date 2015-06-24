#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <iostream>
#include "application.h"
#include "Config.h"

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
    renderer =  SDL_CreateRenderer( window, 0, SDL_RENDERER_ACCELERATED);

    // Set render color to red ( background will be rendered in this color )
    SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0 );

    // Clear winow
    SDL_RenderClear( renderer );

    // Creat a rect at pos ( 50, 50 ) that's 50 pixels wide and 50 pixels high.
    //SDL_Rect r;
    //r.x = 50;
    //r.y = 50;
    //r.w = 50;
    //r.h = 50;

    // Set render color to blue ( rect will be rendered in this color )
	//SDL_SetRenderDrawColor( renderer, 0, 255, 255, 255 );

    // Render rect
    //SDL_RenderFillRect( renderer, &r );

    // Render the rect to the screen
    SDL_RenderPresent(renderer);

    SDL_Event e;
	bool quit = false;
	while (!quit){
		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT){
				quit = true;
			}
			if (e.type == SDL_KEYUP){
				//quit = true;
				if (e.key.keysym.scancode == SDL_SCANCODE_L)
				{
					printf("==> Line Tool\n");
					ActiveTool = LineTool;
				}
			}
			if (e.type == SDL_MOUSEBUTTONUP)
			{
				UpdateMouse();
				if (ActiveTool == LineTool)
				{
					if (LineClickStep == 1)
					{	
						LineEnd[0] = (MouseX + OriginOffsetX);
						LineEnd[1] = (-1*(MouseY - OriginOffsetY));
						ActiveTool = NoTool;
						SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0 );
						SDL_RenderDrawLine(renderer, LineStart[0], LineStart[1], LineEnd[0], LineEnd[1]);
						printf("\t> Line End ==== x: %lf y: %lf\n", LineEnd[0], LineEnd[1]);
						LineClickStep = 0;
						//MouseX = (x - OriginOffsetX);
						//MouseY = ((y - OriginOffsetY)/-1);
					}
					else if (LineClickStep == 0)
					{
						LineStart[0] = (MouseX + OriginOffsetX);
						LineStart[1] = (-1*(MouseY - OriginOffsetY));
						LineClickStep = 1;
						printf("\t> Line Start ==== x: %lf y: %lf\n", LineStart[0], LineStart[1]);
					}
				}
				
			}
			//printf("Physical %s key acting as %s key\n", SDL_GetScancodeName(e.key.keysym.scancode), SDL_GetKeyName(e.key.keysym.sym));
		}
		//Render the scene
		//SDL_RenderClear(renderer);
		//renderTexture(image, renderer, x, y);
		//UpdateMouse();
		//printf("\rX: %lf, Y: %lf\b\b\b\b\b\b\b", MouseX, MouseY);
		SDL_RenderPresent(renderer);
		
		fflush(stdout); 
	}

    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
