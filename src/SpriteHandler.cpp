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

void SpriteHandler::animateSprite(int frames, bool loop)
{

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
