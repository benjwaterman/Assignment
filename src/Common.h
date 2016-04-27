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
		hitEnemy = false;
		ladderCenter = 0;
		elementInArray = -1;
	}

	PositionInfo above, beneath, left, right;
	bool onLadder;
	bool gainScore;
	bool hitEnemy;

	int ladderCenter;
	int elementInArray;
};

struct Vector2
{
	Vector2()
	{
		x = y = 0.0f;
	}

	Vector2(double _x, double _y)
	{
		x = _x;
		y = _y;
	}

	double x, y;
};

struct Vector4
{
	Vector4()
	{
		x = y = w = h = 0;
	}

	Vector4(double _x, double _y, double _w, double _h)
	{
		x = _x;
		y = _y;
		w = _w;
		h = _h;
	}

	double x, y, w, h;
};