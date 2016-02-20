#pragma once
#include <string>

#ifdef _WIN32 // compiling on windows
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#else // NOT compiling on windows
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#endif

class TextBox
{
public:
	TextBox();
	TextBox(std::string theString, TTF_Font* theFont, SDL_Color theColor, SDL_Rect messageRect);
	static void setRenderer(SDL_Renderer* renderer);
	void drawText();
	~TextBox();
	
private:
	std::string _theString;
	TTF_Font* _theFont;
	SDL_Color _theColour;

	//renderer
	static SDL_Renderer *_ren;
	
	//text
	SDL_Surface *_messageSurface;
	SDL_Texture *_messageTexture;
	SDL_Rect _messageRect;
};

