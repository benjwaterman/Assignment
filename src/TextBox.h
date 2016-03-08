#pragma once
#include "Common.h"

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

