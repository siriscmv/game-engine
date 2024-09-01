#pragma once

#include "Entity.h"
#include <SDL/SDL_scancode.h>

class MovableEntity : public Entity {
public:
    MovableEntity(const char* texturePath, int x, int y, int width, int height, float vel, float acc);
    ~MovableEntity();

    void updateXPosition();
    void updateYPosition();
    void updateFromInput(SDL_Scancode);

private:
    float acceleration;
    int xPosition;
    int yPosition;
    float velocity;
    int width;   // Width in pixels
    int height;  // Height in pixels

    const char* texturePath; // Path of the texture file
    SDL_Texture* texture; // Texture of the entity
};