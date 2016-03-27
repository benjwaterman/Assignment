#include "SpriteHandler.h"

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

	if (_enableCollider)
		addBoxCollider();
}

void SpriteHandler::setRenderer(SDL_Renderer* renderer)
{
	_ren = renderer;
}

void SpriteHandler::drawSprite() //renders sprite
{
	if (_enableCollider)
		addBoxCollider();

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

void SpriteHandler::moveSprite(float moveX, float moveY) //moves sprite and flips it according to direction of movement, assuming sprites starts facing right
{
	_speedX = moveX;
	_spriteMoving = true;
	_posRect.x += moveX;
	_posRect.y += moveY;

	//makes the sprite face the correct way when moving
	if (moveX > 0)
	{
		_flip = SDL_FLIP_NONE;
	}

	else if (moveX < 0)
	{
		_flip = SDL_FLIP_HORIZONTAL;
	}
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
	_spriteMoving = false;
	_speedX = 0;
}

void SpriteHandler::gravity()
{
	if (_posRect.y < 900 && _enableGravity)
		moveSprite(0, 5);
}

bool SpriteHandler::jump(int speed, int height)
{
	if (_curJumpHeight < height)
	{
		_curJumpHeight += speed;
		moveSprite(_speedX, -speed);
		return true;
	}

	else
	{
		_curJumpHeight = 0;
		return false;
	}
}

void SpriteHandler::addBoxCollider()
{
	_boxCollider = { _posRect.x, _posRect.y, _posRect.w, _posRect.h };
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
	return _boxCollider;
}

int SpriteHandler::getX()
{
	return _posRect.x;
}

void SpriteHandler::setSpriteX(int x)
{
	_posRect.x = x;
}

SpriteHandler::~SpriteHandler()
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Sprite Destructed(%p)", this);

	if (_texMove != nullptr)
		SDL_DestroyTexture(_texMove);

	if (_texIdle != nullptr)
		SDL_DestroyTexture(_texMove);
}
