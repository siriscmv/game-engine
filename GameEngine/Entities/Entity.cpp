#include "Entity.h"
#include "Renderer.h"
#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL/SDL.h>
#endif
#include  <iostream>

// Constructor for Rectangles
Entity::Entity(Position position, Size size, SDL_Color color) {
    setPosition(position);
    setSize(size);
    setShapeType(ShapeType::RECTANGLE);
    setColor(color);
}

// Contructor for Circles
Entity::Entity(Position position, float radius, SDL_Color color) {
    setPosition(position);
    setCircleRadius(radius);
    setShapeType(ShapeType::CIRCLE);
    setColor(color);
}

// Constructor for triangles
Entity::Entity(Position position, float baseLength, float height, SDL_Color color) {
    setPosition(position);
    setTriangleBaseLength(baseLength);
    setTriangleHeight(height);
    setShapeType(ShapeType::TRIANGLE);
    setColor(color);
}

// Constructor for textured entities
Entity::Entity(const char *texturePath, Position position, Size size) {
    _texturePath = texturePath;
    setPosition(position);
    setSize(size);
    setShapeType(ShapeType::TEXTURE);
}

Entity::~Entity() {
    shutdown();
}

// Setters
void Entity::setPosition(Position position) { _position = position; }
void Entity::setSize(Size size) { _size = size; }
void Entity::setEntityType(EntityType entityType) { _entityType = entityType; }
void Entity::setShapeType(ShapeType shape) { _shape = shape; }
void Entity::setVelocityX(float velocityX) { _velocity.x = velocityX; }
void Entity::setVelocityY(float velocityY) { _velocity.y = velocityY; }
void Entity::setAccelerationX(float accelerationX) { _acceleration.x = accelerationX; }
void Entity::setAccelerationY(float accelerationY) { _acceleration.y = accelerationY; }
void Entity::setCircleRadius(float radius) { _circleRadius = radius; }
void Entity::setTriangleBaseLength(float baseLength) { _triangleBaseLength = baseLength; }
void Entity::setTriangleHeight(float height) { _triangleHeight = height; }
void Entity::setColor(SDL_Color color) { _color = color; }

// Getters
Position Entity::getPosition() const { return _position; }
Size Entity::getSize() const { return _size; }
EntityType Entity::getEntityType() const { return _entityType; }
ShapeType Entity::getShapeType() const { return _shape; }
float Entity::getVelocityX() const { return _velocity.x; }
float Entity::getVelocityY() const { return _velocity.y; }
float Entity::getAccelerationX() const { return _acceleration.x; }
float Entity::getAccelerationY() const { return _acceleration.y; }
float Entity::getCircleRadius() const { return _circleRadius; }
float Entity::getTriangleBaseLength() const { return _triangleBaseLength; }
float Entity::getTriangleHeight() const { return _triangleHeight; }



void Entity::drawRectangle(SDL_Renderer *renderer) {    
    // Print the current position and size of the rectangle    
    SDL_Rect rect = {_position.x, _position.y, _size.width, _size.height};
    SDL_SetRenderDrawColor(renderer, _color.r, _color.g, _color.b, _color.a);
    SDL_RenderFillRect(renderer, &rect);
}

void Entity::drawCircle(SDL_Renderer *renderer) {

    // using the SDL_SetRenderDrawColor function to set the color of the circle
    SDL_SetRenderDrawColor(renderer, _color.r, _color.g, _color.b, _color.a);
    // looping through the circle and drawing the points
    for (int w = 0; w < _circleRadius * 2; w++)     {
        for (int h = 0; h < _circleRadius * 2; h++)         {
            int dx = _circleRadius - w; // horizontal offset
            int dy = _circleRadius - h; // vertical offset
            if ((dx * dx + dy * dy) <= (_circleRadius * _circleRadius))             {
                SDL_RenderDrawPoint(renderer, _position.x + dx, _position.y + dy);
            }
        }
    }
}

void Entity::drawTriangle(SDL_Renderer *renderer) {
    // using the SDL_SetRenderDrawColor function to set the color of the triangle
    SDL_SetRenderDrawColor(renderer, _color.r, _color.g, _color.b, _color.a);
    // creating an array of points to draw the triangle
    SDL_Point points[3] = {
        {_position.x, _position.y},
        {_position.x + _triangleBaseLength, _position.y},
        {_position.x + _triangleBaseLength / 2, _position.y - _triangleHeight}};
    // using the SDL_RenderDrawLines function to draw the triangle
    SDL_RenderDrawLines(renderer, points, 3);
}

// Loads a texture 
bool Entity::loadTexture(SDL_Renderer *renderer) {
    // load the surface from the texture path
    SDL_Surface *surface = SDL_LoadBMP(_texturePath);
    if (!surface) {
        return false;
    }
    // transform the surface into a texture, more efficient for rendering
    _texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return _texture != nullptr;
}

// Renders the entity onto the screen
void Entity::render(SDL_Renderer *renderer) {   

    switch (_shape) {
        case ShapeType::TEXTURE: 
            if (_texture != nullptr) {
                SDL_Rect dstRect = { _position.x, _position.y, _size.width, _size.height };
                SDL_RenderCopy(renderer, _texture, nullptr, &dstRect);
            }
            else {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, 
                    "Failed to render entity: Texture is null at position (%f, %f)\n", _position.x, _position.y);
            }
            break;
        
        case ShapeType::RECTANGLE:
            drawRectangle(renderer);
            break;
        
        case ShapeType::CIRCLE:
            drawCircle(renderer);
            break;
        
        case ShapeType::TRIANGLE:
            drawTriangle(renderer);
            break;
        default:
            break;
    }
}

// Cleans up class variables
void Entity::shutdown() {
    if (_texture) {
        SDL_DestroyTexture(_texture);
        _texture = nullptr;
    }
}