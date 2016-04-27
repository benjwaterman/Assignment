#pragma once
#include "Common.h"
#include "SpriteHandler.h"

class CollisionHandler
{
public:
	CollisionHandler();
	Position4 CheckCollisions(std::unique_ptr<SpriteHandler> const &player, Vector2 playerMovement, std::vector<std::unique_ptr<SpriteHandler>> const &levelObjects);
	void CheckForScore(int type);
	bool CheckBeneath(double, double, double);
	bool CheckAbove(double, double, double);
	bool CheckLeft(double, double, double);
	bool CheckRight(double, double, double);
	~CollisionHandler();
	
private:
	Position4 _relativePosition;
	int i;
};

