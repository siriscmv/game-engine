//
// Created by Cyril Melvin Vincent on 11/28/24.
//
#pragma once

#include <EventManager.h>

#include "vector"
#include "EntityUpdateEvent.cpp"

class Event;

class Replay {
public:
  void startRecording();
  void stopRecording(EventManager *eventManager);
  void handler(const EntityUpdateEvent *entityUpdateEvent);

  bool isReplaying() const;

private:
  bool _isRecording = false;
  bool _isReplaying = false;
  std::vector<const EntityUpdateEvent> _events = {};
};

