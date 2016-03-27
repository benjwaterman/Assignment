#pragma once
#include "Common.h"

typedef std::chrono::high_resolution_clock Clock;

class SpriteHandler
{
public:
	SpriteHandler();	
	SpriteHandler(SDL_Rect rect, SDL_Rect spritePosRect, std::string imagePath, bool enableGravity, int colliderType, float scale);
	static void setRenderer(SDL_Renderer* renderer);
	void drawSprite();
	void animateSprite(int startFrame, int frames, int fps, bool loop);
	void populateAnimationData(std::string filePath);
	void getFromFile(char charToGet);
	void moveSprite(float moveX, float moveY);
	void createIdleSprite(SDL_Rect rect, SDL_Rect spritePosRect, std::string imagePath);
	void setIdle();
	void gravity();
	bool jump(int speed, int height);
	void addBoxCollider();
	int getColliderType();
	SDL_Rect getBoxCollider();
	int getX();
	void setSpriteX(int x);
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
	SDL_Rect _boxCollider;
	SDL_RendererFlip _flip;

	int _spriteWidth;
	int _spriteHeight;
	int _currentFrame;
	int _colFrame = 1;
	int _rowFrame = 1;
	int _scaleFactor;
	int _colliderType; // 1 = solid, eg cant walk through; 2 = ladders, 3 = collectables
	int _curJumpHeight = 0;
	int _speedX = 0;
	int _maxHeight = 9000000;

	bool _spriteMoving;
	bool _enableGravity;
	bool _enableCollider;

	double _dt = 0; //delta time

	std::chrono::steady_clock::time_point time;

	std::string _spriteDataPath;
	std::vector<SDL_Rect> spriteDataList;
};

