#include "Window.h"

#include <utility>
#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL/SDL.h>
#include <utility>
#endif

// Constructor for the Window class. Initializes class variables.
Window::Window(const char* windowTitle, int windowWidth, int windowHeight) {
	_windowTitle = windowTitle;
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
	_scalingMode = ScalingMode::CONSTANT;
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
		SDL_Log("Could not create window : %s", SDL_GetError());
		return false;
	}

	return true;
}

// Toggles the scaling mode variable
void Window::toggleScalingMode() {
	if (_scalingMode == ScalingMode::CONSTANT)
		_scalingMode = ScalingMode::PROPORTIONAL;
	else
		_scalingMode = ScalingMode::CONSTANT;
}

// Returns the scaling factors based on current screen size and scaling mode
std::pair<float, float> Window::getScaleFactors() const {
	int windowWidth, windowHeight;
	SDL_GetWindowSize(_window, &windowWidth, &windowHeight);

	float scaleX = 1.0f;
	float scaleY = 1.0f;

	if (_scalingMode == ScalingMode::PROPORTIONAL) {
		scaleX = static_cast<float>(windowWidth) / _windowWidth;
		scaleY = static_cast<float>(windowHeight) / _windowHeight;
	}
	
	return { scaleX, scaleY };
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