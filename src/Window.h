#pragma once
#include "Common.h"

class Window
{
public:
	Window();
	SDL_Renderer* createRenderer();
	void eventHandler(SDL_Event& evnt);
	void fullscreenToggle();
	bool init();
	int getScreenWidth();
	int getScreenHeight();
	bool mouseFocused();
	bool kbFocused();
	bool isMinimised();
	static void setRenderer(SDL_Renderer* renderer);
	void setWindowTitle(std::string title);
	~Window();

private:
	SDL_Window* _window;
	static SDL_Renderer* _ren;

	int _winHeight = 0;
	int _winWidth = 0;

	bool _isMouseFocused = false;
	bool _isKbFocused = false;
	bool _fullscreen = false;
	bool _minimised = false;
};
