//
// Created by Cyril Melvin Vincent on 11/2/24.
//

#ifndef EVENT_H
#define EVENT_H
#include <Globals.h>

class Event {
public:
    explicit Event(): _timestamp(0) {}

    virtual ~Event() = default;

    void setTimestamp(long long timestamp);
    long long getTimestamp() const;

    // Pure virtual function to get event type
    virtual EventType getType() const = 0;

protected:
    long long _timestamp;
};



#endif //EVENT_H
