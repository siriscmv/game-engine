#include "Renderer.h"
#include <SDL/SDL.h>

// Constructor for the Renderer class.Initializes class variables.
Renderer::Renderer() {	
	_renderer = nullptr;
}

Renderer::~Renderer() {
	shutdown();
}

// Initializes the renderer. Creates a renderer object using the window passed into this function
bool Renderer::initialize(SDL_Window* window) {	
	_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!_renderer) {
		SDL_Log("Could not create renderer: %s", SDL_GetError());
		return false;
	}

	// Setting background color to blue
	SDL_SetRenderDrawColor(_renderer, 0, 0, 255, 255);
	return true;
}

// Erases everything drawn in the screen
void Renderer::clear() {
	SDL_RenderClear(_renderer);
}

// Draws the content on the screen
void Renderer::present() {
	SDL_RenderPresent(_renderer);
}

// Destroys the renderer object and sets it to null pointer.
void Renderer::shutdown() {
	if (_renderer) {
		SDL_DestroyRenderer(_renderer);
		_renderer = nullptr;
	}
}
