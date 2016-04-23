#include "Common.h"
#include "TextBox.h"
#include "SpriteHandler.h"
#include "LevelBuilder.h"
#include "CollisionHandler.h"
#include "Window.h"

typedef std::chrono::high_resolution_clock Clock;

//window
std::string exeName;
SDL_Renderer *ren; //pointer to the SDL_Renderer
Window win; //custom class window
int const winWidth = 700;
int const winHeight = 945;

bool done = false;
bool paused = false;

//sprites
std::vector<std::unique_ptr<SpriteHandler>> spriteList; //list of character spritehandler objects
std::vector<std::unique_ptr<SpriteHandler>> levelSpriteList; //list of level spritehandler objects 
std::vector<std::unique_ptr<SpriteHandler>> menuSpriteList; //list of menu spritehandler objects 
std::vector<std::unique_ptr<SpriteHandler>> menuSpriteListSelected; //list of menu spritehandler objects 

//text
std::vector<std::unique_ptr<TextBox>> textList; //list of textbox objects

//player
bool movingLeft = false;
bool movingRight = false;
bool movingUp = false;
bool movingDown = false;
bool jumping = false;
bool canFall = true;
bool canJump = true;
int playerSpeed = 5.0f;
Vector2 moveVector;
int thisPlayer = 0; //for keeping track of which player the client/server is

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
int score = 0;

//multiplayer
bool ZMQserver = false; //is this client the server
zmq::context_t this_zmq_context(1);

zmq::socket_t this_zmq_publisher(this_zmq_context, ZMQ_PUB); 
zmq::socket_t this_zmq_subscriber(this_zmq_context, ZMQ_SUB);

//pause menu
SDL_Surface *pauseSurface;
SDL_Texture *pauseTex;
int menuItem = 0;
int menuItemTotal = 5;

void cleanExit(int returnValue)
{
	if (returnValue == 1)//pauses before exit so error can be read in console
		std::cin;

	//if (tex != nullptr) SDL_DestroyTexture(tex);
	if (ren != nullptr) SDL_DestroyRenderer(ren);
	SDL_FreeSurface(pauseSurface);
	//if (win != nullptr) SDL_DestroyWindow(win);

	Mix_FreeChunk(walkSound);
	Mix_FreeMusic(bgMusic);

	Mix_Quit();
	SDL_Quit();

	exit(returnValue);
}

void handleNetwork()
{
	// message format
	// 4 floats, separated by spaces, each with 6 digits, including 0 padding, and 3 digits of precision
	// this will use 8 bytes per float (6 digits, the decimal, and the space)
	// making 4 * 8 bytes = 32 bytes
	const int messageLength = 32;

	//server
	if (ZMQserver)
	{
		// create a message
		zmq::message_t message(messageLength);

		// add message content according to above format
		snprintf((char *)message.data(), messageLength,
			"%06.3f %06.3f %06.3f %06.3f ", float(spriteList[0]->getPos().x), float(spriteList[0]->getPos().y), float(spriteList[0]->getPos().w), float(spriteList[0]->getPos().h));

		//std::cout << "Message sent: \"" << std::string(static_cast<char*>(message.data()), message.size()) << "\"" << std::endl;

		//  Send message to all subscribers
		this_zmq_publisher.send(message);
	}

	//not server
	else
	{
		// set up NULL filter - i.e. accept all messages
		const std::string filter = "";

		// set filter on subscriber (we don't really need to do this everyone time)
		this_zmq_subscriber.setsockopt(ZMQ_SUBSCRIBE, filter.c_str(), filter.length());

		// storage for a new message
		zmq::message_t update;

		// loop while there are messages (could be more than one)
		while (this_zmq_subscriber.recv(&update, ZMQ_DONTWAIT))
		{
			// get the data from the message as a char* (for debug output)
			char* the_data = static_cast<char*>(update.data());

			// debug output
			std::cout << "Message received: \"" << std::string(the_data) << "\"" << std::endl;

			// get the data as a streamstring (many other options than this)
			std::istringstream iss(static_cast<char*>(update.data()));

			//data format is floats, so have to read back to floats
			float tx, ty, mx, my;

			// read the string stream into the four floats
			iss >> tx >> ty >> mx >> my;

			// use those floats to set the SDL_Rects (auto convert to int)
			spriteList[0]->setPos(tx, ty, mx, my);
			//texture_rect.y = ty;
			//message_rect.x = mx;
			//message_rect.y = my;

			// Debug output
			//std::cout << "texture x, y: " << std::to_string(texture_rect.x) << ", " << std::to_string(texture_rect.y) << std::endl;
			//std::cout << "message x, y: " << std::to_string(message_rect.x) << ", " << std::to_string(message_rect.y) << std::endl;
		}
	}
}

void handleInput()
{
	SDL_Event event; //somewhere to store an event

	while (SDL_PollEvent(&event)) //loop until SDL_PollEvent returns 0 (meaning no more events)
	{
		switch (event.type)
		{
		case SDL_QUIT:
			done = true; 
			break;

		case SDL_KEYDOWN:
			if (!event.key.repeat)
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE: 
					done = true;

				case SDLK_d:
					if (!paused)
					{
						movingRight = true;
						movingLeft = false;
					}
					break;

				case SDLK_a:
					if (!paused)
					{
						movingLeft = true;
						movingRight = false;
					}
					break;

				case SDLK_w:
					if (!paused)
						movingUp = true;
					else
					{
						if (0 < menuItem && menuItem < menuItemTotal)
							menuItem--;
						else
							menuItem = menuItemTotal - 1;
					}
					break;

				case SDLK_s:
					if (!paused)
						movingDown = true;
					else
					{
						if (menuItem < menuItemTotal - 1)
							menuItem++;
						else
							menuItem = 0;
					}
					break;

				case SDLK_RETURN:
					if (paused)
					{
						switch (menuItem)
						{
						//resume
						case 0:
							paused = false;
							break;

						//toggle bg music
						case 1:
							if (Mix_PausedMusic() == 1)
								Mix_ResumeMusic();
							else
								Mix_PauseMusic();
							break;

						//toggle fullscreen
						case 2:
							win.fullscreenToggle();
							break;

						case 3:
							break;

						case 4:
							cleanExit(0);
							break;

						default:
							break;
						}
					}
					
					break;

				//pause or unpause
				case SDLK_p:
					paused = !paused;
					break;
				
				case SDLK_SPACE:
					if (!paused && canJump)
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
	if (ZMQserver)
	{
		/*bool onLadder;
		canFall = true; //always start each frame as being able to fall, unless a collision occurs

		//get any collisions and their direction
		Position4 relativePosition = CollisionHandler().CheckCollisions(spriteList[0], levelSpriteList);

		//checks collisions and sets variables 
		//if player is on ladder
		relativePosition.onLadder ? onLadder = true : onLadder = false;

		if (relativePosition.beneath.type == 1 && relativePosition.above.type == 1 && relativePosition.left.type == 1 && relativePosition.right.type == 1)
		{
			spriteList[0]->setPos(spriteList[0]->getLastClearPos());
		}

		if (relativePosition.above.isTrue && relativePosition.above.type == 1)
		{
			movingUp = false;
			spriteList[0]->setSpriteY(spriteList[0]->getOldPos().y);
		}

		if (relativePosition.beneath.isTrue && relativePosition.beneath.type == 1)
		{
			movingDown = false;
			canFall = false;

			spriteList[0]->setPos(spriteList[0]->getLastClearPos());
			if (onLadder)
			{
				spriteList[0]->setSpriteY(spriteList[0]->getOldPos().y - 2); //stops the player partially clipping through floor when on ladder
			}
		}

		if (relativePosition.left.isTrue && relativePosition.left.type == 1)
		{
			movingLeft = false;
			spriteList[0]->setSpriteX(spriteList[0]->getOldPos().x);
		}

		if (relativePosition.right.isTrue && relativePosition.right.type == 1)
		{
			movingRight = false;
			spriteList[0]->setSpriteX(spriteList[0]->getOldPos().x);
		}

		//add to score and remove sprite from level
		if (relativePosition.gainScore)
		{
			score += 10;
			levelSpriteList.erase(levelSpriteList.begin() + relativePosition.elementInArray);
		}

		//cant move up or down if not on ladder
		if (!onLadder)// && !(relativePosition.beneath.isTrue && relativePosition.beneath.type == 2))
		{
			movingUp = false;
			movingDown = false;
		}

		//stop player falling and jumping when on ladder
		if (onLadder)
		{
			canFall = false;
			jumping = false;
			if (!movingLeft && !movingRight)
			{
				spriteList[0]->setSpriteX(relativePosition.ladderCenter); //makes the player "stick" to the ladder so they dont clip into terrain when going up and down
			}
		}

		if (canFall && !jumping)
		{
			for (auto const& sprite : spriteList) //apply gravity to all sprites (will only actually apply gravity if it gravity is enabled on that sprite)
				sprite->gravity();
			jumping = false;
		}

		//if not falling
		if (!canFall)
		{
			if ((movingLeft || movingRight) && !onLadder) //moving left or right
				spriteList[0]->animateSprite(5, 5, 30, true);

			if (movingLeft && !movingRight) //moving left
			{
				spriteList[0]->moveSprite(Vector2(-playerSpeed, 0));
				//TODO check walkSound != nullptr
				if (Mix_Playing(1) != 1)
					Mix_PlayChannel(1, walkSound, 0);
			}

			if (movingRight && !movingLeft) //moving right
			{
				spriteList[0]->moveSprite(Vector2(playerSpeed, 0));
				if (Mix_Playing(1) != 1)
					Mix_PlayChannel(1, walkSound, 0);
			}

			if (movingUp || movingDown) //moving up or down
				spriteList[0]->animateSprite(11, 2, 10, true);

			if (movingUp && !movingDown) //moving up
			{
				spriteList[0]->moveSprite(Vector2(0, -playerSpeed));
				if (Mix_Playing(1) != 1)
					Mix_PlayChannel(1, walkSound, 0);
			}

			if (movingDown && !movingUp) //moving down
			{
				spriteList[0]->moveSprite(Vector2(0, playerSpeed));
				if (Mix_Playing(1) != 1)
					Mix_PlayChannel(1, walkSound, 0);
			}
		}

		if (jumping) //when jump is pressed
		{
			switch (spriteList[0]->jump(5, 60)) //speed and height of jump
			{
			case true: //if true is returned, player is still jumping
				break;

			case false: //if false is returned player has reached the max height of the jump and so is set to no longer be jumping
				jumping = false;
				break;
			}
		}

		if ((!movingRight && !movingLeft || movingLeft && movingRight) && (!movingUp && !movingDown || movingUp && movingDown)) //player not moving
		{
			if (onLadder)
			{
				spriteList[0]->animateSprite(12, 1, 5, true);
			}
			else
				spriteList[0]->setIdle();

			Mix_HaltChannel(1); //stops sound playing when stopping moving, in case half way through sound
		}

		//apply all movement changes
		spriteList[0]->updateMovement();*/

		Position4 relativePosition;
		Vector2 playerMovement(0, 0);
		if (movingRight)
		{
			playerMovement = { playerSpeed, 0 };
			relativePosition = CollisionHandler().CheckCollisions(spriteList[thisPlayer], playerMovement, levelSpriteList);
			spriteList[thisPlayer]->moveSprite(playerMovement);
		}

		else if (movingLeft)
		{
			playerMovement = { -playerSpeed, 0 };
			relativePosition = CollisionHandler().CheckCollisions(spriteList[thisPlayer], playerMovement, levelSpriteList);
			spriteList[thisPlayer]->moveSprite(playerMovement);
		}

		else if (movingUp)
		{
			playerMovement = {0, -playerSpeed};
			relativePosition = CollisionHandler().CheckCollisions(spriteList[thisPlayer], playerMovement, levelSpriteList);
			if (relativePosition.onLadder)
				spriteList[thisPlayer]->moveSprite(playerMovement);
		}

		else if (movingDown)
		{
			playerMovement = { 0, playerSpeed };
			relativePosition = CollisionHandler().CheckCollisions(spriteList[thisPlayer], playerMovement, levelSpriteList);
			if(relativePosition.onLadder)
				spriteList[thisPlayer]->moveSprite(playerMovement);
		}

		//not moving
		else
		{
			relativePosition = CollisionHandler().CheckCollisions(spriteList[thisPlayer], Vector2(0, 0), levelSpriteList);
		}

		//jumping
		if (jumping)
		{
			switch (spriteList[thisPlayer]->jump(5, 60)) //speed and height of jump
			{
			case true: //if true is returned, player is still jumping
				jumping = true;
				canJump = false;
				break;

			case false: //if false is returned player has reached the max height of the jump and so is set to no longer be jumping
				jumping = false;
				break;
			}
		}

		if (!jumping && !canJump)
		{
			if (relativePosition.beneath.type == 1)
			{
				canJump = true;
			}
		}

		//animation
		//if player is moving left or right, but not up or down and not on a ladder
		if (playerMovement.x != 0 && playerMovement.y == 0 && !relativePosition.onLadder)
		{
			spriteList[thisPlayer]->animateSprite(5, 5, 30, true);
		}

		//player moving up or down, not left or right and is on ladder
		else if (playerMovement.y != 0 && playerMovement.x == 0 && relativePosition.onLadder)
		{
			spriteList[thisPlayer]->animateSprite(11, 2, 10, true);
		}

		//not moving
		else
		{
			//set idle animations
			if (relativePosition.onLadder)
				spriteList[thisPlayer]->animateSprite(12, 1, 5, true);
			else
				spriteList[thisPlayer]->setIdle();
		}

		//add to score and remove sprite from level
		if (relativePosition.gainScore)
		{
			score += 10;
			levelSpriteList.erase(levelSpriteList.begin() + relativePosition.elementInArray);
		}

		spriteList[thisPlayer]->updateMovement(relativePosition);

		//timeScore decreases by 1 for every 0.5 second starting at 900, bonus goes down by 10 for every 5 timeScore that goes down starting at 1000	
		currentTime = Clock::now();
		deltaTime += std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - previousTime).count();
		deltaTime2 += std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - previousTime).count();

		if (deltaTime >= 500) //0.5 of a second
		{
			if (timeScore > 0)
				timeScore -= 1;

			if (bonusScore > 0)
				bonusScore -= 5;

			deltaTime = 0;
		}

		std::string timeScoreText = std::to_string(timeScore);
		std::string bonusScoreText = std::to_string(bonusScore);
		std::string scoreText = std::to_string(score);
		textList[5]->setText("Time " + timeScoreText);
		textList[4]->setText("Bonus " + bonusScoreText);

		std::string zeros = "";
		for (int i = scoreText.length(); i < 6; i++)
			zeros += "0";
		textList[1]->setText(zeros + scoreText);

		previousTime = Clock::now();
	}

	handleNetwork();
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

	//If paused, draw pause menu
	if (paused)
	{
		
		for (auto const& sprite : menuSpriteList) //loops through menu objects
		{
			sprite->drawSprite();
		}

		menuSpriteListSelected[menuItem]->drawSprite();
	}

	//Update the screen
	SDL_RenderPresent(ren);
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

void loadAssets()
{
	if (walkSound == nullptr)
	{
		walkSound = Mix_LoadWAV("./assets/player_footstep.ogg");
		if (walkSound == NULL)
		{
			std::cout << "Walk sound SDL_mixer Error: " << Mix_GetError() << std::endl;
			cleanExit(1);
		}

		Mix_VolumeChunk(walkSound, 50); //set volume of footsteps
	}
}

int main( int argc, char* args[] )
{
	std::cout << "argc was: " << argc << std::endl;
	if (argc > 1)
	{
		std::string s(args[1]);
		if (s == "--server")
		{
			ZMQserver = true;
			std::cout << "Running as SERVER" << std::endl;
			std::cout << "Args[1] was: \"" << args[1] << "\"" << std::endl;
		}
	}

	else
	{
		ZMQserver = false;
		std::cout << "Running as CLIENT" << std::endl;
	}

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

	//create window and error check
	if (!win.init())
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		cleanExit(1);
	}

	else
	{
		ren = win.createRenderer();
		if (ren == nullptr)
		{
			std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
			cleanExit(1);
		}
		Window::setRenderer(ren);
	}


	if (ZMQserver)
	{
		win.setWindowTitle("Server");
	}

	else
	{
		win.setWindowTitle("Client");
	}

	SDL_RenderSetLogicalSize(ren, winWidth, winHeight);
	SDL_SetRenderDrawColor(ren, 20, 49, 59, 255); //set background colour
	SDL_RenderClear(ren);

	SpriteHandler::setRenderer(ren); //set SpriteHandler renderer
	TextBox::setRenderer(ren); //set TextBox renderer

	//Showloading screen
	SDL_Rect rect = { 0, 0, winWidth, winHeight }; //size and position of sprite, x, y, w, h
	SDL_Rect spritePosRect = { 0, 0, winWidth, winHeight }; //position of sprite in spritesheet, x, y, w, h

	std::string imagePath = "./assets/loadingScreen.png"; //sprite image path
	SpriteHandler loadingScreen(rect, spritePosRect, imagePath, false, -1, 1);

	loadingScreen.drawSprite();
	SDL_RenderPresent(ren);
	

	//---- sprite begin ----//
	//---- player 1 begin ----//
	rect = {100, 800, 66, 92}; //size and position of sprite, x, y, w, h
	spritePosRect = {0, 0, 66, 92}; //position of sprite in spritesheet, x, y, w, h

	imagePath = "./assets/player_walk.png"; //sprite image path
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

	theString = "000000"; //score number
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
	bgMusic = Mix_LoadMUS("./assets/background_music.ogg");
	if (bgMusic == NULL)
	{
		std::cout << "Background music SDL_mixer Error: " << Mix_GetError() << std::endl;
		cleanExit(1);
	}

	//load other sounds
	
	if (Mix_PlayingMusic() == 0)
	{
		Mix_PlayMusic(bgMusic, -1);
	}
	//---- sound end ----//


	//---- network begin ----//
	if (ZMQserver)
	{
		this_zmq_publisher.bind("tcp://*:5556");
	}

	else
	{
		std::cout << "Subscribing to server ..." << std::endl;
		this_zmq_subscriber.connect("tcp://localhost:5556");
	}
	//---- network end ----//


	//---- menu begin ----//
	//pauseSurface = SDL_CreateRGBSurface(0, 700, 945, 32, 0, 0, 0, 0);
	//rect = { 0, 0, 700, 945 };
	//SDL_FillRect(pauseSurface, &rect, SDL_MapRGB(pauseSurface->format, 255, 0, 0));
	//pauseTex = SDL_CreateTextureFromSurface(ren, pauseSurface);

	int menuGap = 0;
	int increment = 50;
	int menuPosX = (int)winWidth / 2 - 85;
	int menuPosY = (int)winHeight / 2 - 100;

	spritePosRect = { 0, 0, 190, 45 }; 

	rect = { menuPosX, menuPosY + menuGap, 190, 45 }; 
	imagePath = "./assets/buttonResumeUp.png"; 
	menuSpriteList.push_back(std::unique_ptr<SpriteHandler>(new SpriteHandler(rect, spritePosRect, imagePath, false, -1, 1))); 
	menuGap += increment;

	rect = { menuPosX, menuPosY + menuGap, 190, 45 };
	imagePath = "./assets/buttonMusicUp.png"; 
	menuSpriteList.push_back(std::unique_ptr<SpriteHandler>(new SpriteHandler(rect, spritePosRect, imagePath, false, -1, 1)));
	menuGap += increment;

	rect = { menuPosX, menuPosY + menuGap, 190, 45 };
	imagePath = "./assets/buttonFullscreenUp.png"; 
	menuSpriteList.push_back(std::unique_ptr<SpriteHandler>(new SpriteHandler(rect, spritePosRect, imagePath, false, -1, 1)));
	menuGap += increment;

	rect = { menuPosX, menuPosY + menuGap, 190, 45 };
	imagePath = "./assets/buttonRestartUp.png"; 
	menuSpriteList.push_back(std::unique_ptr<SpriteHandler>(new SpriteHandler(rect, spritePosRect, imagePath, false, -1, 1)));
	menuGap += increment;

	rect = { menuPosX, menuPosY + menuGap, 190, 45 };
	imagePath = "./assets/buttonQuitUp.png";
	menuSpriteList.push_back(std::unique_ptr<SpriteHandler>(new SpriteHandler(rect, spritePosRect, imagePath, false, -1, 1)));
	menuGap += increment;

	//pressed buttons
	menuGap = 0;

	rect = { menuPosX, menuPosY + menuGap, 190, 45 }; 
	imagePath = "./assets/buttonResumeDown.png"; 
	menuSpriteListSelected.push_back(std::unique_ptr<SpriteHandler>(new SpriteHandler(rect, spritePosRect, imagePath, false, -1, 1)));
	menuGap += increment;

	rect = { menuPosX, menuPosY + menuGap, 190, 45 };
	imagePath = "./assets/buttonMusicDown.png"; 
	menuSpriteListSelected.push_back(std::unique_ptr<SpriteHandler>(new SpriteHandler(rect, spritePosRect, imagePath, false, -1, 1)));
	menuGap += increment;

	rect = { menuPosX, menuPosY + menuGap, 190, 45 };
	imagePath = "./assets/buttonFullscreenDown.png"; 
	menuSpriteListSelected.push_back(std::unique_ptr<SpriteHandler>(new SpriteHandler(rect, spritePosRect, imagePath, false, -1, 1)));
	menuGap += increment;

	rect = { menuPosX, menuPosY + menuGap, 190, 45 };
	imagePath = "./assets/buttonRestartDown.png"; 
	menuSpriteListSelected.push_back(std::unique_ptr<SpriteHandler>(new SpriteHandler(rect, spritePosRect, imagePath, false, -1, 1)));
	menuGap += increment;

	rect = { menuPosX, menuPosY + menuGap, 190, 45 };
	imagePath = "./assets/buttonQuitDown.png"; 
	menuSpriteListSelected.push_back(std::unique_ptr<SpriteHandler>(new SpriteHandler(rect, spritePosRect, imagePath, false, -1, 1)));
	menuGap += increment;
	//---- menu end ----//


	//delay to display loading screen
	SDL_Delay(2000);

	while (!done) //loop until done flag is set)
	{
		auto time = Clock::now(); //used for FPS limiter

		loadAssets();

		handleInput(); // this should ONLY SET VARIABLES

		if(!paused)
			updateSimulation(); // this should ONLY SET VARIABLES according to simulation

		render(); // this should render the world state according to VARIABLES

		fpsLimiter(time); //always call after all other functions
	}

	cleanExit(0);
	return 0;
}
