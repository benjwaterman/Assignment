#pragma once
#include <string>
#include <iostream>

#ifdef _WIN32 // compiling on windows
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#else // NOT compiling on windows
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#endif

class SpriteHandler
{
public:
	SpriteHandler();	
	SpriteHandler(SDL_Rect rect, SDL_Rect spritePosRect, std::string imagePath);
	static void setRenderer(SDL_Renderer* renderer);
	void drawSprite();
	void animateSprite(int frames, bool loop);
	void moveSprite();
	~SpriteHandler();

private:
	//renderer
	static SDL_Renderer *_ren;

	//text
	SDL_Surface *_surface; 
	SDL_Texture *_tex; 
	SDL_Rect _rect;
	SDL_Rect _spritePosRect;
};

