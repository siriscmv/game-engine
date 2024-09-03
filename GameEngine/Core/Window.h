#pragma once

#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL/SDL.h>
#include <utility>
#endif

// Enum class to track the scaling mode
// CONSTANT: Enity sizes won't change on screen resizing
// PROPORTIONAL: Entity sizes will scale proportionally to screen size
enum class ScalingMode { CONSTANT, PROPORTIONAL };            

// Class, functions, varible signatures of the Window class. This class manages window creation.
class Window {
public:
	Window(const char* windowTitle, int windowWidth, int windowHeight);
	~Window();

	bool initialize();
	void shutdown();
	SDL_Window* getSDLWindow() const;
	void toggleScalingMode();
	std::pair<float, float> getScaleFactors() const;

private:
	const char* _windowTitle;
	int _windowWidth;
	int _windowHeight;
	SDL_Window* _window;
	ScalingMode _scalingMode;
};