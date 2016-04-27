#include "SpriteHandler.h"
#include "CollisionHandler.h"

SDL_Renderer* SpriteHandler::_ren = nullptr;

SpriteHandler::SpriteHandler()
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "(This should never be called) Sprite Constructed(%p)", this);
}

SpriteHandler::SpriteHandler(SDL_Rect rect, SDL_Rect spritePosRect, std::string imagePath, bool enableGravity, int colliderType, float scale)
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Sprite Constructed(%p)", this);

	//ASSERT or check that ren is not nullptr
	_posRect = rect;
	_spritePositionDouble = {double (_posRect.x), double(_posRect.y) };
	_texPosRect = spritePosRect;
	_origSPR = _texPosRect; //store original (used for animation)
	_flip = SDL_FLIP_NONE;
	_enableGravity = enableGravity;
	
	if (colliderType != 0)
	{
		_enableCollider = true;
		_colliderType = colliderType;
	}
	scale = 1 / scale;
	_scaleFactor = scale;

	//for scaling purposes
	_posRect.h /= _scaleFactor;
	_posRect.w /= _scaleFactor;

	_currentFrame = 0;

	_surface = IMG_Load(imagePath.c_str());
	if (_surface == nullptr)
	{
		std::cout << "SDL IMG_Load Error: " << SDL_GetError() << std::endl;
		//cleanExit(1);
	}

	_texMove = SDL_CreateTextureFromSurface(_ren, _surface);
	SDL_FreeSurface(_surface);
	if (_texMove == nullptr)
	{
		std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		//cleanExit(1);
	}
}

void SpriteHandler::setRenderer(SDL_Renderer* renderer)
{
	_ren = renderer;
}

void SpriteHandler::drawSprite() //renders sprite
{
	if(!_spriteMoving)
		SDL_RenderCopyEx(_ren, _texIdle, &_texPosRectIdle, &_posRect, 0, 0, _flip);

	SDL_Rect posRectMove = SDL_Rect{ _posRect.x, _posRect.y, _texPosRect.w / _scaleFactor, _texPosRect.h / _scaleFactor };

	if(_spriteMoving)
		SDL_RenderCopyEx(_ren, _texMove, &_texPosRect, &posRectMove, 0, 0, _flip);
}

void SpriteHandler::animateSprite(int startFrame, int frames, int fps, bool loop)
{
	//startFrame--;
	int spriteFPS; //sprite should not play at the same fps as game runs, so this limits the the fps the sprite is running at
	if (fps > 0)
		spriteFPS = 1000 / fps;

	else
		spriteFPS = 0;

	if (loop && _currentFrame >= frames)
	{
		_currentFrame = 0;
	}

	_animationFrame = _currentFrame + startFrame;
	
	_dt += std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - time).count();

	if (_dt > spriteFPS) //64ms ~= 15fps
	{
		_texPosRect = spriteDataList[_currentFrame + startFrame];
		_currentFrame++;
		
		_dt = 0;
	}

	time = Clock::now();
}

void SpriteHandler::populateAnimationData(std::string filePath)
{
	_spriteDataPath = filePath;

	getFromFile('x');
	getFromFile('y');
	getFromFile('w');
	getFromFile('h');
}

void SpriteHandler::getFromFile(char charToGet)
{
	std::ifstream playerWalkJSON(_spriteDataPath);
	std::string str;
	int xVal;
	int yVal;
	int wVal;
	int hVal;
	int counter = 0;

	while (std::getline(playerWalkJSON, str))
	{
		std::size_t pos = str.find("\"frame\""); //find line in file that has "frame" data
		
		if (pos != std::string::npos) //checks pos exists
		{
			std::string str2 = str.substr(pos); //creates substring at position found above

			//finds char
			std::string searchString;
			searchString += "\"";
			searchString += charToGet;
			searchString += "\":";

			std::size_t pos2 = str.find(searchString);

			if (pos != std::string::npos)
			{
				std::string strx = str.substr(pos2);
				std::string charHolder;
				bool canAdd = false;

				for (char c : strx)
				{
					if (c == ',' || c == '}')
					{
						canAdd = false;
						break;
					}

					if (canAdd)
					{
						charHolder += c;
					}

					if (c == ':')
					{
						canAdd = true;
					}
				}
				
				switch (charToGet)
				{
				case 'x':
					xVal = atoi(charHolder.c_str());
					spriteDataList.push_back(SDL_Rect{ xVal, 0, 0, 0 });
					break;

				case 'y':
					yVal = atoi(charHolder.c_str());
					spriteDataList[counter++].y = yVal;
					break;

				case 'w':
					wVal = atoi(charHolder.c_str());
					spriteDataList[counter++].w = wVal;
					break;

				case 'h':
					hVal = atoi(charHolder.c_str());
					spriteDataList[counter++].h = hVal;
					break;
				}
			}
		}
	}
}

//moves sprite
void SpriteHandler::moveSprite(Vector2 vec2) 
{
	double x = vec2.x;
	double y = vec2.y;

	_spriteMoving = true;
	_spriteMovement.x += x;
	_spriteMovement.y += y;
}

void SpriteHandler::createIdleSprite(SDL_Rect rect, SDL_Rect spritePosRect, std::string imagePath)
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Idle sprite Constructed(%p)", this);

	//ASSERT or check that ren is not nullptr
	_texPosRectIdle = spritePosRect;

	_surface = IMG_Load(imagePath.c_str());
	if (_surface == nullptr)
	{
		std::cout << "SDL IMG_Load Error: " << SDL_GetError() << std::endl;
		//cleanExit(1);
	}

	_texIdle = SDL_CreateTextureFromSurface(_ren, _surface);
	SDL_FreeSurface(_surface);

	if (_texMove == nullptr)
	{
		std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		//cleanExit(1);
	}
}

void SpriteHandler::setIdle()
{
	//_animationFrame = -1;
	_spriteMoving = false;
}

bool SpriteHandler::getGravity()
{
	return _enableGravity;
}

bool SpriteHandler::jump(float speed, int height)
{
	//keeps increasing player height until it reaches the height specified
	if (_curJumpHeight < height) 
	{
		_jumping = true;
		_curJumpHeight += speed;
		moveSprite(Vector2(0, -speed));
		//as long as true is returned the function will continue to be called
		return true; 
	}

	//after specified height is reached, reset variable and return false
	else 
	{
		_jumping = false;
		_curJumpHeight = 0;
		return false;
	}
}

int SpriteHandler::getColliderType()
{
	if (_colliderType != 0)
		return _colliderType;

	else
		return 0;
}

SDL_Rect SpriteHandler::getBoxCollider()
{
	return _posRect;
}

int SpriteHandler::getX()
{
	return _posRect.x;
}

//for directly setting the sprites x value
void SpriteHandler::setSpriteX(int x)
{
	_spritePositionDouble.x = x;
}

//for directly setting the sprites y value
void SpriteHandler::setSpriteY(int y)
{
	_spritePositionDouble.y = y;
}

void SpriteHandler::updateMovement(Position4 relativePosition)
{
	//flips sprite according to direction of movement, assuming sprite starts facing right
	if (_startRight)
	{
		if (_spriteMovement.x > 0)
		{
			_flip = SDL_FLIP_NONE;
		}

		else if (_spriteMovement.x < 0)
		{
			_flip = SDL_FLIP_HORIZONTAL;
		}
	}

	else
	{
		if (_spriteMovement.x > 0)
		{
			_flip = SDL_FLIP_HORIZONTAL;
		}

		else if (_spriteMovement.x < 0)
		{
			_flip = SDL_FLIP_NONE;
		}
	}

	//get any collisions and their direction
	//if player is on ladder
	relativePosition.onLadder ? _onLadder = true : _onLadder = false;
	
	//if on a ladder
	if (_onLadder)
	{
		//makes the player "stick" to the ladder when not moving left or right, so they dont clip into terrain when going up and down
		if(_spriteMovement.x == 0)
			setSpriteX(relativePosition.ladderCenter); 
		_enableGravity = false;
	}

	if (relativePosition.above.isTrue && relativePosition.above.type == 1)
	{
		//if colliding above, no moving up
		if (_spriteMovement.y < 0)
			_spriteMovement.y = 0;
	}

	if (relativePosition.beneath.isTrue && relativePosition.beneath.type == 1)
	{
		//if colliding below, no moving down
		if (_spriteMovement.y > 0)
			_spriteMovement.y = 0;
		_enableGravity = false;
	}

	//else if not on ladder and not jumping enable gravity, prevent playing moving up or down
	else if(!_onLadder && !_jumping)
	{
		_enableGravity = true;
	}

	if (relativePosition.left.isTrue && relativePosition.left.type == 1)
	{
		//if colliding left, no moving left
		if (_spriteMovement.x < 0)
			_spriteMovement.x = 0;
	}

	if (relativePosition.right.isTrue && relativePosition.right.type == 1)
	{
		//if colliding right, no moving right
		if (_spriteMovement.x > 0)
			_spriteMovement.x = 0;
	}

	//if gravity is enabled, apply it
	if (_enableGravity == true)
	{
		_spriteMovement.y = _gravitySpeed;
		//_spriteMovement.x = 0;
	}

	//finally update the sprite position
	_spritePositionDouble.x += _spriteMovement.x;

	//prevent the playing from going up when not on a ladder
	if (_jumping != _onLadder || _enableGravity)
		_spritePositionDouble.y += _spriteMovement.y;

	//prevent playing from going double speed if jumping while on ladder
	else if (_jumping && _onLadder)
		_spritePositionDouble.y += _spriteMovement.y/2;

	_posRect.x = int(_spritePositionDouble.x);
	_posRect.y = int(_spritePositionDouble.y);

	//reset movement for next frame
	_spriteMovement = { 0, 0 };
}

SDL_Rect SpriteHandler::getPos()
{
	return _posRect;
}

void SpriteHandler::setPos(int x, int y, int w, int h)
{
	_posRect.x = x;
	_posRect.y = y;
	_posRect.w = w; 
	_posRect.h = h;
}

void SpriteHandler::setPos(SDL_Rect rect)
{
	_posRect = rect;
}

void SpriteHandler::enableGravity(bool x)
{
	_enableGravity = x;
}

void SpriteHandler::setLastClearPos(int x, int y, int w, int h)
{
	_lastClearPosRect.x = x;
	_lastClearPosRect.y = y;
	_lastClearPosRect.w = w;
	_lastClearPosRect.h = h;
}

void SpriteHandler::setLastClearPos(SDL_Rect rect)
{
	_lastClearPosRect = rect;
}

SDL_Rect SpriteHandler::getLastClearPos()
{
	return _lastClearPosRect;
}

void SpriteHandler::setFacing(bool facingRight) 
{
	_startRight = facingRight;
}

void SpriteHandler::setGravitySpeed(double speed)
{
	_gravitySpeed = speed;
}

int SpriteHandler::getCurrentFrame()
{
	return _animationFrame;
}

void SpriteHandler::setCurrentFrame(int frame)
{
	if (frame > 0)
	{
		_animationFrame = frame;
		_texPosRect = spriteDataList[_animationFrame];
	}
	
}

SpriteHandler::~SpriteHandler()
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Sprite Destructed(%p)", this);

	if (_texMove != nullptr)
		SDL_DestroyTexture(_texMove);
}
