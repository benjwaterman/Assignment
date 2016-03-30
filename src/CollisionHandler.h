#pragma once
#include "Common.h"
#include "SpriteHandler.h"

struct Position4 
{
	bool above, beneath, left, right;
};

class CollisionHandler
{
public:
	CollisionHandler();
	Position4 CheckCollisions(std::unique_ptr<SpriteHandler> const &player, std::vector<std::unique_ptr<SpriteHandler>> const &levelObjects);
	bool CheckType(int type);
	bool IsOnLadder();
	~CollisionHandler();
	
private:
	Position4 _relativePosition;
	bool _onLadder = false;
};

