//
// Created by Cyril Melvin Vincent on 11/2/24.
//

#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL/SDL.h>
#include <utility>
#endif

#include <set>
#include "Event.h"

class InputEvent final : public Event {
    public:
    explicit InputEvent(const std::set<SDL_Scancode>& binding)
        : _binding(binding) {}

    EventType getType() const override { return EventType::Input; }

    std::set<SDL_Scancode> getBinding() const { return _binding; }

    private:
    std::set<SDL_Scancode> _binding;
};