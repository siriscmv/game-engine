#include <SDL/SDL.h>

// Class, functions, varible signatures of the Window class. This class manages window creation.
class Window {
public:
	Window(const char* windowTitle, int windowWidth, int windowHeight);
	~Window();

	bool initialize();
	void shutdown();
	SDL_Window* getSDLWindow() const;

private:
	const char* _windowTitle;
	int _windowWidth;
	int _windowHeight;
	SDL_Window* _window;
};