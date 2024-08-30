#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL/SDL.h>
#endif

// Class, functions, varible signatures of the Rendere class. This class manages the renderer.
class Renderer {
public:
	Renderer();
	~Renderer();

	bool initialize(SDL_Window* window);
	void clear();
	void present();
	void shutdown();

private:
	SDL_Renderer* _renderer;	
};
