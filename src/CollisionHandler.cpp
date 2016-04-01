#include "CollisionHandler.h"

CollisionHandler::CollisionHandler()
{

}

Position4 CollisionHandler::CheckCollisions(std::unique_ptr<SpriteHandler> const &player, std::vector<std::unique_ptr<SpriteHandler>> const &levelObjects)
{
	for (i = 0; i < (int)levelObjects.size(); i++) //check player collider with every other collider in level
	{
		int playerSpriteX = player->getBoxCollider().x; //represents position of top left pixel x value
		int playerSpriteY = player->getBoxCollider().y; //represents position of top left pixel y value
		int playerSpriteW = player->getBoxCollider().w; //width of sprite, x value + this value give the top right value of the sprite
		int playerSpriteH = player->getBoxCollider().h; //height of sprite, y value + this value give the bottom left value of the sprite

		int playerSpriteCentX = playerSpriteX + (playerSpriteW / 2);
		int playerSpriteCentY = playerSpriteY + (playerSpriteH / 2);

		int levelSpriteX = levelObjects[i]->getBoxCollider().x;
		int levelSpriteY = levelObjects[i]->getBoxCollider().y;
		int levelSpriteW = levelObjects[i]->getBoxCollider().w;
		int levelSpriteH = levelObjects[i]->getBoxCollider().h;

		int colliderType = levelObjects[i]->getColliderType();

		playerSpriteX += 2; //to ensure player falls through gaps (default player sprite is 2 pixels wider than the gaps between terrain)
		playerSpriteW -= 2;

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
		}

		//ladder, make sure center of player is within ladder
		if (levelObjects[i]->getColliderType() == 2) //if ladder
		{
			if (levelSpriteX <= playerSpriteCentX && playerSpriteCentX <= levelSpriteX + levelSpriteW) //x axis
			{
				if (levelSpriteY <= playerSpriteCentY && playerSpriteCentY <= levelSpriteY + levelSpriteH) //y axis
				{
					_relativePosition.onLadder = true;
					_relativePosition.ladderCenter = levelObjects[i]->getX();
				}
			}
		}
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
	else
		_relativePosition.gainScore = false;
}

CollisionHandler::~CollisionHandler()
{

}