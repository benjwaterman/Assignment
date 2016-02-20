#include "SpriteHandler.h"

#ifdef _WIN32 // compiling on windows
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#else // NOT compiling on windows
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#endif

SDL_Renderer* SpriteHandler::_ren = nullptr;

SpriteHandler::SpriteHandler()
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "(This should never be called) Sprite Constructed(%p)", this);
}

SpriteHandler::SpriteHandler(SDL_Rect rect, SDL_Rect spritePosRect, std::string imagePath)
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Sprite Constructed(%p)", this);

	//ASSERT or check that ren is not nullptr
	_rect = rect;
	_spritePosRect = spritePosRect;
	_origSPR = _spritePosRect; //store original (used for animation)

	_spriteWidth = spritePosRect.w - spritePosRect.x; //works out width of sprite
	_spriteHeight = spritePosRect.h - spritePosRect.y; //works out height of sprite

	_currentFrame = 1;

	_surface = IMG_Load(imagePath.c_str());
	if (_surface == nullptr)
	{
		std::cout << "SDL IMG_Load Error: " << SDL_GetError() << std::endl;
		//cleanExit(1);
	}

	_tex = SDL_CreateTextureFromSurface(_ren, _surface);
	SDL_FreeSurface(_surface);
	if (_tex == nullptr)
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
	SDL_RenderCopy(_ren, _tex, &_spritePosRect, &_rect);
}

void SpriteHandler::animateSprite(int framesPerRow, int framesPerCol, int frames, bool loop)
{
	if (_rowFrame == framesPerRow)
	{
		if (_colFrame == framesPerCol)
		{
			_colFrame = 1;
		}

		_rowFrame = 1;
		_colFrame++;
	}

	if (_currentFrame < frames)
	{
		_spritePosRect.x = _origSPR.x + (_spriteWidth * (_rowFrame - 1));
		_spritePosRect.y = _origSPR.y + (_spriteHeight * (_colFrame - 1));
		_currentFrame++;
		_rowFrame++;
	}

	if (loop && _currentFrame == frames)
	{
		_currentFrame = 1;
		_spritePosRect.x = 0;
	}
}

void SpriteHandler::moveSprite()
{

}

SpriteHandler::~SpriteHandler()
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Sprite Destructed(%p)", this);

	if (_tex != nullptr) 
		SDL_DestroyTexture(_tex);
}
