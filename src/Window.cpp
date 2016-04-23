#include "Window.h"

SDL_Renderer* Window::_ren = nullptr;

Window::Window()
{

}

SDL_Renderer* Window::createRenderer()
{
	return SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void Window::eventHandler(SDL_Event& evnt)
{
	if (evnt.type == SDL_WINDOWEVENT)
	{
		switch (evnt.window.event)
		{
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			_winWidth = evnt.window.data1;
			_winHeight = evnt.window.data2;
			SDL_RenderPresent(_ren);
			break;

		case SDL_WINDOWEVENT_EXPOSED:
			SDL_RenderPresent(_ren);
			break;

		case SDL_WINDOWEVENT_ENTER:
			_isMouseFocused = true;
			break;

		case SDL_WINDOWEVENT_LEAVE:
			_isMouseFocused = false;
			break;

		case SDL_WINDOWEVENT_FOCUS_GAINED:
			_isKbFocused = true;
			break;

		case SDL_WINDOWEVENT_FOCUS_LOST:
			_isKbFocused = false;
			break;

		case SDL_WINDOWEVENT_MINIMIZED:
			_minimised = true;
			break;

		case SDL_WINDOWEVENT_MAXIMIZED:
			_minimised = false;
			break;

		case SDL_WINDOWEVENT_RESTORED:
			_minimised = false;
			break;
		}
	}
}

void Window::fullscreenToggle()
{
	if (_fullscreen)
	{
		SDL_SetWindowFullscreen(_window, SDL_FALSE);
		_fullscreen = false;
	}
	else
	{
		SDL_SetWindowFullscreen(_window, SDL_TRUE);
		_fullscreen = true;
		_minimised = false;
	}
}

bool Window::init()
{
	_window = SDL_CreateWindow("My Game", 100, 100, 700, 945, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	//error checking
	if (_window == nullptr)
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
	}
	else
	{
		_isMouseFocused = true;
		_isKbFocused = true;
		_winWidth = 700;
		_winHeight = 945;
	}

	return _window != NULL;
}

int Window::getScreenWidth()
{
	return _winWidth;
}

int Window::getScreenHeight()
{
	return _winHeight;
}

bool Window::mouseFocused()
{
	return _isMouseFocused;
}

bool Window::kbFocused()
{
	return _isKbFocused;
}

bool Window::isMinimised()
{
	return _minimised;
}

void Window::setRenderer(SDL_Renderer* renderer)
{
	_ren = renderer;
}

void Window::setWindowTitle(std::string title)
{
	SDL_SetWindowTitle(_window, title.c_str());
}

Window::~Window()
{
	if (_ren != nullptr)
		SDL_DestroyRenderer(_ren);
}