#include "MovableEntity.h"


MovableEntity::MovableEntity(const char* texturePath, int x, int y, int width, int height, float vel, float acc): 
Entity(texturePath, x, y, width, height, vel) 
{
    acceleration = acc;
}

MovableEntity::~MovableEntity() {
    Entity::~Entity();
}
// Update the x position of the entity
void MovableEntity::updateXPosition() {
    velocity += acceleration;
    this->xPosition += (int)(velocity);
}
// Update the y position of the entity
void MovableEntity::updateYPosition() {
    velocity += acceleration;
    this->yPosition += (int)(velocity);
}

// could be refactored, enables the user to move the entity using the arrow keys or WASD
void MovableEntity::updateFromInput(SDL_Scancode scancode) {
    switch (scancode) {
        case SDL_SCANCODE_LEFT:
        case SDL_SCANCODE_A:
            xPosition -= 1;
            break;
        case SDL_SCANCODE_RIGHT:
        case SDL_SCANCODE_D:
            xPosition += 1;
            break;
        case SDL_SCANCODE_UP:
        case SDL_SCANCODE_W:
            yPosition -= 1;
            break;
        case SDL_SCANCODE_DOWN:
        case SDL_SCANCODE_S:
            yPosition += 1;
            break;
        default:
            break;
    }
}