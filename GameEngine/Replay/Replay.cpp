//
// Created by Cyril Melvin Vincent on 11/28/24.
//

#include "Replay.h"

#include <thread>

#include "EventManager.h"

void Replay::startRecording() {
  _isRecording = true;
}

void Replay::stopRecording(EventManager* eventManager) {
  if (!_isRecording) return;

  _isReplaying = true;
  _isRecording = false;

  std::thread replayThread([this, eventManager]() {
    for (const auto& e : _events) {
      auto* event = new ReplayEvent(e);
      eventManager->raiseRawEvent(event);

      int sleepDurationMs = 1000 / static_cast<int>(RefreshRate::SIXTY_FPS);
      int64_t sleepDurationNs = sleepDurationMs * 1e6;
      std::this_thread::sleep_for(std::chrono::nanoseconds(sleepDurationNs));
    }

    _events.clear();
    _isReplaying = false;
  });

  replayThread.detach();
}

void Replay::handler(const EntityUpdateEvent* entityUpdateEvent) {
  ReplayEvent copy(entityUpdateEvent);
  copy.setIsReplay(true);

  _events.push_back(copy);
}

bool Replay::isReplaying() const {
  return _isReplaying;
}

bool Replay::isRecording() const {
  return _isRecording;
}