#pragma once
#include "Common.h"
#include "SpriteHandler.h"

class CollisionHandler
{
public:
	CollisionHandler();
	Position4 CheckCollisions(std::unique_ptr<SpriteHandler> const &player, std::vector<std::unique_ptr<SpriteHandler>> const &levelObjects);
	void CheckForScore(int type);
	bool CheckBeneath(int oldPlayerSpriteMaxY, int playerSpriteMaxY, int levelSpriteMinY);
	bool CheckAbove(int, int, int);
	bool CheckLeft(int, int, int);
	bool CheckRight(int, int, int);
	~CollisionHandler();
	
private:
	Position4 _relativePosition;
	int i;
};

