#include "LevelBuilder.h"

SDL_Renderer* LevelBuilder::_ren = nullptr;

LevelBuilder::LevelBuilder()
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "LevelBuilder Constructed(%p)", this);
}

void LevelBuilder::getLevel(std::vector<std::unique_ptr<SpriteHandler>> &v, std::string levelPath)
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "LevelBuilder Constructed(%p)", this);

	_column = 1;
	_row = 1;

	std::ifstream levelData(levelPath);
	std::string str;
	SpriteHandler floor(_posRect, _texPosRect, grassImagePath, false);


	while (std::getline(levelData, str))
	{
		for (char c : str)
		{
			switch (c)
			{
			case 0:
				_row++;
				break;
			
			case 1:
				_row++;
				levelSpriteList.push_back(floor);
				break;
			
			case 2:
				_row++;
				break;
			
			case 3:
				_row++;
				break;
			
			default:
				_row++;
				break;

			}
		}

		_row++;
	}
}

LevelBuilder::~LevelBuilder()
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "LevelBuilder Destructed(%p)", this);
}
