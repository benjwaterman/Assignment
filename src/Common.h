#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <vector>
#include <memory>
#include <chrono>

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