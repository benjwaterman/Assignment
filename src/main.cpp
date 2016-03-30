#include "Common.h"
#include "TextBox.h"
#include "SpriteHandler.h"
#include "LevelBuilder.h"
#include "CollisionHandler.h"

typedef std::chrono::high_resolution_clock Clock;

std::string exeName;
SDL_Window *win; //pointer to the SDL_Window
SDL_Renderer *ren; //pointer to the SDL_Renderer
bool done = false;

//sprites
std::vector<std::unique_ptr<SpriteHandler>> spriteList; //list of character spritehandler objects
std::vector<std::unique_ptr<SpriteHandler>> levelSpriteList; //list of level spritehandler objects 

//text
std::vector<std::unique_ptr<TextBox>> textList; //list of textbox objects

//player
bool movingLeft = false;
bool movingRight = false;
bool movingUp = false;
bool movingDown = false;
bool jumping = false;
bool canFall = true;
float playerSpeed = 5.0f;

//sound
Mix_Music *bgMusic;
Mix_Chunk *walkSound;

//score
std::chrono::steady_clock::time_point currentTime;
std::chrono::steady_clock::time_point previousTime;
float deltaTime;
float deltaTime2;
int timeScore = 900;
int bonusScore = 1000;

void handleInput()
{
	//Event-based input handling
	//The underlying OS is event-based, so **each** key-up or key-down (for example)
	//generates an event.
	//  - https://wiki.libsdl.org/SDL_PollEvent
	//In some scenarios we want to catch **ALL** the events, not just to present state
	//  - for instance, if taking keyboard input the user might key-down two keys during a frame
	//    - we want to catch based, and know the order
	//  - or the user might key-down and key-up the same within a frame, and we still want something to happen (e.g. jump)
	//  - the alternative is to Poll the current state with SDL_GetKeyboardState

	SDL_Event event; //somewhere to store an event

	//NOTE: there may be multiple events per frame
	while (SDL_PollEvent(&event)) //loop until SDL_PollEvent returns 0 (meaning no more events)
	{
		switch (event.type)
		{
		case SDL_QUIT:
			done = true; //set donecreate remote branch flag if SDL wants to quit (i.e. if the OS has triggered a close event,
							//  - such as window close, or SIGINT
			break;

			//keydown handling - we should to the opposite on key-up for direction controls (generally)
		case SDL_KEYDOWN:
			//Keydown can fire repeatable if key-repeat is on.
			//  - the repeat flag is set on the keyboard event, if this is a repeat event
			//  - in our case, we're going to ignore repeat events
			//  - https://wiki.libsdl.org/SDL_KeyboardEvent
			if (!event.key.repeat)
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE: 
					done = true;

				case SDLK_d:
					movingRight = true;
					break;

				case SDLK_a:
					movingLeft = true;
					break;

				case SDLK_w:
					movingUp = true;
					break;

				case SDLK_s:
					movingDown = true;
					break;
				
				case SDLK_SPACE:
					if (!canFall)
						jumping = true;
					break;
				}

			if (event.key.repeat)
				switch (event.key.keysym.sym)
				{
				case SDLK_d:
					movingRight = true;
					break;

				case SDLK_a:
					movingLeft = true;
					break;

				case SDLK_SPACE:
					if (!canFall)
						jumping = true;
					break;
				}
			break;

		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
			case SDLK_d:
				movingRight = false;
				break;

			case SDLK_a:
				movingLeft = false;
				break;

			case SDLK_w:
				movingUp = false;
				break;

			case SDLK_s:
				movingDown = false;
				break;
			}
			break;
		}
	}
}
// end::handleInput[]

// tag::updateSimulation[]
void updateSimulation(double simLength = 0.02) //update simulation with an amount of time to simulate for (in seconds)
{
	canFall = true;
	bool onLadder = false;
	/*
	for (int i = 0; i < (int)levelSpriteList.size(); i++) //check player collider with every other collider in level
	{
		int playerSpriteX = spriteList[0]->getBoxCollider().x; //represents position of top left pixel x value
		int playerSpriteY = spriteList[0]->getBoxCollider().y; //represents position of top left pixel y value
		int playerSpriteW = spriteList[0]->getBoxCollider().w; //width of sprite, x value + this value give the top right value of the sprite
		int playerSpriteH = spriteList[0]->getBoxCollider().h; //height of sprite, y value + this value give the bottom left value of the sprite

		int playerSpriteCentX = playerSpriteX + (playerSpriteW / 2);
		int playerSpriteCentY = playerSpriteY + (playerSpriteH / 2);

		int levelSpriteX = levelSpriteList[i]->getBoxCollider().x;
		int levelSpriteY = levelSpriteList[i]->getBoxCollider().y;
		int levelSpriteW = levelSpriteList[i]->getBoxCollider().w;
		int levelSpriteH = levelSpriteList[i]->getBoxCollider().h;

		int colliderType = levelSpriteList[i]->getColliderType();

		playerSpriteX += 2; //to ensure player falls through gaps (default player sprite is 2 pixels wider than the gaps between terrain)
		playerSpriteW -= 2;

		//vertical checks 
		//beneath player
		if ((levelSpriteX <= playerSpriteX && playerSpriteX <= levelSpriteX + levelSpriteW) || 
			(levelSpriteX <= playerSpriteX + playerSpriteW && playerSpriteX + playerSpriteW <= levelSpriteX + levelSpriteW)) //x axis
		{
			if (levelSpriteY <= playerSpriteY + playerSpriteH && playerSpriteY + playerSpriteH <= levelSpriteY + levelSpriteH) //y axis
			{
				switch (colliderType)
				{
				case 1: //solid
					canFall = false;
					movingDown = false;	
					break;
				
				case 3://mushroom
					//pick up, add to score
					break;
				
				case 4://plant
					//pick up, add to score
					break;
				
				default:
					break;
				}
			}
		}

		//above player
		if ((levelSpriteX <= playerSpriteX && playerSpriteX <= levelSpriteX + levelSpriteW) || 
			(levelSpriteX <= playerSpriteX + playerSpriteW && playerSpriteX + playerSpriteW <= levelSpriteX + levelSpriteW)) //x axis
		{
			if (levelSpriteY + levelSpriteH <= playerSpriteY && playerSpriteY <= levelSpriteY + levelSpriteH) //y axis
			{
				switch (colliderType)
				{
				case 1: //solid
					movingUp = false;
					break;

				default:
					break;
				}
			}
		}

		//ladder, make sure center of player is within ladder
		if (levelSpriteList[i]->getColliderType() == 2) //if ladder
		{
			if (levelSpriteX <= playerSpriteCentX && playerSpriteCentX <= levelSpriteX + levelSpriteW) //x axis
			{
				if (levelSpriteY <= playerSpriteCentY && playerSpriteCentY <= levelSpriteY + levelSpriteH) //y axis
				{
					switch (colliderType)
					{
					case 2://ladder
						canFall = false;
						onLadder = true;

						if (!movingLeft && !movingRight)
						{
							spriteList[0]->setSpriteX(levelSpriteList[i]->getX()); //makes the player "stick" to the ladder so they dont clip into terrain when going up and down
						}

						break;

					default:
						break;
					}
				}
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
				switch (colliderType)
				{
				case 1: //solid
					movingRight = false;
					break;

				default:
					break;
				}
			}
		}

		//left
		if (playerSpriteX <= levelSpriteX + levelSpriteW && playerSpriteX >= levelSpriteX) //x axis 
		{
			if ((levelSpriteY <= playerSpriteY && playerSpriteY <= levelSpriteY + levelSpriteH) ||
				(levelSpriteY <= playerSpriteY + playerSpriteH - 2 && playerSpriteY + playerSpriteH - 2 <= levelSpriteY + levelSpriteH) ||
				(levelSpriteY <= playerSpriteY + playerSpriteH - playerSpriteH / 2 && playerSpriteY + playerSpriteH - playerSpriteH / 2 <= levelSpriteY + levelSpriteH)) //have to check at 3 points along edge of sprite, top, middle and bottom for collisions
			{
				switch (colliderType)
				{
				case 1: //solid
					movingLeft = false;
					break;

				default:
					break;
				}
			}
		}
	} */

	Position4 relativePosition = CollisionHandler().CheckCollisions(spriteList[0], levelSpriteList);
	onLadder = CollisionHandler().IsOnLadder();

	if(relativePosition.above)	{movingUp = false;}
	if(relativePosition.beneath)	{canFall = false;}
	if (relativePosition.left)	{movingLeft = false;}
	if (relativePosition.right)	{movingRight = false;}

	if (!onLadder)
	{
		movingUp = false;
		movingDown = false;
	}

	if (canFall && !jumping)
	{
		for (auto const& sprite : spriteList) //apply gravity to all sprites (will only actually apply gravity if it gravity is enabled on sprite creation)
			sprite->gravity();
		jumping = false;
	}

	if (onLadder) //stop playing jumping when on ladder
		jumping = false;

	if (jumping) //when jump is pressed
	{
		switch (spriteList[0]->jump(10, 60)) //speed and height of jump
		{
		case true: //if true is return, player is still jumping
			break;

		case false: //if false is return player has reached the max height of the jump and so is set to no longer be jumping
			jumping = false;
			break;
		}
	}

	if (!canFall)
	{
		if ((movingLeft || movingRight) )//&& !onLadder)
			spriteList[0]->animateSprite(5, 5, 30, true); //frames (includes start frame), sprite fps, looping

		if (movingUp || movingDown) //if moving up or down
			spriteList[0]->animateSprite(11, 2, 10, true);

		if (movingLeft && !movingRight) //moving left
		{
			spriteList[0]->moveSprite(-playerSpeed, 0);
			if (Mix_Playing(1) != 1)
				Mix_PlayChannel(1, walkSound, 0);
		}

		if (movingRight && !movingLeft) //moving right
		{
			spriteList[0]->moveSprite(playerSpeed, 0);
			if (Mix_Playing(1) != 1)
				Mix_PlayChannel(1, walkSound, 0);
		}

		if (movingUp && !movingDown) //moving up
		{
			spriteList[0]->moveSprite(0, -playerSpeed);
			if (Mix_Playing(1) != 1)
				Mix_PlayChannel(1, walkSound, 0);
		}

		if (movingDown && !movingUp) //moving down
		{
			spriteList[0]->moveSprite(0, playerSpeed);
			if (Mix_Playing(1) != 1)
				Mix_PlayChannel(1, walkSound, 0);
		}
	}

	if ((!movingRight && !movingLeft || movingLeft && movingRight) && (!movingUp && !movingDown || movingUp && movingDown)) //player not moving
	{
		if (onLadder)
		{
		}//spriteList[0]->animateSprite(12, 1, 5, true);
		else
			spriteList[0]->setIdle();

		Mix_HaltChannel(1); //stops sound playing when stopping moving, in case half way through sound
	}

	//time decreases by 1 for every 0.5 second starting at 900, bonus goes down by 10 for every 5 time that goes down starting at 1000
	
	currentTime = Clock::now();
	deltaTime += std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - previousTime).count();
	deltaTime2 += std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - previousTime).count();

	if (deltaTime >= 500) //0.5 of a second
	{
		timeScore -= 1;
		deltaTime = 0;
	}

	if (deltaTime2 >= 1000) //0.5 of a second
	{
		bonusScore -= 10;
		deltaTime2 = 0;
	}

	std::string timeScoreText = std::to_string(timeScore);
	std::string bonusScoreText = std::to_string(bonusScore);
	textList[5]->setText("Time " + timeScoreText);
	textList[4]->setText("Bonus " + bonusScoreText);

	previousTime = Clock::now();
}

void render()
{
	//First clear the renderer
	SDL_RenderClear(ren);

	//Draw the sprite
	for (auto const& sprite : levelSpriteList) //loops through all level objects in list and calls their draw (render) function
	{
		sprite->drawSprite();
	}

	//Draw the level
	for (auto const& sprite : spriteList) //loops through all player sprite objects in list and calls their draw (render) function
	{
		sprite->drawSprite();
	}

	//Draw the text
	for (auto const& text : textList) //loops through all TextBox objects in list and calls their draw (render) function
	{
		text->drawText();
	}

	//Update the screen
	SDL_RenderPresent(ren);
}

void cleanExit(int returnValue)
{
	if (returnValue == 1)//pauses before exit so error can be read in console
		std::cin;

	//if (tex != nullptr) SDL_DestroyTexture(tex);
	if (ren != nullptr) SDL_DestroyRenderer(ren);
	if (win != nullptr) SDL_DestroyWindow(win);

	Mix_FreeChunk(walkSound);
	Mix_FreeMusic(bgMusic);
	
	Mix_Quit();
	SDL_Quit();

	exit(returnValue);
}

void fpsLimiter(std::chrono::steady_clock::time_point time) //limits to 60fps
{
	auto time2 = Clock::now();
	auto dt = (int) std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time).count();
	int timeToWait = (16 - dt); //16 = 60fps, 32 = 30fps, 64 = 15fps
	if (timeToWait < 0) //error checking, negative values cause infinite delay
		timeToWait = 0;

	SDL_Delay(timeToWait);
}

// based on http://www.willusher.io/sdl2%20tutorials/2013/08/17/lesson-1-hello-world/
int main( int argc, char* args[] )
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		cleanExit(1);
	}
	std::cout << "SDL initialised OK!\n";

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		std::cout << "SDL_Mixer Error: " << Mix_GetError() << std::endl;
		cleanExit(1);
	}
	std::cout << "SDL_Mixer initialised OK!\n";

	//create window
	win = SDL_CreateWindow("My Game", 100, 100, 700, 945, SDL_WINDOW_SHOWN);

	//error handling
	if (win == nullptr)
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		cleanExit(1);
	}
	std::cout << "SDL CreatedWindow OK!\n";

	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); //creates renderer here
	if (ren == nullptr)
	{
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		cleanExit(1);
	}
	SDL_SetRenderDrawColor(ren, 20, 49, 59, 255); //set background colour
	SDL_RenderClear(ren);
	SpriteHandler::setRenderer(ren); //set SpriteHandler renderer
	TextBox::setRenderer(ren); //set TextBox renderer

	//---- sprite begin ----//
	//---- player 1 begin ----//
	//SDL_Surface *surface; //pointer to the SDL_Surface
	//SDL_Texture *tex; //pointer to the SDL_Texture
	SDL_Rect rect = {150, 150, 66, 92}; //size and position of sprite, x, y, w, h
	SDL_Rect spritePosRect = {0, 0, 66, 92}; //position of sprite in spritesheet, x, y, w, h

	std::string imagePath = "./assets/player_walk.png"; //sprite image path
	std::string spriteDataPath = "./assets/player_walk.txt"; //sprite image data (for animations) path

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Adding player sprite...");
	spriteList.push_back(std::unique_ptr<SpriteHandler>(new SpriteHandler(rect, spritePosRect, imagePath, true, 1, 0.5))); //adds sprite to list
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Player sprite added");

	spriteList[0]->populateAnimationData(spriteDataPath); //reads spritesheet information and stores it for later use

	//create idle
	imagePath = "./assets/player_idle.png";
	rect = { 0, 0, 66, 92 }; 
	spritePosRect = { 0, 0, 66, 92 }; 

	spriteList[0]->createIdleSprite(rect, spritePosRect, imagePath); //creates idle sprite for player when not moving
	//---- player 1 end ----//

	//---- player 2 begin ----//
	
	//---- player 2 end ----//

	//---- ground begin ----//
	imagePath = "./assets/grassMid.png";

	rect = { 0, 0, 70, 70 };
	spritePosRect = { 0, 0, 70, 70 };

	LevelBuilder level01;
	std::string levelPath = "./assets/level01.txt";
	levelSpriteList = level01.getLevel(levelPath);
	
	//---- ground end ----//
	//---- sprite end ----//


	//---- text begin ----//
	if( TTF_Init() == -1 )
	{
		std::cout << "TTF_Init Failed: " << TTF_GetError() << std::endl;
		cleanExit(1);
	}

	TTF_Font* theFont = TTF_OpenFont("./assets/Hack-Regular.ttf", 96); //font point size //Hack-Regular
	if (theFont == nullptr)
	{
		std::cout << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
		cleanExit(1);
	}

	SDL_Color theColour = {255, 255, 255}; //text colour
	SDL_Rect messageRect; //x pos, y pos, width, height
	std::string theString;

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Adding text...");

	theString = "Score";
	messageRect = { 20, 20, 100, 30 };
	textList.push_back(std::unique_ptr<TextBox>(new TextBox(theString, theFont, theColour, messageRect))); //adds text to list

	theString = "000000";
	messageRect = { 150, 20, 110, 30 };
	textList.push_back(std::unique_ptr<TextBox>(new TextBox(theString, theFont, theColour, messageRect)));
	
	theString = "Player 1";
	messageRect = { 20, 70, 120, 30 };
	textList.push_back(std::unique_ptr<TextBox>(new TextBox(theString, theFont, theColour, messageRect)));

	theString = "Level 01";
	messageRect = { 160, 70, 150, 30 };
	textList.push_back(std::unique_ptr<TextBox>(new TextBox(theString, theFont, theColour, messageRect)));

	theString = "Bonus 0000";
	messageRect = { 330, 70, 180, 30 };
	textList.push_back(std::unique_ptr<TextBox>(new TextBox(theString, theFont, theColour, messageRect)));

	theString = "Time 000";
	messageRect = { 540, 70, 150, 30 };
	textList.push_back(std::unique_ptr<TextBox>(new TextBox(theString, theFont, theColour, messageRect)));

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Text added");
	//---- text end ----//


	//---- sound begin ----//
	//load background music
	//bgMusic = Mix_LoadMUS("./assets/background_music.ogg");
	//if (bgMusic == NULL)
	//{
	//	std::cout << "Background music SDL_mixer Error: " << Mix_GetError() << std::endl;
	//	cleanExit(1);
	//}

	//load other sounds
	walkSound = Mix_LoadWAV("./assets/player_footstep.ogg");
	if (walkSound == NULL)
	{
		std::cout << "Walk sound SDL_mixer Error: " << Mix_GetError() << std::endl;
		cleanExit(1);
	}

	//if (Mix_PlayingMusic() == 0)
	//{
	//	Mix_PlayMusic(bgMusic, -1);
	//}

	Mix_VolumeChunk(walkSound, 50); //set volume of footsteps
	//---- sound end ----//

	while (!done) //loop until done flag is set)
	{
		auto time = Clock::now(); //used for FPS limiter

		handleInput(); // this should ONLY SET VARIABLES

		updateSimulation(); // this should ONLY SET VARIABLES according to simulation

		render(); // this should render the world state according to VARIABLES

		fpsLimiter(time); //always call after all other functions
	}

	cleanExit(0);
	return 0;
}
