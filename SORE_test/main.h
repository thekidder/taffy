
#ifndef  __MAIN_H__
#define  __MAIN_H__

#include <sore.h>

using namespace std;

//Functions
void Cleanup();
void Resize(int width, int height);
int  Render();
bool EventLoop();
void GameLogic(float timeElapsed, float timestep);
int  GameLoop();
int  InitGL();
int  InitSDL();
int  LoadGLTextures();
void ToggleGrab();


int HandleUserEvents(SDL_Event* event);
int HandleKeydown   (SDL_Event* event);
int HandleKeyup     (SDL_Event* event);
int HandleMouse     (SDL_Event* event);

#endif //__MAIN_H__
