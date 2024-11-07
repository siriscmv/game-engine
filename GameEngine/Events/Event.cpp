//
// Created by Cyril Melvin Vincent on 11/2/24.
//

#include "Event.h"

#include <stdexcept>

void Event::setTimestamp(const long long timestamp) {_timestamp = timestamp;}
long long Event::getTimestamp() const {return _timestamp;}
