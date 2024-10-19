#pragma once

#include "Renderer.h"
#include "Globals.h"
#include <utility>
#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL/SDL.h>
#endif


// Entity class. Represents an object drawn on the screen.
class Entity {
public:    
    Entity(Position position, Size size, SDL_Color color = { 255, 0, 0, 255 });                          // Rectangles
    Entity(Position position, float radius, SDL_Color color = { 255, 0, 0, 255 });                       // Circles
    Entity(Position position, float baseLength, float height, SDL_Color color = { 255, 0, 0, 255 });     // Triangles
    Entity(const char *texturePath, Position position, Size size);                                       // Textured entities

    ~Entity();

    // Setters    
    void setEntityID(int id);
    void setSize(Size size);
    void setOriginalSize(Size size);
    void setPosition(Position position);
    void setOriginalPosition(Position position);
    void setEntityType(EntityType entityType);
    void setZoneType(ZoneType zoneType);
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
    int getEntityID() const;
    Size getSize() const;
    Position getPosition() const;
    Position getOriginalPosition() const;
    EntityType getEntityType() const;
    ZoneType getZoneType() const;
    ShapeType getShapeType() const;
    float getVelocityX() const;
    float getVelocityY() const;
    float getAccelerationX() const;
    float getAccelerationY() const;
    float getCircleRadius() const;
    float getTriangleBaseLength() const;
    float getTriangleHeight() const;

    void generateEntityID();
    bool loadTexture(SDL_Renderer *renderer);                                   // Load texture into entity
    void render(SDL_Renderer *renderer, const Camera& camera);                  // Render entity 
    void applyScaling(float scaleX, float scaleY);        
    bool isWithinViewPort(const Camera& camera) const;
    void shutdown();

private:
    Position _position = {};
    Size _size = {};
    EntityType _entityType = EntityType::DEFAULT;
    ZoneType _zoneType = ZoneType::NONE;
    ShapeType _shape = ShapeType::NONE;
    Velocity _velocity = {};
    Acceleration _acceleration = {};
    SDL_Color _color;

    float _circleRadius = 0.0f;
    float _triangleBaseLength = 0.0f;
    float _triangleHeight = 0.0f;

    // Variables to hold original sizes (used in screen scaling)
    Position _originalPosition = {};
    Size _originalSize = {};
    float _originalCircleRadius = 0.0f;  
    float _originalTriangleBaseLength = 0.0f;  
    float _originalTriangleHeight = 0.0f;  

    const char* _texturePath = nullptr;                  // Path of the texture file
    SDL_Texture* _texture = nullptr;                     // Texture of the entity

    void drawRectangle(SDL_Renderer* renderer, Position position);
    void drawCircle(SDL_Renderer* renderer, Position position);
    void drawTriangle(SDL_Renderer* renderer, Position position);

    int _entityID;                                       // Unique ID of the entity
    static int _nextID;                                  // Variable to track next available ID
};