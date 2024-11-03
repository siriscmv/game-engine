//
// Created by Cyril Melvin Vincent on 11/2/24.
//

#include "EventManager.h"

// Constructor
EventManager::EventManager(Timeline* timeline)
    : _timeline(timeline) {}

// Register an event handler for a specific event type
void EventManager::registerHandler(const EventType eventType, const EventHandler& handler) {
    _handlers[eventType].push_back(handler);
}

// Raise an event by adding it to the event queue
void EventManager::raiseEvent(Event* event) {
    event->setTimestamp(_timeline->getTime());
    _eventQueue.push(event);
}

void EventManager::raiseEventWithDelay(Event* event, const int delay) {
    event->setTimestamp(_timeline->getTime() + delay);
    _eventQueue.push(event);
}

// Process and handle events that are due based on the timeline
void EventManager::process() {
    while (!_eventQueue.empty()) {
       const auto event = _eventQueue.top();

        // Process events that are due based on timestamp
        if (event->getTimestamp() <= _timeline->getTime()) {
            _eventQueue.pop();

            // Find and execute all handlers for this event type
            auto it = _handlers.find(event->getType());
            if (it != _handlers.end()) {
                for (const auto& handler : it->second) {
                    handler(event);
                }
            }
        } else {
            break;  // Future events will be handled later
        }
    }
}