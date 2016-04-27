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
	void moveSprite(Vector2 vec2);
	void createIdleSprite(SDL_Rect rect, SDL_Rect spritePosRect, std::string imagePath);
	void setIdle();
	bool getGravity();
	bool jump(float speed, int height);
	int getColliderType();
	SDL_Rect getBoxCollider();
	int getX();
	void setSpriteX(int x);
	void setSpriteY(int y);
	void updateMovement(Position4 relativePosition);
	SDL_Rect getPos();
	void setPos(int x, int y, int w, int h);
	void setPos(SDL_Rect rect);
	void enableGravity(bool x);
	void setLastClearPos(int x, int y, int w, int h);
	void setLastClearPos(SDL_Rect rect);
	void setFacing(bool facing);
	void setGravitySpeed(double speed);
	SDL_Rect getLastClearPos();
	int getCurrentFrame();
	void setCurrentFrame(int frame);
	void setFlip( int flip);
	int getFlip();

	~SpriteHandler();

private:
	//renderer
	static SDL_Renderer *_ren;

	//sprite
	SDL_Surface *_surface; 
	SDL_Texture *_texMove;
	SDL_Texture *_texIdle;
	SDL_Rect _posRect;
	SDL_Rect _lastClearPosRect;
	SDL_Rect _texPosRect;
	SDL_Rect _texPosRectIdle;
	SDL_Rect _origSPR;
	SDL_Rect _boxCollider;
	SDL_RendererFlip _flip;

	Vector2 _spriteMovement;
	//for keeping movement as double instead of int, needed for physics sim
	Vector2 _spritePositionDouble;

	int _spriteWidth;
	int _spriteHeight;
	int _currentFrame = 0;
	int _animationFrame = 0;
	int _colFrame = 1;
	int _rowFrame = 1;
	int _scaleFactor;
	int _colliderType; // 1 = solid, eg cant walk through; 2 = ladders, 3 = collectables
	int _speedX = 0;
	int _maxHeight = 9000000;

	bool _spriteMoving;
	bool _enableGravity;
	bool _enableCollider;
	bool _movingLeft = false;
	bool _movingRight = false;
	bool _movingUp = false;
	bool _movingDown = false;
	bool _jumping = false;
	bool _canFall = true;
	bool _onLadder = true;
	bool _startRight = true;

	double _dt = 0; //delta time
	double _gravitySpeed = 0;
	double _curJumpHeight = 0;

	std::chrono::steady_clock::time_point time;

	std::string _spriteDataPath;
	std::vector<SDL_Rect> spriteDataList;
};

