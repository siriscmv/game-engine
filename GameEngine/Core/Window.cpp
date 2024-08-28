#include "Window.h"
#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL/SDL.h>
#endif

// Constructor for the Window class. Initializes class variables.
Window::Window(const char* windowTitle, int windowWidth, int windowHeight) {
	_windowTitle = windowTitle;
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
	_window = nullptr;	
}

// Destructor
Window::~Window() {
	shutdown();
}

// Window initialization. Logs error and returns 'false' if fails. Returns 'true' otherwise.
bool Window::initialize() {

	// Initializing the video subsytem. Logs error on failure.
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	// Creating the actual window
	_window = SDL_CreateWindow(_windowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		_windowWidth, _windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	// If window creation failed, logs error.
	if (!_window) {
		SDL_Log("Could not create window : % s", SDL_GetError());
		return false;
	}

	return true;
}

// Destroys the window and sets the class variable to null pointer.
void Window::shutdown() {
	if (_window) {
		SDL_DestroyWindow(_window);
		_window = nullptr;
	}
}

// Getter for the window object. 
SDL_Window* Window::getSDLWindow() const {
	return _window;
}