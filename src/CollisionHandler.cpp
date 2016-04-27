#include "CollisionHandler.h"

CollisionHandler::CollisionHandler()
{

}

Position4 CollisionHandler::CheckCollisions(std::unique_ptr<SpriteHandler> const &player, Vector2 playerMovement, std::vector<std::unique_ptr<SpriteHandler>> const &levelObjects)
{
	bool touchingLadder = false;
	bool touchingFloor = false;
	for (i = 0; i < (int)levelObjects.size(); i++) //check player collider with every other collider in level
	{
		//new position
		double playerSpriteMinX = player->getBoxCollider().x; //represents position of top pixel x value
		double playerSpriteMinY = player->getBoxCollider().y; //represents position of top pixel y value
		double playerSpriteMaxX = playerSpriteMinX + player->getBoxCollider().w; //width of sprite
		double playerSpriteMaxY = playerSpriteMinY + player->getBoxCollider().h; //height of sprite

		//gets old position
		double oldPlayerSpriteMinX = player->getLastClearPos().x;
		double oldPlayerSpriteMinY = player->getLastClearPos().y;
		double oldPlayerSpriteMaxX = oldPlayerSpriteMinX + player->getLastClearPos().w;
		double oldPlayerSpriteMaxY = oldPlayerSpriteMinY + player->getLastClearPos().h;

		//center of player
		double playerSpriteCentX = playerSpriteMinX + (player->getBoxCollider().w / 2);
		double playerSpriteCentY = playerSpriteMinY + (player->getBoxCollider().h / 2);

		//level object position
		double levelSpriteMinX = levelObjects[i]->getBoxCollider().x;
		double levelSpriteMinY = levelObjects[i]->getBoxCollider().y;
		double levelSpriteMaxX = levelSpriteMinX + levelObjects[i]->getBoxCollider().w;
		double levelSpriteMaxY = levelSpriteMinY + levelObjects[i]->getBoxCollider().h;
		
		//type of collider of current level object
		int colliderType = levelObjects[i]->getColliderType();

		//to ensure player falls through gaps (default player sprite is 2 pixels wider than the gaps between terrain)
		playerSpriteMinX += 2; 
		playerSpriteMaxX -= 2;

		//checks for collisions where player will be next frame if movement takes place
		if (playerMovement.y > 0) //going down
		{
			playerSpriteMaxY += playerMovement.y;
		}

		else if (playerMovement.y < 0) //going up
		{
			playerSpriteMinY += playerMovement.y;
		}

		else if (playerMovement.x > 0) //going right
		{
			playerSpriteMaxX += playerMovement.x;
			playerSpriteMinX += playerMovement.x;
		}

		else if (playerMovement.x < 0) //going left
		{
			playerSpriteMinX += playerMovement.x;
			playerSpriteMaxX += playerMovement.x;
		}

		//checks if collision boxes interesect
		if (playerSpriteMaxX >= levelSpriteMinX)
		{
			if (playerSpriteMinX <= levelSpriteMaxX)
			{
				if (playerSpriteMaxY >= levelSpriteMinY)
				{
					if (playerSpriteMinY <= levelSpriteMaxY)
					{
						//check if collision has occurred with a score gaining object
						CheckForScore(colliderType);

						//ladders are special a case which is handled seperately
						if (colliderType != 2) 
						{
							//work out the direction the collision occured
							if (CheckBeneath(oldPlayerSpriteMaxY, playerSpriteMaxY, levelSpriteMinY))
							{
								//std::cout << "Collision detected beneath" << std::endl;
								_relativePosition.beneath.isTrue = true;
								_relativePosition.beneath.type = colliderType;
							}

							if (CheckAbove(oldPlayerSpriteMinY, playerSpriteMinY, levelSpriteMaxY)) 
							{
								//std::cout << "Collision detected above" << std::endl;
								_relativePosition.above.isTrue = true;
								_relativePosition.above.type = colliderType;
							}

							if (CheckRight(oldPlayerSpriteMaxX, playerSpriteMaxX, levelSpriteMinX))
							{
								//std::cout << "Collision detected right" << std::endl;
								_relativePosition.right.isTrue = true;
								_relativePosition.right.type = colliderType;
							}

							if (CheckLeft(oldPlayerSpriteMinX, playerSpriteMinX, levelSpriteMaxX))
							{
								//std::cout << "Collision detected left" << std::endl;
								_relativePosition.left.isTrue = true;
								_relativePosition.left.type = colliderType;
							}
						}
					}
				}
			}
		}

		//bounds of level check
		if (playerSpriteMinX <= 0)
		{
			_relativePosition.left.isTrue = true;
			_relativePosition.left.type = 1;
		}

		if (playerSpriteMaxX >= 700)
		{
			_relativePosition.right.isTrue = true;
			_relativePosition.right.type = 1;
		}

		if (playerSpriteMaxY >= 945)
		{
			_relativePosition.beneath.isTrue = true;
			_relativePosition.beneath.type = 1;
		}

		if (playerSpriteMinY <= 0)
		{
			_relativePosition.above.isTrue = true;
			_relativePosition.above.type = 1;
		}

		//ladder, make sure center of player is within ladder
		if (colliderType == 2) //if ladder
		{
			if (levelSpriteMinX <= playerSpriteCentX && playerSpriteCentX <= levelSpriteMaxX) //x axis
			{
				if (levelSpriteMinY <= playerSpriteCentY && playerSpriteCentY <= levelSpriteMaxY) //y axis
				{
					_relativePosition.onLadder = true;
					_relativePosition.ladderCenter = levelObjects[i]->getX();
				}
			}
		}
	}

	//last known location with no solid collisions
	if (_relativePosition.beneath.type != 1 && _relativePosition.above.type != 1 && _relativePosition.left.type != 1 && _relativePosition.right.type != 1)
	{
		//std::cout << "No collision detected" << std::endl;
		player->setLastClearPos(player->getPos());
	}

	return _relativePosition;
}

void CollisionHandler::CheckForScore(int type)
{
	if (type == 3 || type == 4)
	{
		_relativePosition.gainScore = true;
		_relativePosition.elementInArray = i;
	}
}

bool CollisionHandler::CheckBeneath(double oldPlayerSpriteMaxY, double playerSpriteMaxY, double levelSpriteMinY)
{
	return (oldPlayerSpriteMaxY <= levelSpriteMinY && playerSpriteMaxY >= levelSpriteMinY); 
}

bool CollisionHandler::CheckAbove(double oldPlayerSpriteMinY, double playerSpriteMinY, double levelSpriteMaxY)
{
	return (oldPlayerSpriteMinY >= levelSpriteMaxY && playerSpriteMinY <= levelSpriteMaxY);
}

bool CollisionHandler::CheckLeft(double oldPlayerSpriteMinX, double playerSpriteMinX, double levelSpriteMaxX)
{
	return (oldPlayerSpriteMinX >= levelSpriteMaxX && playerSpriteMinX <= levelSpriteMaxX);
}

bool CollisionHandler::CheckRight(double oldPlayerSpriteMaxX, double playerSpriteMaxX, double levelSpriteMinX)
{
	return (oldPlayerSpriteMaxX <= levelSpriteMinX && playerSpriteMaxX >= levelSpriteMinX);
}

CollisionHandler::~CollisionHandler()
{

}