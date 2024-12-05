#pragma once

#include <string>
#include <unordered_map>

#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL/SDL.h>
#endif

class TextureCache {
public:
    static SDL_Texture* getTexture(SDL_Renderer* renderer, const std::string& path);
    static void cleanup();

private:
    static std::unordered_map<std::string, SDL_Texture*> cache; 
};