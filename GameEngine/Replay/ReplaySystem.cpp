#include "ReplaySystem.h"
#include <thread>
#include <cmath>

ReplaySystem::ReplaySystem(Timeline* timeline) : _timeline(timeline) {}

void ReplaySystem::startRecording() {
    _isRecording = true;
}

void ReplaySystem::stopRecording(EventManager* eventManager) {
    if (!_isRecording) return;

    _isReplaying = true;
    _isRecording = false;

    std::thread replayThread([this, eventManager]() {
        for (const auto& [frame, events] : _eventsByFrame) {
            // Get a frame time bucket and replay all events that occured during that frame
            for (const auto& e : events) {
                auto* event = new ReplayEvent(e);
                eventManager->raiseRawEvent(event);
            }
            // Wait for the frame delay (60 FPS hardcoded for now)
            int64_t sleepDurationNs = static_cast<int64_t>(1e9 / 60);
            std::this_thread::sleep_for(std::chrono::nanoseconds(sleepDurationNs));
        }

        _eventsByFrame.clear();
        _isReplaying = false;
        });

    replayThread.detach();
}

void ReplaySystem::handler(const EntityUpdateEvent* entityUpdateEvent) {
    if (!_isRecording) return;

    // Get the current frame bucket 
    int64_t currentFrame = _timeline->getTime() / (1e9 / 60); 
    
    ReplayEvent copy(entityUpdateEvent);
    copy.setIsReplay(true);
    _eventsByFrame[currentFrame].push_back(copy);
}

bool ReplaySystem::isReplaying() const {
    return _isReplaying;
}

bool ReplaySystem::isRecording() const {
    return _isRecording;
}
