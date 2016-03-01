#pragma once
#include "Common.h"

class LevelBuilder
{
public:
	LevelBuilder();
	void getLevel(std::vector<std::unique_ptr<SpriteHandler>> &v, std::string levelPath);
	~LevelBuilder();

private:
	//renderer
	static SDL_Renderer *_ren;
	

	//level
	std::vector<SpriteHandler> levelSpriteList;
	std::string grassImagePath = "./assets/grassMid.png";

	int _column;
	int _row;

	SDL_Rect _posRect;
	SDL_Rect _texPosRect;
	
};

