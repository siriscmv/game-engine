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
    Entity(Position position, Size size, SDL_Color color = { 255, 0, 0, 255 }, bool hidden = false );                          // Rectangles
    Entity(Position position, float radius, SDL_Color color = { 255, 0, 0, 255 }, bool hidden = false);                       // Circles
    Entity(Position position, float baseLength, float height, SDL_Color color = { 255, 0, 0, 255 }, bool hidden = false);     // Triangles
    Entity(const char *texturePath, Position position, Size size, bool hidden = false);                                       // Textured entities

    ~Entity();

    // Setters    
    void setEntityID(int id);
    void setSize(Size size);
    void setOriginalSize(Size size);
    void setPosition(Position position);
    void setOriginalPosition(Position position);
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
    void setHidden(bool hidden);


    // Getters
    int getEntityID() const;
    Size getSize() const;
    Position getPosition() const;
    Position getOriginalPosition() const;
    EntityType getEntityType() const;
    ShapeType getShapeType() const;
    float getVelocityX() const;
    float getVelocityY() const;
    float getAccelerationX() const;
    float getAccelerationY() const;
    float getCircleRadius() const;
    float getTriangleBaseLength() const;
    float getTriangleHeight() const;
    bool getHidden() const;
    SDL_Color getColor() const;

    void generateEntityID();
    bool loadTexture(SDL_Renderer *renderer);             // Load texture into entity
    void render(SDL_Renderer *renderer);                  // Render entity 
    void applyScaling(float scaleX, float scaleY, Position offset = Position(0, 0), int entityId = -1);
    void teleportTo(const Position& position);
    void shutdown();

private:
    Position _position = {};
    Size _size = {};
	// help with entity visibility
    bool _hidden;
    EntityType _entityType = EntityType::DEFAULT;
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

    void drawRectangle(SDL_Renderer* renderer);
    void drawCircle(SDL_Renderer* renderer);
    void drawTriangle(SDL_Renderer* renderer);

    int _entityID;                                       // Unique ID of the entity
    static int _nextID;                                  // Variable to track next available ID
};

EntityType stringToEntityType(const std::string& str);