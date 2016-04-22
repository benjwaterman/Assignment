#include "CollisionHandler.h"

CollisionHandler::CollisionHandler()
{

}

Position4 CollisionHandler::CheckCollisions(std::unique_ptr<SpriteHandler> const &player, Vector2 playerMovement, std::vector<std::unique_ptr<SpriteHandler>> const &levelObjects)
{
	bool touchingLadder = false;
	for (i = 0; i < (int)levelObjects.size(); i++) //check player collider with every other collider in level
	{
		//new position
		int playerSpriteMinX = player->getBoxCollider().x; //represents position of top pixel x value
		int playerSpriteMinY = player->getBoxCollider().y; //represents position of top pixel y value
		int playerSpriteMaxX = playerSpriteMinX + player->getBoxCollider().w; //width of sprite
		int playerSpriteMaxY = playerSpriteMinY + player->getBoxCollider().h; //height of sprite

		//gets old position
		int oldPlayerSpriteMinX = player->getLastClearPos().x; 
		int oldPlayerSpriteMinY = player->getLastClearPos().y;
		int oldPlayerSpriteMaxX = oldPlayerSpriteMinX + player->getLastClearPos().w;
		int oldPlayerSpriteMaxY = oldPlayerSpriteMinY + player->getLastClearPos().h;

		//center of player
		int playerSpriteCentX = playerSpriteMinX + (player->getBoxCollider().w / 2);
		int playerSpriteCentY = playerSpriteMinY + (player->getBoxCollider().h / 2);

		//level object position
		int levelSpriteMinX = levelObjects[i]->getBoxCollider().x;
		int levelSpriteMinY = levelObjects[i]->getBoxCollider().y;
		int levelSpriteMaxX = levelSpriteMinX + levelObjects[i]->getBoxCollider().w;
		int levelSpriteMaxY = levelSpriteMinY + levelObjects[i]->getBoxCollider().h;
		
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

						//if touching a ladder
						if (colliderType == 2)
						{
							touchingLadder = true;
						}

						//ladders are special a case which is handled seperately
						if (colliderType != 2) 
						{
							//work out the direction the collision occured
							if (CheckBeneath(oldPlayerSpriteMaxY, playerSpriteMaxY, levelSpriteMinY))
							{
								std::cout << "Collision detected beneath" << std::endl;
								_relativePosition.beneath.isTrue = true;
								_relativePosition.beneath.type = colliderType;
							}

							if (CheckAbove(oldPlayerSpriteMinY, playerSpriteMinY, levelSpriteMaxY)) 
							{
								std::cout << "Collision detected above" << std::endl;
								_relativePosition.above.isTrue = true;
								_relativePosition.above.type = colliderType;
							}

							if ((playerSpriteMaxY - 5 > levelSpriteMinY) && CheckRight(oldPlayerSpriteMaxX, playerSpriteMaxX, levelSpriteMinX)) // - 5 required to stop clipping with floor
							{
								std::cout << "Collision detected right" << std::endl;
								_relativePosition.right.isTrue = true;
								_relativePosition.right.type = colliderType;
							}

							if ((playerSpriteMaxY - 5 > levelSpriteMinY) && CheckLeft(oldPlayerSpriteMinX, playerSpriteMinX, levelSpriteMaxX))
							{
								std::cout << "Collision detected left" << std::endl;
								_relativePosition.left.isTrue = true;
								_relativePosition.left.type = colliderType;
							}
						}
					}
				}
			}
		}



		/*
		//vertical checks 
		//beneath player
		if ((levelSpriteX <= playerSpriteX && playerSpriteX <= levelSpriteX + levelSpriteW) ||
			(levelSpriteX <= playerSpriteX + playerSpriteW && playerSpriteX + playerSpriteW <= levelSpriteX + levelSpriteW)) //x axis
		{
			if (levelSpriteY <= playerSpriteY + playerSpriteH && playerSpriteY + playerSpriteH <= levelSpriteY + levelSpriteH) //y axis
			{
				if (colliderType != 2) //ladders are special a case which is handled seperately
				{
					_relativePosition.beneath.isTrue = true;
					_relativePosition.beneath.type = colliderType;

					CheckForScore(colliderType);
				}
			}
		}

		//above player
		if ((levelSpriteX <= playerSpriteX && playerSpriteX <= levelSpriteX + levelSpriteW) ||
			(levelSpriteX <= playerSpriteX + playerSpriteW && playerSpriteX + playerSpriteW <= levelSpriteX + levelSpriteW)) //x axis
		{
			if (levelSpriteY + levelSpriteH <= playerSpriteY && playerSpriteY <= levelSpriteY + levelSpriteH) //y axis
			{
				_relativePosition.above.isTrue = true;
				_relativePosition.above.type = colliderType;

				CheckForScore(colliderType);
			}
		}

		//horizontal checks (aka beside player)
		//right
		if (playerSpriteX + playerSpriteW >= levelSpriteX && playerSpriteX + playerSpriteW <= levelSpriteX + levelSpriteW) //x axis 
		{
			if ((levelSpriteY <= playerSpriteY && playerSpriteY <= levelSpriteY + levelSpriteH) ||
				(levelSpriteY <= playerSpriteY + playerSpriteH - 2 && playerSpriteY + playerSpriteH - 2 <= levelSpriteY + levelSpriteH) ||
				(levelSpriteY <= playerSpriteY + playerSpriteH - playerSpriteH / 2 && playerSpriteY + playerSpriteH - playerSpriteH / 2 <= levelSpriteY + levelSpriteH)) //have to check at 3 points along edge of sprite, top, middle and bottom for collisions
			{
				_relativePosition.right.isTrue = true;
				_relativePosition.right.type = colliderType;

				CheckForScore(colliderType);
			}
		}

		//left
		if (playerSpriteX <= levelSpriteX + levelSpriteW && playerSpriteX >= levelSpriteX) //x axis 
		{
			if ((levelSpriteY <= playerSpriteY && playerSpriteY <= levelSpriteY + levelSpriteH) ||
				(levelSpriteY <= playerSpriteY + playerSpriteH - 2 && playerSpriteY + playerSpriteH - 2 <= levelSpriteY + levelSpriteH) ||
				(levelSpriteY <= playerSpriteY + playerSpriteH - playerSpriteH / 2 && playerSpriteY + playerSpriteH - playerSpriteH / 2 <= levelSpriteY + levelSpriteH)) //have to check at 3 points along edge of sprite, top, middle and bottom for collisions
			{
				_relativePosition.left.isTrue = true;
				_relativePosition.left.type = colliderType;

				CheckForScore(colliderType);
			}
		} */

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

	//	//mushrooms and plants
	//	if (colliderType == 3 || colliderType == 4) 
	//	{
	//		if ((levelSpriteX <= playerSpriteX && playerSpriteX <= levelSpriteX + levelSpriteW) ||
	//			(levelSpriteX <= playerSpriteX + playerSpriteW && playerSpriteX + playerSpriteW <= levelSpriteX + levelSpriteW)) //x axis
	//		{
	//			if (levelSpriteY <= playerSpriteY + playerSpriteH && playerSpriteY + playerSpriteH <= levelSpriteY + levelSpriteH) //y axis
	//			{
	//				CheckForScore(colliderType);
	//			}
	//		}
	//	}
	}

	//last known location with no solid collisions
	if (_relativePosition.beneath.type != 1 && _relativePosition.above.type != 1 && _relativePosition.left.type != 1 && _relativePosition.right.type != 1)
	{
		std::cout << "No collision detected" << std::endl;
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

bool CollisionHandler::CheckBeneath(int oldPlayerSpriteMaxY, int playerSpriteMaxY, int levelSpriteMinY)
{
	return (oldPlayerSpriteMaxY <= levelSpriteMinY && playerSpriteMaxY >= levelSpriteMinY); 
}

bool CollisionHandler::CheckAbove(int oldPlayerSpriteMinY, int playerSpriteMinY, int levelSpriteMaxY)
{
	return (oldPlayerSpriteMinY >= levelSpriteMaxY && playerSpriteMinY <= levelSpriteMaxY);
}

bool CollisionHandler::CheckLeft(int oldPlayerSpriteMinX, int playerSpriteMinX, int levelSpriteMaxX)
{
	return (oldPlayerSpriteMinX >= levelSpriteMaxX && playerSpriteMinX <= levelSpriteMaxX);
}

bool CollisionHandler::CheckRight(int oldPlayerSpriteMaxX, int playerSpriteMaxX, int levelSpriteMinX)
{
	return (oldPlayerSpriteMaxX <= levelSpriteMinX && playerSpriteMaxX >= levelSpriteMinX);
}

CollisionHandler::~CollisionHandler()
{

}