//
// Created by Cyril Melvin Vincent on 11/2/24.//


#include <SDL/SDL.h>
#include <set>

#include "Event.h"

class InputEvent final : public Event {
    public:
    explicit InputEvent(const std::set<SDL_Scancode>& binding, int clientID)
        : _binding(binding), _clientID(clientID) {}

    explicit InputEvent(const std::set<SDL_Scancode>& binding)
        : _binding(binding), _clientID(-1) {}

    EventType getType() const override { return EventType::Input; }

    std::set<SDL_Scancode> getBinding() const { return _binding; }
    int getClientID() const { return _clientID; }

    private:
    std::set<SDL_Scancode> _binding;
    int _clientID;                                                  // To determine which client this event belongs to
};