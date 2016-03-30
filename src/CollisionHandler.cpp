#include "CollisionHandler.h"

CollisionHandler::CollisionHandler()
{

}

Position4 CollisionHandler::CheckCollisions(std::unique_ptr<SpriteHandler> const &player, std::vector<std::unique_ptr<SpriteHandler>> const &levelObjects)
{
	_relativePosition.above = false;
	_relativePosition.beneath = false;
	_relativePosition.left = false;
	_relativePosition.right = false;

	for (int i = 0; i < (int)levelObjects.size(); i++) //check player collider with every other collider in level
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
				_relativePosition.beneath = true;
			}
		}

		//above player
		if ((levelSpriteX <= playerSpriteX && playerSpriteX <= levelSpriteX + levelSpriteW) ||
			(levelSpriteX <= playerSpriteX + playerSpriteW && playerSpriteX + playerSpriteW <= levelSpriteX + levelSpriteW)) //x axis
		{
			if (levelSpriteY + levelSpriteH <= playerSpriteY && playerSpriteY <= levelSpriteY + levelSpriteH) //y axis
			{
				_relativePosition.above = true;
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
				_relativePosition.right = true;
			}
		}

		//left
		if (playerSpriteX <= levelSpriteX + levelSpriteW && playerSpriteX >= levelSpriteX) //x axis 
		{
			if ((levelSpriteY <= playerSpriteY && playerSpriteY <= levelSpriteY + levelSpriteH) ||
				(levelSpriteY <= playerSpriteY + playerSpriteH - 2 && playerSpriteY + playerSpriteH - 2 <= levelSpriteY + levelSpriteH) ||
				(levelSpriteY <= playerSpriteY + playerSpriteH - playerSpriteH / 2 && playerSpriteY + playerSpriteH - playerSpriteH / 2 <= levelSpriteY + levelSpriteH)) //have to check at 3 points along edge of sprite, top, middle and bottom for collisions
			{
				_relativePosition.left = true;
			}
		}

		//ladder, make sure center of player is within ladder
		if (levelObjects[i]->getColliderType() == 2) //if ladder
		{
			if (levelSpriteX <= playerSpriteCentX && playerSpriteCentX <= levelSpriteX + levelSpriteW) //x axis
			{
				if (levelSpriteY <= playerSpriteCentY && playerSpriteCentY <= levelSpriteY + levelSpriteH) //y axis
				{
					_onLadder = true;

						//if (!movingLeft && !movingRight)
						//{
						//	_player.setSpriteX(_levelObjects[i]->getX()); //makes the player "stick" to the ladder so they dont clip into terrain when going up and down
						//}

				}
			}
		}
	}

	return _relativePosition;
}

bool CollisionHandler::CheckType(int type) //check what the collision is with
{
	switch (type)
	{
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
		break;
	default:
		break;
	}
}

bool CollisionHandler::IsOnLadder()
{
	return _onLadder;
}

CollisionHandler::~CollisionHandler()
{

}