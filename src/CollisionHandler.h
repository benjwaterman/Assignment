#pragma once
#include "Common.h"
#include "SpriteHandler.h"

class CollisionHandler
{
public:
	CollisionHandler();
	Position4 CheckCollisions(std::unique_ptr<SpriteHandler> const &player, std::vector<std::unique_ptr<SpriteHandler>> const &levelObjects);
	void CheckForScore(int type);
	~CollisionHandler();
	
private:
	Position4 _relativePosition;
	int i;
};

