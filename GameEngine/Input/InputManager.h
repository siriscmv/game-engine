#pragma once

#ifdef __APPLE__
#include <SDL2/SDL_Scancode.h>
#else
#include <SDL/SDL_Scancode.h>
#endif
#include "unordered_map"
#include <string>
#include <functional>

// This class handles Input (key presses) and provides methods to handle them.
class InputManager {
public:
    InputManager();
    ~InputManager();

    // Process currently pressed keys and call the associated callback functions
    void process() const;
    // Bind a <name, callback> pair to a key
    void bind(SDL_Scancode scancode, const std::string& name, std::function<void()>);
    // Unbind a callback by name for a specific key
    void unbind(SDL_Scancode scancode, const std::string& name);

private:
    std::unordered_map<SDL_Scancode, std::unordered_map<std::string, std::function<void()>>> bindings;
};
