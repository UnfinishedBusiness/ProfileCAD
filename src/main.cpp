#include "application.h"
#include "Config.h"
#include "Engine.h"

int ActiveTool;
//Tools
#define NoTool 0
#define LineTool 1
//LineTool
int LastEntityClicked = 0;
int LineClickStep = 0;
float LineStart[2];
float LineEnd[2];
float TmpPos[2];
float MousePos[2];
float RealMousePos[2];
float LastMouseScrollPos[2];
float LastRealMousePos[2];
float PanDistance[2];
float VirtualOrigin[2]; //0,0 in Real Cordinant plane
char XYBuff[100];
char ResizeBuff[100];
std::string MsgBuff = "";
std::string inputText = "";
Engine *engine=NULL;
Config *config=NULL;
time_t MsgTimer=NULL;
int MsgDuration = 1500; //ms
double timer_diff_ms;

int main (int argc, char** argv)
{
    if (TTF_Init() != 0)
    {
      printf("SDL_ttf Error!\n");
      SDL_Quit();
      return 1;
    }
    SDL_Window* window = NULL;
    window = SDL_CreateWindow
    (
        WindowTitle, SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        DefaultWindowWidth,
        DefaultWindowHeight,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    SDL_Renderer* renderer = NULL;
    SDL_Renderer* screen = NULL;
    renderer =  SDL_CreateRenderer( window, 1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);

		config = new Config(window, renderer);
		engine = new Engine(window, renderer, config);

    SDL_Event e;
		bool quit = false;
		while (!quit)
		{
      engine->GetMousePos(MousePos);
      engine->GetRealXY(RealMousePos, MousePos);
      sprintf(XYBuff, "X: %lf, Y: %lf, rX: %lf, rY: %lf", MousePos[0], MousePos[1], RealMousePos[0], RealMousePos[1]);

      SDL_StartTextInput();
      //SDL_StopTextInput();
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
						engine->UpdateWindowSize(e.window.data1, e.window.data2);
            //sprintf(ResizeBuff, "H: %d, W: %d", e.window.data1, e.window.data2);
            //engine->PutTexture(engine->MakeText(ResizeBuff, 8), 10, 10);
					}
				}
        if (e.type == SDL_TEXTINPUT)
        {
          if( !( ( e.text.text[ 0 ] == 'c' || e.text.text[ 0 ] == 'C' ) && ( e.text.text[ 0 ] == 'v' || e.text.text[ 0 ] == 'V' ) && SDL_GetModState() & KMOD_CTRL ) )
          {
                if (inputText == "")
                {
                  inputText = ":";
                }
                inputText += e.text.text;
          }
        }
				if (e.type == SDL_KEYDOWN)
				{
          if( e.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0 )
          {
            //lop off character]
            if (inputText != ":")
            {
              inputText.pop_back();
            }
            if (inputText == ":")
            {
              inputText = "";
            }
          }
          else if( e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL )
          {
            SDL_SetClipboardText( inputText.c_str() );
          }
          //Handle paste
          else if( e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL )
          {
            inputText = SDL_GetClipboardText();
          }
          if (e.key.keysym.scancode == SDL_SCANCODE_UP)
					{
							engine->PanIncY(-10);

					}
					if (e.key.keysym.scancode == SDL_SCANCODE_DOWN)
					{
							engine->PanIncY(+10);

					}
					if (e.key.keysym.scancode == SDL_SCANCODE_LEFT)
					{
							engine->PanIncX(-10);

					}
					if (e.key.keysym.scancode == SDL_SCANCODE_RIGHT)
					{
							engine->PanIncX(+10);
					}
          if (e.key.keysym.scancode == SDL_SCANCODE_RETURN)
					{
							//printf("Input: %s\n", inputText.c_str());
              if (inputText.find(":save") != std::string::npos)
							{
								engine->Save();
								MsgBuff = ">Saving " + std::string(config->Filename);
							}
              if (inputText.find(":open") != std::string::npos)
							{
								engine->Open();
								MsgBuff = ">Opening " + std::string(config->Filename);
							}
							if (inputText.find(":file") != std::string::npos)
							{
								std::string file = trim(inputText.substr(5, inputText.length()));
								config->Filename = strdup(file.c_str());
								engine->Open();
								MsgBuff = ">Opened " + file;
							}
              if (inputText.find(":rm") != std::string::npos)
							{
                engine->EntityInstruction[LastEntityClicked][0] = '\0';
                MsgBuff = "> Removed Entity ID#" + std::to_string(LastEntityClicked);
                engine->EntityRedraw = true;
              }
              if (inputText.find(":gll") != std::string::npos)
							{
                std::string Entity = std::string(engine->EntityInstruction[LastEntityClicked]);
                float *p = engine->ParseLineInstruction(Entity);
                MsgBuff = "> Line is " + std::to_string(engine->GetDistance(p[0], p[1], p[2], p[3])) + " units long";

              }
              if (inputText.find(":ac") != std::string::npos)
							{
                float *p = engine->ParseArcByCenterInstruction(inputText.erase(0, 1));
                MsgBuff = ">Drew Arc cX: " + std::to_string(p[0]) + " cY: " + std::to_string(p[1]) + " R: " + std::to_string(p[2]);
                engine->ArcByCenter(p[0], p[1], p[2]);
              }
              if (inputText.find(":lp") != std::string::npos && std::string::npos && inputText.find("d"))
							{
                std::size_t posD = inputText.find("d");
                std::string Distance = inputText.substr(posD+1, (inputText.length() - posD)-1);
                MsgBuff = ">Draw line parallel to line# " + std::to_string(LastEntityClicked) + " with distance " + Distance;

                std::string Entity = std::string(engine->EntityInstruction[LastEntityClicked]);
                //MsgBuff = Entity;
                float *p = engine->ParseLineInstruction(Entity);
                float real_d = atof((char*)Distance.c_str());
                //MsgBuff = "Slope is " + std::to_string((p[3]-p[1])) + "/" + std::to_string((p[2] - p[0]));
                float x1, y1, x2, y2, b, x, y, NewX, NewY;
                x1 = p[0];
                y1 = p[1];
                x2 = p[2];
                y2 = p[3];
                float slope = ((y2 - y1) / (x2 - x1));
                float slopeInverse =  ((x2 - x1) / (y2 - y1));
                float Start[2];
                float End[2];

                //float d = (real_d / sqrtf(1 + ((slopeInverse) * (slopeInverse))));
                if (x1 == x2)
                {
                  Start[0] = x1 + real_d;
                  Start[1] = y1;
                  End[0] = x2 + real_d;
                  End[1] = y2;
                  engine->Line(Start, End);
                }
                else if (y1 == y2)
                {
                  Start[0] = x1;
                  Start[1] = y1 + real_d;
                  End[0] = x2;
                  End[1] = y2 + real_d;
                  engine->Line(Start, End);
                }
                else
                {
                  float line_length = engine->GetDistance(x1, y1, x2, y2);
                  float Xmidpoint = ((x1+x2)/2);
                  float Ymidpoint = ((y1+y2)/2);
                  b = ((slopeInverse*Xmidpoint)-Ymidpoint);
                  if (slopeInverse == 1)
                  {
                    printf("Slope is 1, using alt method, Distance: %f, Length: %f\n", real_d, line_length);
                    if (y1 > 0 || y2 > 0)
                    {
                      if (real_d > 0)
                      {
                        NewX = (Xmidpoint) - (Xmidpoint);
                        NewY = (Ymidpoint) + (Ymidpoint);
                      }
                      else
                      {
                        NewX = (Xmidpoint) + (Xmidpoint);
                        NewY = (Ymidpoint) - (Ymidpoint);
                      }
                    }
                    else
                    {
                      if (real_d > 0)
                      {
                        NewX = (Xmidpoint) + (Xmidpoint);
                        NewY = (Ymidpoint) - (Ymidpoint);
                      }
                      else
                      {
                        NewX = (Xmidpoint) - (Xmidpoint);
                        NewY = (Ymidpoint) + (Ymidpoint);
                      }
                    }
                    float *PerpEnd = engine->GetPointAlongLine(Xmidpoint, Ymidpoint, NewX, NewY, fabs(real_d));
                    Start[0] = Xmidpoint;
                    Start[1] = Ymidpoint;
                    //float End[2];
                    //engine->Line(Start, PerpEnd);
                    float *NewEnd = engine->GetPointAlong45Line(PerpEnd[0], PerpEnd[1], (line_length/2), 2);
                    float *NewStart = engine->GetPointAlong45Line(PerpEnd[0], PerpEnd[1], (line_length/2), 4);
                    engine->Line(NewStart, NewEnd);
                  }
                  else if(slopeInverse == -1)
                  {
                    printf("Slope is -1, using alt method, Distance: %f, Length: %f\n", real_d, line_length);
                    if (y1 > 0 || y2 > 0)
                    {
                      if (real_d < 0)
                      {
                        NewX = (Xmidpoint) + (Xmidpoint);
                        NewY = (Ymidpoint) - (Ymidpoint);
                      }
                      else
                      {
                        NewX = (Xmidpoint) - (Xmidpoint);
                        NewY = (Ymidpoint) + (Ymidpoint);
                      }
                    }
                    else
                    {
                      if (real_d < 0)
                      {
                        NewX = (Xmidpoint) - (Xmidpoint);
                        NewY = (Ymidpoint) + (Ymidpoint);
                      }
                      else
                      {
                        NewX = (Xmidpoint) + (Xmidpoint);
                        NewY = (Ymidpoint) - (Ymidpoint);
                      }
                    }
                    float *PerpEnd = engine->GetPointAlongLine(Xmidpoint, Ymidpoint, NewX, NewY, fabs(real_d));
                    Start[0] = Xmidpoint;
                    Start[1] = Ymidpoint;
                    float *NewEnd = engine->GetPointAlong45Line(PerpEnd[0], PerpEnd[1], (line_length/2), 1);
                    float *NewStart = engine->GetPointAlong45Line(PerpEnd[0], PerpEnd[1], (line_length/2), 3);
                    engine->Line(NewStart, NewEnd);
                  }
                  else
                  {
                    //printf("Xmidpoint=%f, Ymidpoint=%f, b=%f slope=%f, slopeInverse=%f\n",Xmidpoint, Ymidpoint, b, slope, slopeInverse);
                    //NewX = (Ymidpoint-b)/slopeInverse;
                    //NewY = (slopeInverse*Xmidpoint) + b;
                    float *PerpEnd;
                    if (y1 > 0 || y2 > 0)
                    {
                      if (real_d > 0)
                      {
                        PerpEnd = engine->GetPointAlongSlope(Xmidpoint, Ymidpoint, (x2 - x1), (y2 - y1), fabs(real_d), 1);
                      }
                      else
                      {
                        PerpEnd = engine->GetPointAlongSlope(Xmidpoint, Ymidpoint, (x2 - x1), (y2 - y1), fabs(real_d), 3);
                      }
                    }
                    else
                    {
                      if (real_d > 0)
                      {
                        PerpEnd = engine->GetPointAlongSlope(Xmidpoint, Ymidpoint, (x2 - x1), (y2 - y1), fabs(real_d), 3);
                      }
                      else
                      {
                        PerpEnd = engine->GetPointAlongSlope(Xmidpoint, Ymidpoint, (x2 - x1), (y2 - y1), fabs(real_d), 1);
                      }
                    }
                    float *finalLineStart = engine->GetPointAlongSlope(PerpEnd[0], PerpEnd[1], (y2 - y1), (x2 - x1), (line_length/2), 2);
                    float *finalLineEnd = engine->GetPointAlongSlope(PerpEnd[0], PerpEnd[1], (y2 - y1), (x2 - x1), (line_length/2), 4);
                    engine->Line(finalLineStart, finalLineEnd);
                  }
                }
                //MsgBuff = ">Line X1" + std::to_string(Start[0]) + " Y1: " + std::to_string(Start[1]) + " X2: " + std::to_string(End[0]) + " Y2: " + std::to_string(End[1]);
              }
              if (inputText.find(":lh") != std::string::npos)
							{
                MsgBuff = ">Line Horizontal: ";
              }
              if (inputText.find(":lv") != std::string::npos)
							{
                MsgBuff = ">Line verticle: ";
              }
							if (inputText.find(":l") != std::string::npos && inputText.find("x") != std::string::npos && inputText.find("y") != std::string::npos)
							{
                float *p = engine->ParseLineInstruction(inputText.erase(0, 1)); //Erase dirty ':'
								std::string X1 = std::to_string(p[0]);
								std::string Y1 = std::to_string(p[1]);
								std::string X2 = std::to_string(p[2]);
								std::string Y2 = std::to_string(p[3]);

								if (X1 != inputText && X2 != inputText && Y1 != inputText && Y2 != inputText)
								{
									LineStart[0] = atof((char*)X1.c_str());
									LineStart[1] = atof((char*)Y1.c_str());
									LineEnd[0] = atof((char*)X2.c_str());
									LineEnd[1] = atof((char*)Y2.c_str());
									engine->Line(LineStart, LineEnd);
									MsgBuff = ">Drew Line X1: " + X1 + " Y1: " + Y1 + " X2: " + X2 + " Y2: " + Y2;
								}
								else if (X2 == inputText && Y2 == inputText)
								{
									LineStart[0] = atof((char*)X1.c_str());
									LineStart[1] = atof((char*)Y1.c_str());
									LineClickStep = 1;
									ActiveTool = LineTool;
									MsgBuff = ">Added Line X1: " + X1 + " Y1: " + Y1 + " => Click at Endpoint";
								}
								else if (ActiveTool == NoTool)
								{
										MsgBuff = ">Line Tool Active";
										ActiveTool = LineTool;
								}
								else
								{
									MsgBuff = ">Line Tool Inctive";
									ActiveTool = NoTool;
								}

							}
              inputText="";
					}
					if (e.key.keysym.scancode == SDL_SCANCODE_S && SDL_GetModState() & KMOD_CTRL)
					{
							//engine->Save();
					}
					if (e.key.keysym.scancode == SDL_SCANCODE_O && SDL_GetModState() & KMOD_CTRL)
					{
							//engine->Open();
					}
					if (e.key.keysym.sym == SDLK_n && SDL_GetModState() & KMOD_CTRL)
					{
						//config->Color((char *)"Black");
						//SDL_RenderClear(renderer);
						//engine->Trash();
						//printf("====> Deleted current changes!\n");
					}
					if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
					{
						printf("====> Bye!\n");
						quit = true;
					}
				}
				if (e.type == SDL_MOUSEWHEEL)
				{

          if (e.wheel.y > 0)
          {
              //printf("> ZoomIn ++ %f\n", engine->ZoomIn());
              engine->ZoomIn();
          }
          else
          {
              //printf("> ZoomOut ++ %f\n", engine->ZoomOut());
              engine->ZoomOut();
          }
					//printf("\rX: %lf, Y: %lf |||||| rX: %lf, rY: %lf\0", MousePos[0], MousePos[1], RealMousePos[0], RealMousePos[1]);
				}
				if (e.type == SDL_MOUSEBUTTONUP)
				{
					if (e.button.button == SDL_BUTTON_LEFT)
					{
						printf("=====> Left Click!\n");
            int id = engine->GetCurserOverId(); //Remember that this resets the id buffer!
            if (id != -1)
            {
              MsgBuff = ">Operate on entity id# " + std::to_string(id) + " with instruction " + std::string(engine->EntityInstruction[id]);
              LastEntityClicked = id;
            }
						if (ActiveTool == LineTool)
						{
							if (LineClickStep == 1)
							{
								engine->GetMousePos(LineEnd);
								engine->Line(LineStart, LineEnd);
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
    //Begin Render -- quick in, quick out!
    config->Color(config->BackgroundColor); //Background color
    SDL_RenderClear(renderer);
    engine->PutTexture(engine->MakeText(XYBuff, 8), 10, 10);
    timer_diff_ms = difftime(time(0), MsgTimer) * 1000.;
    //if (timer_diff_ms < MsgDuration)
    if (MsgBuff != "")
    {
      engine->PutTexture(engine->MakeColorText(config->ColorGreen, (char *)MsgBuff.c_str(), 8), 10, (engine->WindowHeight - 25));
    }
    if (inputText != "")
    {
      engine->PutTexture(engine->MakeText((char *)inputText.c_str(), 12), 10, (engine->WindowHeight - 50));
    }
    engine->UpdateScreen();
    SDL_RenderPresent( renderer );
    //usleep(10);
    //End Render
      //SDL_RenderPresent( renderer );
			//fflush(stdout);
		}
    config->SaveConfig();
    engine->UnInit();
		delete engine;
		SDL_RenderClear(renderer);
		SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
