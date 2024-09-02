#pragma once

#include "Renderer.h"
#include <utility>

class Entity
{
public:
    // Constructor that takes path to a texture, x and y position, width and height of the entity, and velocity
    Entity(const char *texturePath, int x, int y, int width, int height, float vel, float acc);

    ~Entity();

    void setSize(int width, int height);
    void setPosition(int x, int y);
    void setVelocity(float vel);
    void setAcceleration(float acc);
    int getWidth() {}
    int getHeight() {}
    int getXPosition() {}
    int getYPosition() {}
    float getVelocity() {}
    float getAcceleration() {}
    bool loadTexture(SDL_Renderer *renderer); // load texture into entity
    void render(SDL_Renderer *renderer);      // render entity

    // render a rectangle onto the window
    void drawRect(SDL_Renderer *renderer, std::pair<int, int> position, std::pair<int, int> size, SDL_Color color);
    // render a circle onto the window
    void drawCircle(SDL_Renderer *renderer, std::pair<int, int> position, int radius, SDL_Color color);
    // render a triangle onto the window
    void drawTriangle(SDL_Renderer *renderer, std::pair<int, int> position, int base, int height, SDL_Color color);

private:
    int width;  // Width in pixels
    int height; // Height in pixels
    int xPosition;
    int yPosition;
    float velocity;
    float acceleration;

    const char *texturePath; // Path of the texture file
    SDL_Texture *texture;    // Texture of the entity
};