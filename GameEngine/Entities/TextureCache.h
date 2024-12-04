#pragma once

#include <SDL/SDL.h>
#include <string>
#include <unordered_map>

class TextureCache {
public:
    static SDL_Texture* getTexture(SDL_Renderer* renderer, const std::string& path);
    static void cleanup();

private:
    static std::unordered_map<std::string, SDL_Texture*> cache; 
};