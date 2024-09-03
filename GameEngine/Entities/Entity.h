#pragma once

#include "Renderer.h"
#include <utility>
#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL/SDL.h>
#endif


// An enum class to manage the shape of the entity
enum class ShapeType {
    NONE,
    RECTANGLE,
    TRIANGLE,
    CIRCLE,
    TEXTURE
};

// An enum class that denotes the type of entity
enum class EntityType {
    DEFAULT, // Is affected by collisions
    FIXED, // Is unaffected by collisions
    ELASTIC // A collision causes a reversal in movement direction
};

// structure to hold position 
struct Position {
    float x;
    float y;
    Position(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
};

// Structure to hold size 
struct Size {
    float width;
    float height;
    Size(float width = 0.0f, float height = 0.0f) : width(width), height(height) {}
};

// Structure to hold velocity
struct Velocity {
    float x;
    float y;
    Velocity(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
};

// Structure to hold acceleration
struct Acceleration {
    float x;
    float y;
    Acceleration(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
};

// Entity class. Represents an object drawn on the screen.
class Entity {
public:    
    Entity(Position position, Size size, SDL_Color color = { 255, 0, 0, 255 });                          // Rectangles
    Entity(Position position, float radius, SDL_Color color = { 255, 0, 0, 255 });                       // Circles
    Entity(Position position, float baseLength, float height, SDL_Color color = { 255, 0, 0, 255 });     // Triangles
    Entity(const char *texturePath, Position position, Size size);                                       // Textured entities

    ~Entity();

    // Setters
    void setSize(Size size);
    void setOriginalSize(Size size);
    void setPosition(Position position);
    void setEntityType(EntityType entityType);
    void setShapeType(ShapeType shapeType);
    void setVelocityX(float velocityX);
    void setVelocityY(float velocityY);
    void setAccelerationX(float accelerationX);
    void setAccelerationY(float accelerationY);
    void setCircleRadius(float radius);
    void setOriginalCircleRadius(float radius);
    void setTriangleBaseLength(float baseLength);
    void setOriginalTriangleBaseLength(float baseLength);
    void setTriangleHeight(float height);
    void setOriginalTriangleHeight(float height);
    void setColor(SDL_Color color);

    // Getters
    Size getSize() const;
    Position getPosition() const;
    EntityType getEntityType() const;
    ShapeType getShapeType() const;
    float getVelocityX() const;
    float getVelocityY() const;
    float getAccelerationX() const;
    float getAccelerationY() const;
    float getCircleRadius() const;
    float getTriangleBaseLength() const;
    float getTriangleHeight() const;


    bool loadTexture(SDL_Renderer *renderer);             // Load texture into entity
    void render(SDL_Renderer *renderer);                  // Render entity 
    void applyScaling(float scaleX, float scaleY);        
    void shutdown();

private:
    Position _position = {};
    Size _size = {};
    EntityType _entityType = EntityType::DEFAULT;
    ShapeType _shape = ShapeType::NONE;
    Velocity _velocity = {};
    Acceleration _acceleration = {};
    SDL_Color _color;

    float _circleRadius = 0.0f;
    float _triangleBaseLength = 0.0f;
    float _triangleHeight = 0.0f;

    // Variables to hold original sizes (used in screen scaling)
    Size _originalSize = {};
    float _originalCircleRadius = 0.0f;  
    float _originalTriangleBaseLength = 0.0f;  
    float _originalTriangleHeight = 0.0f;  

    const char* _texturePath = nullptr;                  // Path of the texture file
    SDL_Texture* _texture = nullptr;                     // Texture of the entity

    void drawRectangle(SDL_Renderer* renderer);
    void drawCircle(SDL_Renderer* renderer);
    void drawTriangle(SDL_Renderer* renderer);
};