#include "Entity.h"
#include "Renderer.h"

Entity::Entity(const char* texturePath, int x, int y, int width, int height, float vel) {
    this->texturePath = texturePath;
    xPosition = x;
    yPosition = y;
    this->width = width;
    this->height = height;
    velocity = vel;
}

Entity::~Entity() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

void Entity::setSize(int width, int height) {
    this->width = width;
    this->height = height;
}

void Entity::setPosition(int x, int y) {
    xPosition = x;
    yPosition = y;
}

void Entity::setVelocity(float vel) {
    velocity = vel;
}

bool Entity::loadTexture(SDL_Renderer* renderer) {
    // load the surface from the texture path
    SDL_Surface* surface = SDL_LoadBMP(texturePath);
    if (!surface) {
        return false;
    }
    //transform the surface into a texture, more efficient for rendering
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture != nullptr;
}

void Entity::render(SDL_Renderer* renderer) {
    SDL_Rect dstRect = { xPosition, yPosition, width, height };
    SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
}