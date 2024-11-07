#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL/SDL.h>
#endif
#include "InputManager.h"

#include <EventManager.h>

#include "InputEvent.cpp"
#include <stdexcept>

InputManager::InputManager(const bool considerPrevKeys) : _considerPrevKeys(considerPrevKeys) {}

InputManager::~InputManager() = default;

void InputManager::bind(const keyBinding& keyBinding) {
    if (bindings.find(keyBinding) != bindings.end()) {
        throw std::runtime_error("keyBinding exists already");
    }

    bindings.insert(keyBinding);
}

void InputManager::unbind(const keyBinding& keyBinding) {
    if (bindings.find(keyBinding) == bindings.end()) {
        throw std::runtime_error("keyBinding does not exist");
    }

    bindings.erase(keyBinding);
}

Uint8 *prevKeys = nullptr;

void InputManager::process(EventManager* eventManager) const {
    int size = 0;
    const Uint8 *keys = SDL_GetKeyboardState(&size);

    if (keys == nullptr) {
        throw std::runtime_error("SDL_GetKeyboardState failed");
    }

    std::set<SDL_Scancode> pressed;

    for (int i = 0; i < size; i++) {
        if (keys[i] == 1) {
            pressed.insert(static_cast<SDL_Scancode>(i));
        }
    };

    std::vector<keyBinding> sortedBindings(bindings.begin(), bindings.end());

    // Sort by the size of the keyBinding (in descending order)
    std::sort(sortedBindings.begin(), sortedBindings.end(),
              [](const keyBinding& b, const keyBinding& a) {
                  return a.size() < b.size();
              });


    // Iterate through the bindings
    for (auto & binding : sortedBindings) {
        bool isPressed = true;

        for (auto & key : binding) {
            const bool isCurrentlyPressed = pressed.find(key) != pressed.end();
            const bool isNewlyPressed = prevKeys == nullptr || prevKeys[key] == 0;
            if (!isCurrentlyPressed || !isNewlyPressed) {
                isPressed = false;
                break;
            }
        }

        if (!isPressed) continue;

        // Remove the keys that were pressed from the set of pressed keys
        for (auto & key: binding) {
            pressed.erase(key);
        }

        Event* event = new InputEvent(binding);
        eventManager->raiseEvent(event);
    }

    // Store the state of keys for comparison in the next render cycle
    if (_considerPrevKeys) {
        prevKeys = new Uint8[size];
        std::memcpy(prevKeys, keys, size * sizeof(Uint8));
    }
}
