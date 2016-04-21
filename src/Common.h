#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <sstream>
#include "zmq.hpp"

#ifdef _WIN32 // compiling on windows
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#else // NOT compiling on windows
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#endif

#include <SDL_mixer.h>

//custom data structures
struct PositionInfo
{
	PositionInfo()
	{
		type = 0;
		isTrue = false;
	}

	int type;
	bool isTrue;
};

struct Position4
{
	Position4()
	{
		onLadder = false;
		gainScore = false;
		ladderCenter = 0;
		elementInArray = -1;
	}

	PositionInfo above, beneath, left, right;
	bool onLadder;
	bool gainScore;

	int ladderCenter;
	int elementInArray;
};

struct Vector2
{
	Vector2()
	{
		x = y = 0;
	}

	Vector2(int _x, int _y)
	{
		x = _x;
		y = _y;
	}

	int x, y;
};