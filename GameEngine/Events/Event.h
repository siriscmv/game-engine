//
// Created by Cyril Melvin Vincent on 11/2/24.
//

#ifndef EVENT_H
#define EVENT_H
#include <Globals.h>

class Event {
public:
    Event() : _timestamp(0) {}
    explicit Event(const long long timestamp) : _timestamp(timestamp) {}

    virtual ~Event() = default;

    void setTimestamp(const long long timestamp) { _timestamp = timestamp; }
    long long getTimestamp() const { return _timestamp; }

    virtual EventType getType() const = 0;

protected:
    long long _timestamp;
};



#endif //EVENT_H
