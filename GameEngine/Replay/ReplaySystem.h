#pragma once

#include <EventManager.h>
#include <vector>
#include <map>
#include "EntityUpdateEvent.cpp"
#include "ReplayEvent.cpp"
#include "Timeline.h"

class ReplaySystem {
public:
    explicit ReplaySystem(Timeline* timeline); 

    void startRecording();
    void stopRecording(EventManager* eventManager);
    void handler(const EntityUpdateEvent* entityUpdateEvent);

    bool isReplaying() const;
    bool isRecording() const;

private:
    bool _isRecording = false;
    bool _isReplaying = false;
    Timeline* _timeline; 
    std::map<int64_t, std::vector<ReplayEvent>> _eventsByFrame;                           // Events grouped by frame time
};
