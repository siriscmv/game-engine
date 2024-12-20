#include "Entity.h"
#include <string>
#include <algorithm>
#include "TextureCache.h"

#include "Renderer.h"
#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL/SDL.h>
#endif

int Entity::_nextID = 0;

// Constructor for Rectangles
Entity::Entity(Position position, Size size, SDL_Color color) {
    generateEntityID();
    setPosition(position);
    setOriginalPosition(position);
    setSize(size);
    setOriginalSize(size);
    setShapeType(ShapeType::RECTANGLE);
    setColor(color);
}

// Contructor for Circles
Entity::Entity(Position position, float radius, SDL_Color color ) {
    generateEntityID();
    setPosition(position);
    setOriginalPosition(position);
    setCircleRadius(radius);
    setOriginalCircleRadius(radius);
    setShapeType(ShapeType::CIRCLE);
    setColor(color);
}

// Constructor for triangles
Entity::Entity(Position position, float baseLength, float height, SDL_Color color ) {
    generateEntityID();
    setPosition(position);
    setOriginalPosition(position);
    setTriangleBaseLength(baseLength);
    setOriginalTriangleBaseLength(baseLength);
    setTriangleHeight(height);
    setOriginalTriangleHeight(height);
    setShapeType(ShapeType::TRIANGLE);
    setColor(color);
}

// Constructor for textured entities
Entity::Entity(const char *texturePath, Position position, Size size) {
    generateEntityID();
    _texturePath = texturePath;
    setPosition(position);
    setOriginalPosition(position);
    setSize(size);
    setShapeType(ShapeType::TEXTURE);
}

//void resetToOriginalPosition(Entity* entity) {
//	entity->setPosition(entity->getOriginalPosition());
//}

Entity::~Entity() {
    shutdown();
}

// Setters
void Entity::generateEntityID() { _entityID = _nextID++; }
void Entity::setEntityID(int id) { _entityID = id; }
void Entity::setPosition(Position position) { _position = position; }
void Entity::setOriginalPosition(Position position) { _originalPosition = position; }
void Entity::setSize(Size size) { _size = size; }
void Entity::setOriginalSize(Size size) { _originalSize = size; }
void Entity::setEntityType(EntityType entityType) { _entityType = entityType; }
void Entity::setZoneType(ZoneType zoneType) { _zoneType = zoneType; }
void Entity::setShapeType(ShapeType shape) { _shape = shape; }
void Entity::setVelocityX(float velocityX) { _velocity.x = velocityX; }
void Entity::setVelocityY(float velocityY) { _velocity.y = velocityY; }
void Entity::setAccelerationX(float accelerationX) { _acceleration.x = accelerationX; }
void Entity::setAccelerationY(float accelerationY) { _acceleration.y = accelerationY; }
void Entity::setCircleRadius(float radius) { _circleRadius = radius; }
void Entity::setOriginalCircleRadius(float radius) { _originalCircleRadius = radius; }
void Entity::setTriangleBaseLength(float baseLength) { _triangleBaseLength = baseLength; }
void Entity::setOriginalTriangleBaseLength(float baseLength) { _originalTriangleBaseLength = baseLength; }
void Entity::setTriangleHeight(float height) { _triangleHeight = height; }
void Entity::setOriginalTriangleHeight(float height) { _originalTriangleHeight = height; }
void Entity::setColor(SDL_Color color) { _color = color; }
void Entity::setEventDelay(int delay) { _eventDelay = delay; }

// Getters
int Entity::getEntityID() const { return _entityID; }
Position Entity::getPosition() const { return _position; }
Position Entity::getOriginalPosition() const { return _originalPosition; }
Size Entity::getOriginalSize() const { return _originalSize; }
Size Entity::getSize() const { return _size; }
EntityType Entity::getEntityType() const { return _entityType; }
ZoneType Entity::getZoneType() const { return _zoneType; }
ShapeType Entity::getShapeType() const { return _shape; }
float Entity::getVelocityX() const { return _velocity.x; }
float Entity::getVelocityY() const { return _velocity.y; }
float Entity::getAccelerationX() const { return _acceleration.x; }
float Entity::getAccelerationY() const { return _acceleration.y; }
float Entity::getCircleRadius() const { return _circleRadius; }
float Entity::getTriangleBaseLength() const { return _triangleBaseLength; }
float Entity::getTriangleHeight() const { return _triangleHeight; }
SDL_Color Entity::getColor() const { return _color; }
int Entity::getEventDelay() const { return _eventDelay; }

// Draws a rectangle
void Entity::drawRectangle(SDL_Renderer* renderer, Position position) {
    if (_texture == nullptr) {
        if (!_texturePath.empty()) {
            // Try to load the texture from the path
            loadTexture(renderer);
        }
        else {
            // Generate a solid texture if no path is set
            _texture = generateSolidTexture(renderer);
        }
    }

    SDL_Rect dstRect = { position.x, position.y, _size.width, _size.height };
    SDL_Point center = { _size.width / 2, _size.height / 2 };
    SDL_RenderCopyEx(renderer, _texture, nullptr, &dstRect, _rotationAngle, &center, SDL_FLIP_NONE);
}

// Draws a circle
void Entity::drawCircle(SDL_Renderer *renderer, Position position) {

    // using the SDL_SetRenderDrawColor function to set the color of the circle
    SDL_SetRenderDrawColor(renderer, _color.r, _color.g, _color.b, _color.a);
    // looping through the circle and drawing the points
    for (int w = 0; w < _circleRadius * 2; w++)     {
        for (int h = 0; h < _circleRadius * 2; h++)         {
            int dx = _circleRadius - w; // horizontal offset
            int dy = _circleRadius - h; // vertical offset
            if ((dx * dx + dy * dy) <= (_circleRadius * _circleRadius))             {
                SDL_RenderDrawPoint(renderer, position.x + dx, position.y + dy);
            }
        }
    }
}

// Draws a triangle
void Entity::drawTriangle(SDL_Renderer *renderer, Position position) {
    // using the SDL_SetRenderDrawColor function to set the color of the triangle
    SDL_SetRenderDrawColor(renderer, _color.r, _color.g, _color.b, _color.a);
    // creating an array of points to draw the triangle
    SDL_Point points[3] = {
        {position.x, position.y},
        {position.x + _triangleBaseLength, position.y},
        {position.x + _triangleBaseLength / 2, position.y - _triangleHeight}};
    // using the SDL_RenderDrawLines function to draw the triangle
    SDL_RenderDrawLines(renderer, points, 3);
}

// Load the texture using the _texturePath variable
bool Entity::loadTexture(SDL_Renderer* renderer) {
    if (_texturePath.empty()) {
        throw std::runtime_error("Texture path is empty. Cannot load texture.");
    }

    _texture = TextureCache::getTexture(renderer, _texturePath);
    return true;
}

// Generates a texture
SDL_Texture* Entity::generateSolidTexture(SDL_Renderer* renderer) {
    // Create an empty texture
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, _size.width, _size.height);
    if (!texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create texture: %s", SDL_GetError());
        return nullptr;
    }

    // Set the texture as the rendering target
    SDL_SetRenderTarget(renderer, texture);

    // Set the texture color and fill it
    SDL_SetRenderDrawColor(renderer, _color.r, _color.g, _color.b, _color.a);
    SDL_RenderClear(renderer);

    // Reset rendering target to default
    SDL_SetRenderTarget(renderer, nullptr);

    return texture;
}

// Scales the entity based on the scale factors passed into the function
void Entity::applyScaling(float scaleX, float scaleY) {
    _position.x = _originalPosition.x * scaleX;
    _position.y = _originalPosition.y * scaleY;

    switch (_shape) {
    case ShapeType::RECTANGLE:
    case ShapeType::TEXTURE:        
        _size.width = _originalSize.width * scaleX;
        _size.height = _originalSize.height * scaleY;
        break;
    case ShapeType::CIRCLE:        
        _circleRadius = _originalCircleRadius * std::min(scaleX, scaleY);  // Use the smaller scale factor to maintain shape        
        break;
    case ShapeType::TRIANGLE:        
        _triangleBaseLength = _originalTriangleBaseLength * scaleX;
        _triangleHeight = _originalTriangleHeight * scaleY;        
        break;
    default:
        break;
    }
}

// Teleports the entity to the position passed
void Entity::teleportTo(const Position& position) {
    setPosition(position);
}


// Renders the entity onto the screen
void Entity::render(SDL_Renderer* renderer, const Camera& camera) {
    // Adjusted position based on camera's position (to render relative to the camera)
    Position adjustedPosition = {
        _position.x - camera.x,  
        _position.y - camera.y   
    };

    switch (_shape) {
    case ShapeType::TEXTURE:
        if (_texture != nullptr) {
            SDL_Rect dstRect = { adjustedPosition.x, adjustedPosition.y, _size.width, _size.height };
            SDL_RenderCopy(renderer, _texture, nullptr, &dstRect);
        }
        else {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to render entity: Texture is null at position (%f, %f)\n", _position.x, _position.y);
        }
        break;
    case ShapeType::RECTANGLE:
        drawRectangle(renderer, adjustedPosition);
        break;

    case ShapeType::CIRCLE:
        drawCircle(renderer, adjustedPosition);
        break;

    case ShapeType::TRIANGLE:
        drawTriangle(renderer, adjustedPosition);
        break;
    default:
        break;
    }
}


// Checks whether the entity is within the camera's boundaries (viewport).
bool Entity::isWithinViewPort(const Camera& camera) const {   
    return !(
        _position.x + _size.width < camera.x ||             // Entity is completely to the left of the camera
        _position.x > camera.x + camera.size.width ||       // Entity is completely to the right of the camera
        _position.y + _size.height < camera.y ||            // Entity is completely above the camera
        _position.y > camera.y + camera.size.height         // Entity is completely below the camera
        );
}


// Cleans up class variables
void Entity::shutdown() {
    if (_texture) {
        SDL_DestroyTexture(_texture);
        _texture = nullptr;
    }
}

// Converts entity type string into an enum variable
EntityType stringToEntityType(const std::string& str) {
    if (str == "DEFAULT") return EntityType::DEFAULT;
    else if (str == "GHOST") return EntityType::GHOST;
    else if (str == "ELASTIC") return EntityType::ELASTIC;
    else if (str == "FIXED") return EntityType::FIXED;
    else return EntityType::DEFAULT; // Default case
}

// Converts zone type string into an enum variable
ZoneType stringToZoneType(const std::string& str) {
    if (str == "SPAWN") return ZoneType::SPAWN;
    else if (str == "DEATH") return ZoneType::DEATH;
    else if (str == "SIDESCROLL") return ZoneType::SIDESCROLL;
    else return ZoneType::NONE;
}

void Entity::setRotationAngle(float angle) { _rotationAngle = angle; }
float Entity::getRotationAngle() const { return _rotationAngle; }
void Entity::setTexturePath(const std::string& texturePath) { _texturePath = texturePath; }
const std::string& Entity::getTexturePath() const { return _texturePath; }
