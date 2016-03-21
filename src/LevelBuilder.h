#pragma once
#include "Common.h"
#include "SpriteHandler.h"

class LevelBuilder
{
public:
	LevelBuilder();
	std::vector<std::unique_ptr<SpriteHandler>> getLevel(std::string levelPath);
	~LevelBuilder();

private:
	//renderer
	static SDL_Renderer *_ren;
	

	//level
	//std::vector<SpriteHandler> levelSpriteList;
	std::string grassImagePath = "./assets/grassMid.png";
	std::string ladderImagePath = "./assets/ladderMid.png";
	std::string mushroomImagePath = "./assets/mushroomRed.png";
	std::string plantImagePath = "./assets/plantPurple.png";

	int _column;
	int _row;

	SDL_Rect _posRect;
	SDL_Rect _texPosRect;
	
};

