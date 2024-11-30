//
// Created by Cyril Melvin Vincent on 11/28/24.
//

#include "Replay.h"
#include "EventManager.h"

void Replay::startRecording() {
  _isRecording = true;
}

void Replay::stopRecording(EventManager* eventManager) {
  if (!_isRecording) return;

  _isReplaying = true;
  _isRecording = false;

  for (const auto& e : _events) {
    auto* event = new ReplayEvent(e);
    event->setIsReplay(true);
    event->setTimestamp(e.getTimestamp());
    event->setEntity(e.getEntity());
    eventManager->raiseRawEvent(event);
  }

  _events.clear();
  _isReplaying = false;
}

void Replay::handler(const EntityUpdateEvent* entityUpdateEvent) {
  //FIXME: @cyril: Ugly hack
  Entity entity = Entity(Position(0, 0), Size(0, 0), SDL_Color{0, 0, 0, 0});
  entity.setPosition(Position(entityUpdateEvent->getEntity()->getPosition().x, entityUpdateEvent->getEntity()->getPosition().y));
  entity.setSize(Size(entityUpdateEvent->getEntity()->getSize().width, entityUpdateEvent->getEntity()->getSize().height));
  entity.setEntityType(entityUpdateEvent->getEntity()->getEntityType());
  entity.setZoneType(entityUpdateEvent->getEntity()->getZoneType());
  entity.setVelocityX(entityUpdateEvent->getEntity()->getVelocityX());
  entity.setVelocityY(entityUpdateEvent->getEntity()->getVelocityY());
  entity.setAccelerationX(entityUpdateEvent->getEntity()->getAccelerationX());
  entity.setAccelerationY(entityUpdateEvent->getEntity()->getAccelerationY());
  entity.setColor(entityUpdateEvent->getEntity()->getColor());
  entity.setEventDelay(entityUpdateEvent->getEntity()->getEventDelay());

  ReplayEvent copy(&entity);
  copy.setEntity(&entity);
  copy.setIsReplay(true);
  copy.setTimestamp(entityUpdateEvent->getTimestamp());
  _events.push_back(copy);
}

bool Replay::isReplaying() const {
  return _isReplaying;
}

bool Replay::isRecording() const {
  return _isRecording;
}