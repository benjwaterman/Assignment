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
	if (_currentFrame == frames)
	{
		_currentFrame = 1;
	}

	_spritePosRect = *spriteDataList[_currentFrame - 1];
	_currentFrame++;
	
	//if (_rowFrame == framesPerRow)
	//{
	//	if (_colFrame == framesPerCol)
	//	{
	//		_colFrame = 1;
	//	}

	//	_rowFrame = 1;
	//	_colFrame++;
	//}

	//if (_currentFrame < frames)
	//{
	//	_spritePosRect.x = _origSPR.x + (_spriteWidth * (_rowFrame - 1));
	//	_spritePosRect.y = _origSPR.y + (_spriteHeight * (_colFrame - 1));
	//	_currentFrame++;
	//	_rowFrame++;
	//}

	//if (loop && _currentFrame == frames)
	//{
	//	_currentFrame = 1;
	//	_spritePosRect.x = 0;
	//}
}

void SpriteHandler::populatAnimationData(std::string filePath)
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

	while (std::getline(playerWalkJSON, str))
	{
		std::size_t pos = str.find("\"frame\""); //find line in file that has "frame" data
		int counter = 0;
		if (pos != std::string::npos)
		{
			std::string str2 = str.substr(pos); //creates substring at position found above

			//finds char
			std::string searchString;
			searchString += "\"";
			searchString += charToGet;
			searchString += "\":";
			std::cout << "search string: " << searchString << std::endl;

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

				std::cout << "char: " << charToGet << " " << charHolder << std::endl;
				
				switch (charToGet)
				{
				case 'x':
					xVal = atoi(charHolder.c_str());
					spriteDataList.push_back(std::unique_ptr<SDL_Rect>(new SDL_Rect{ xVal, 0, 0, 0 }));
					break;

				case 'y':
					yVal = atoi(charHolder.c_str());
					spriteDataList[counter++]->y = yVal;
					break;

				case 'w':
					wVal = atoi(charHolder.c_str());
					spriteDataList[counter++]->w = wVal;
					break;

				case 'h':
					hVal = atoi(charHolder.c_str());
					spriteDataList[counter++]->h = hVal;
					break;
				}
			}
		}
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
