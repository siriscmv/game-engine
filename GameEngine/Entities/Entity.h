#pragma once

#include "Renderer.h"

class Entity {
public:
    // Constructor that takes path to a texture, x and y position, width and height of the entity, and velocity
    Entity(const char* texturePath, int x, int y, int width, int height, float vel);

    ~Entity();

    void setSize(int width, int height);
    void setPosition(int x, int y);
    void setVelocity(float vel); 

    bool loadTexture(SDL_Renderer* renderer);
    void render(SDL_Renderer* renderer); // render entity
private:
    int xPosition;
    int yPosition;
    float velocity;
    int width;   // Width in pixels
    int height;  // Height in pixels

    const char* texturePath; // Path of the texture file
    SDL_Texture* texture; // Texture of the entity
};