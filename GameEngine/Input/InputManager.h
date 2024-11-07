#pragma once

#ifdef __APPLE__
#include <SDL2/SDL_scancode.h>
#else
#include <SDL/SDL_scancode.h>
#endif
#include <set>

class EventManager;
using keyBinding = std::set<SDL_Scancode>;

// This class handles Input (key presses) and provides methods to handle them.
class InputManager {
public:
    explicit InputManager(bool considerPrevKeys = false);
    ~InputManager();

    // Process currently pressed keys and call the associated callback functions
    void process(EventManager* eventManager) const;
    // Register a keyBinding
    void bind(const keyBinding& keyBinding);
    // Unregister a keyBinding
    void unbind(const keyBinding& keyBinding);

private:
    std::set<keyBinding> bindings;
    bool _considerPrevKeys; // Whether to consider the previous key state to determine if a key is pressed or not
};
