#pragma once

enum class GameState { PLAY, EXIT };                                     // A state enum class to manage a simple state machine
enum class Mode { SERVER, CLIENT, PEER, PEER_SERVER, SINGLE_PLAYER };                 // Mode of the game engine (who does it serve)

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