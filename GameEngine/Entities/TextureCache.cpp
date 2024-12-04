#include "TextureCache.h"
#include <stdexcept>

std::unordered_map<std::string, SDL_Texture*> TextureCache::cache; 

SDL_Texture* TextureCache::getTexture(SDL_Renderer* renderer, const std::string& path) {
    if (cache.find(path) == cache.end()) {
        SDL_Surface* surface = SDL_LoadBMP(path.c_str());
        if (!surface) {
            throw std::runtime_error("Failed to load BMP: " + path + ". SDL Error: " + std::string(SDL_GetError()));
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        if (!texture) {
            throw std::runtime_error("Failed to create texture from BMP: " + path + ". SDL Error: " + std::string(SDL_GetError()));
        }

        cache[path] = texture;
    }

    return cache[path];
}

void TextureCache::cleanup() {
    for (auto& [path, texture] : cache) {
        SDL_DestroyTexture(texture);
    }
    cache.clear();
}
