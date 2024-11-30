#pragma once

enum class GameState { PLAY, EXIT, PAUSED};                                     // A state enum class to manage a simple state machine
enum class Mode { SERVER, CLIENT, PEER, SINGLE_PLAYER };                        // Mode of the game engine (who does it serve)deturner

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
    FIXED, // Is unaffected by collisions, but can stop other entities from moving
    ELASTIC, // A collision causes a reversal in movement direction
    GHOST // Present in a different realm, is unaffected by and does not affect other entities
};

// An enum class to differentiate between zone types. 
enum class ZoneType {
    NONE,                          // Not a Zone
    SPAWN,                         // Spawn zones
    DEATH,                         // Death zones
    SIDESCROLL                     // Side boundaries
};

// An enum class that denotes the type of Event.
enum class EventType {
    Collision,
    Death,
    Spawn,
    Input,
    EntityUpdate,
    Replay
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

// Represents refresh rates either a server or a client update their systems
enum class RefreshRate {
    FIFTEEN_FPS = 15,
    THIRTY_FPS = 30,
    SIXTY_FPS = 60,
    NINETY_FPS = 90,
    ONE_TWENTY_FPS = 120,
    TWO_FORTY_FPS = 240
};

// Camera object to track the area that needs to be rendered
struct Camera {
    int x, y;
    Size size;
    Size originalSize;    
};