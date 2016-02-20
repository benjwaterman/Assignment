#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <fstream>

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
	void animateSprite(int framesPerRow, int framesPerCol, int frames, bool loop);
	void populatAnimationData(std::string filePath);
	void getFromFile(char charToGet);
	void moveSprite();
	~SpriteHandler();

private:
	//renderer
	static SDL_Renderer *_ren;

	//sprite
	SDL_Surface *_surface; 
	SDL_Texture *_tex; 
	SDL_Rect _rect;
	SDL_Rect _spritePosRect;
	SDL_Rect _origSPR;

	int _spriteWidth;
	int _spriteHeight;
	int _currentFrame;
	int _colFrame = 1;
	int _rowFrame = 1;

	std::string _spriteDataPath;
	std::vector<std::unique_ptr<SDL_Rect>> spriteDataList;
};

