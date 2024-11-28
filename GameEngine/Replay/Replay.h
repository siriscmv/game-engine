//
// Created by Cyril Melvin Vincent on 11/28/24.
//
#pragma once

#include "vector"

class Event;

class Replay {
public:
  void startRecording();
  void stopRecording();

private:
  bool _isRecording = false;
  std::vector<Event*> _events;
};


