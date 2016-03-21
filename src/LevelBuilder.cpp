#include "LevelBuilder.h"

SDL_Renderer* LevelBuilder::_ren = nullptr;

LevelBuilder::LevelBuilder()
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "LevelBuilder Constructed(%p)", this);
}

std::vector<std::unique_ptr<SpriteHandler>> LevelBuilder::getLevel(std::string levelPath)
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "LevelBuilder Constructed(%p)", this);
	
	std::vector<std::unique_ptr<SpriteHandler>> levelSpriteList;

	_column = 0;
	_row = 0;

	_texPosRect.x = 0;
	_texPosRect.y = 0;
	_texPosRect.w = 70;
	_texPosRect.h = 70;

	std::ifstream levelData(levelPath);
	std::string str;
	//std::unique_ptr<SpriteHandler> floor(_posRect, _texPosRect, grassImagePath, false);
	


	while (std::getline(levelData, str))
	{
		for (char c : str)
		{
			_posRect.x = 35 * (_column);
			_posRect.y = 35 * (_row);
			_posRect.w = 70;
			_posRect.h = 70;

			if (_column >= 20)
			{
				_row++;
				_column = 0;
			}

			switch (c)
			{
			case '0':
				_column++;
				break;
			
			case '1': //terrain
				levelSpriteList.push_back(std::unique_ptr<SpriteHandler>(new SpriteHandler(_posRect, _texPosRect, grassImagePath, false, 1, 0.5)));
				_column++;
				break;
			
			case '2': //ladder
				levelSpriteList.push_back(std::unique_ptr<SpriteHandler>(new SpriteHandler(_posRect, _texPosRect, ladderImagePath, false, 2, 0.5)));
				_column++;
				break;
			
			case '3': //mushroom
				levelSpriteList.push_back(std::unique_ptr<SpriteHandler>(new SpriteHandler(_posRect, _texPosRect, mushroomImagePath, false, 3, 0.5)));
				_column++;
				break;

			case '4': //plant
				levelSpriteList.push_back(std::unique_ptr<SpriteHandler>(new SpriteHandler(_posRect, _texPosRect, plantImagePath, false, 3, 0.5)));
				_column++;
				break;
			
			default:
				_column++;
				break;

			}
		}
	}

	return levelSpriteList;
}

LevelBuilder::~LevelBuilder()
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "LevelBuilder Destructed(%p)", this);
}
