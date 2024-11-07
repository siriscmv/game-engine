//
// Created by Cyril Melvin Vincent on 11/2/24.
//

#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <functional>
#include <unordered_map>
#include <vector>
#include <queue>
#include "Event.h"
#include "Timeline.h"

// Alias for the event handler function
using EventHandler = std::function<void(const Event*)>;

// Comparator for the priority queue
struct EventComparator {
    bool operator()(const Event* lhs, const Event* rhs) const {
        return lhs->getTimestamp() > rhs->getTimestamp();
    }
};

class EventManager {
public:
    // Constructor
    explicit EventManager(Timeline* timeline);

    // Register an event handler for a specific event type
    void registerHandler(EventType eventType, const EventHandler& handler);

    // Raise an event by adding it to the event queue
    void raiseEvent(Event* event);

    // Raise an event with added delay
    void raiseEventWithDelay(Event* event, int delay);

    // Process and handle events that are due based on the timeline
    void process();

private:
    Timeline* _timeline;
    std::priority_queue<Event*,
                        std::vector<Event*>,
                        EventComparator> _eventQueue;
    std::unordered_map<EventType, std::vector<EventHandler>> _handlers;
    std::mutex _eventQueueMutex;
};

#endif // EVENT_MANAGER_H
