//
// Created by Cyril Melvin Vincent on 11/28/24.
//

#include "vector"
#include "Event.h"

#ifndef REPLAY_H
#define REPLAY_H

class Replay {
public:
  void startRecording();
  void stopRecording();

private:
  bool _isRecording;
  std::vector<Event*> _events;
};



#endif //REPLAY_H
