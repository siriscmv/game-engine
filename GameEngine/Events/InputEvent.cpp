//
// Created by Cyril Melvin Vincent on 11/2/24.
//

#include <SDL_scancode.h>

#include "Event.h"

class InputEvent final : public Event {
    public:
    explicit InputEvent(const SDL_Scancode scancode)
        : _scancode(scancode) {}

    EventType getType() const override { return EventType::Input; }

    SDL_Scancode getScancode() const { return _scancode; }

    private:
    SDL_Scancode _scancode;
};