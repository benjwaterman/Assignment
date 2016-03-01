#include "TextBox.h"

SDL_Renderer* TextBox::_ren = nullptr;

TextBox::TextBox()
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "(This should never be called) Text Constructed(%p)", this);
}

TextBox::TextBox(std::string theString, TTF_Font* theFont, SDL_Color theColour, SDL_Rect messageRect)
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Text Constructed(%p)", this);

	//ASSERT or check that ren is not nullptr
	_theString = theString;
	_theFont = theFont;
	_theColour = theColour;

	_messageSurface = TTF_RenderText_Solid(_theFont, _theString.c_str(), _theColour); //text 
	_messageTexture = SDL_CreateTextureFromSurface(TextBox::_ren, _messageSurface);
	_messageRect = messageRect;

}

void TextBox::setRenderer(SDL_Renderer* renderer)
{
	_ren = renderer;
}

void TextBox::drawText()
{
	SDL_RenderCopy(_ren, _messageTexture, NULL, &_messageRect);
}

TextBox::~TextBox()
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Text Destructed(%p)", this);

	if (_messageTexture != nullptr) 
		SDL_DestroyTexture(_messageTexture);
}
