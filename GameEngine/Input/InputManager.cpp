#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL/SDL.h>
#endif
#include "InputManager.h"

InputManager::InputManager() = default;

InputManager::~InputManager() = default;

void InputManager::bind(const SDL_Scancode scancode, const std::string& name, void (*callback)()) {
    if (bindings[scancode][name] != nullptr) {
        throw std::runtime_error("keybinding names must be unique");
    }

    bindings[scancode][name] = callback;
}

void InputManager::unbind(const SDL_Scancode scancode, const std::string& name) {
    if (bindings[scancode][name] == nullptr) {
        throw std::runtime_error("This keybinding does not exist");
    }

    bindings[scancode].erase(name);
}

void InputManager::run() const {
    int* size = nullptr;
    const Uint8 *keys = SDL_GetKeyboardState(size);

    if (keys == nullptr) {
        throw std::runtime_error("SDL_GetKeyboardState failed");
    }

    // Iterate through the bindings
    for (auto & binding : bindings) {
        // If the corresponding key is pressed
        if (binding.first <= *size && keys[binding.first] == 1) {
            // Iterate through all callbacks associated with that key
            for (auto & action: binding.second) {
                // Execute them
                action.second();
            }
        }
    }
}
