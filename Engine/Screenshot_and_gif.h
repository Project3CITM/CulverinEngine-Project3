#ifndef _SCREENSHOT_AND_GIF_
#define _SCREENSHOT_AND_GIF_

#include "Math\float2.h"
#include "SDL\include\SDL_scancode.h"

#include "Globals.h"

static uint ScreenshotsNum = 0; //Used to enable multiple screenshots in the same second

enum Culverin_PartScreenshot_STATE
{
	Culverin_PartScreenshot_STATE_WAITING_FOR_POINT1,
	Culverin_PartScreenshot_STATE_WAITING_FOR_POINT2
};

struct Culverin_Screenshot
{
	SDL_Scancode FullScreenKey = SDL_Scancode::SDL_SCANCODE_F10;
	SDL_Scancode PartScreenKey = SDL_Scancode::SDL_SCANCODE_F9;
	Culverin_PartScreenshot_STATE Screen_State_part = Culverin_PartScreenshot_STATE::Culverin_PartScreenshot_STATE_WAITING_FOR_POINT1;

	float2 Point1 = float2::zero;
	float2 Point2 = float2::zero;

	void TakeFullScreen();
	void TakePartScreen();
	void Stop();
};

enum Culverin_FullGif_STATE
{
	Culverin_FullGif_STATE_CAN_START,
	Culverin_FullGif_STATE_RUNNING
};

enum Culverin_PartGif_STATE
{
	Culverin_PartGif_STATE_WAITING_FOR_POINT1,
	Culverin_PartGif_STATE_WAITING_FOR_POINT2,
	Culverin_PartGif_STATE_RUNNING
};

struct Culverin_Gif
{
	int FullScreenKey = SDL_Scancode::SDL_SCANCODE_F8;
	int PartScreenKey = SDL_Scancode::SDL_SCANCODE_F7;
	Culverin_FullGif_STATE Gif_State_full = Culverin_FullGif_STATE::Culverin_FullGif_STATE_CAN_START;
	Culverin_PartGif_STATE Gif_State_part = Culverin_PartGif_STATE::Culverin_PartGif_STATE_WAITING_FOR_POINT1;
	unsigned char* pixels_full = nullptr;
	unsigned char* pixels_part = nullptr;
	
	float2 Point1 = float2::zero;
	float2 Point2 = float2::zero;

	float2 MinPoint = float2::zero;
	float2 MaxPoint = float2::zero;
	uint width = 0;
	uint height = 0;

	void TakeFullScreen(float dt);
	void TakePartScreen(float dt);
	void Stop();
	void StopFull();
	void StopPart();
};

#endif