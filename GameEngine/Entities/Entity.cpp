#include "Entity.h"
#include "Renderer.h"

Entity::Entity(const char *texturePath, int x, int y, int width, int height, float vel, float accel)
{
    this->texturePath = texturePath;
    xPosition = x;
    yPosition = y;
    this->width = width;
    this->height = height;
    velocity = vel;
    acceleration = accel;
}

Entity::~Entity()
{
    if (texture)
    {
        SDL_DestroyTexture(texture);
    }
}

void Entity::setSize(int width, int height)
{
    this->width = width;
    this->height = height;
}

void Entity::setPosition(int x, int y)
{
    xPosition = x;
    yPosition = y;
}

void Entity::setVelocity(float vel)
{
    velocity = vel;
}

void Entity::setAcceleration(float acc)
{
    acceleration = acc;
}

int Entity::getWidth()
{
    return width;
}

int Entity::getHeight()
{
    return height;
}

int Entity::getXPosition()
{
    return xPosition;
}

int Entity::getYPosition()
{
    return yPosition;
}

float Entity::getVelocity()
{
    return velocity;
}

float Entity::getAcceleration()
{
    return acceleration;
}

void drawRect(SDL_Renderer *renderer, std::pair<int, int> position, std::pair<int, int> size, SDL_Color color)
{
    SDL_Rect rect = {position.first, position.second, size.first, size.second};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

void drawCircle(SDL_Renderer *renderer, std::pair<int, int> position, int radius, SDL_Color color)
{

    // using the SDL_SetRenderDrawColor function to set the color of the circle
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    // looping through the circle and drawing the points
    for (int w = 0; w < radius * 2; w++)
    {
        for (int h = 0; h < radius * 2; h++)
        {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx * dx + dy * dy) <= (radius * radius))
            {
                SDL_RenderDrawPoint(renderer, position.first + dx, position.second + dy);
            }
        }
    }
}

void drawTriangle(SDL_Renderer *renderer, std::pair<int, int> position, int base, int height, SDL_Color color)
{
    // using the SDL_SetRenderDrawColor function to set the color of the triangle
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    // creating an array of points to draw the triangle
    SDL_Point points[3] = {
        {position.first, position.second},
        {position.first + base, position.second},
        {position.first + base / 2, position.second - height}};
    // using the SDL_RenderDrawLines function to draw the triangle
    SDL_RenderDrawLines(renderer, points, 3);
}
bool Entity::loadTexture(SDL_Renderer *renderer)
{
    // load the surface from the texture path
    SDL_Surface *surface = SDL_LoadBMP(texturePath);
    if (!surface)
    {
        return false;
    }
    // transform the surface into a texture, more efficient for rendering
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture != nullptr;
}

void Entity::render(SDL_Renderer *renderer)
{
    SDL_Rect dstRect = {xPosition, yPosition, width, height};
    SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
}