#pragma once
#include "Common.h"

typedef std::chrono::high_resolution_clock Clock;

class SpriteHandler
{
public:
	SpriteHandler();	
	SpriteHandler(SDL_Rect rect, SDL_Rect spritePosRect, std::string imagePath, bool enableGravity);
	static void setRenderer(SDL_Renderer* renderer);
	void drawSprite();
	void animateSprite(int startFrame, int frames, int fps, bool loop);
	void populatAnimationData(std::string filePath);
	void getFromFile(char charToGet);
	void moveSprite(float moveX, float moveY);
	void createIdleSprite(SDL_Rect rect, SDL_Rect spritePosRect, std::string imagePath);
	void setIdle();
	void gravity();
	~SpriteHandler();

private:
	//renderer
	static SDL_Renderer *_ren;

	//sprite
	SDL_Surface *_surface; 
	SDL_Texture *_texMove;
	SDL_Texture *_texIdle;
	SDL_Rect _posRect;
	SDL_Rect _texPosRect;
	SDL_Rect _texPosRectIdle;
	SDL_Rect _origSPR;
	SDL_RendererFlip _flip;

	int _spriteWidth;
	int _spriteHeight;
	int _currentFrame;
	int _colFrame = 1;
	int _rowFrame = 1;

	bool _spriteMoving;
	bool _enableGravity;

	double _dt = 0; //delta time

	std::chrono::steady_clock::time_point time;

	std::string _spriteDataPath;
	std::vector<std::unique_ptr<SDL_Rect>> spriteDataList;
};

