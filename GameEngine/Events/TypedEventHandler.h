//
// Created by Cyril Melvin Vincent on 11/2/24.
//

#ifndef TYPED_EVENT_HANDLER_H
#define TYPED_EVENT_HANDLER_H

#include <functional>

class Event;

template<typename T>
class TypedEventHandler : public std::function<void(const Event*)> {
public:
    using HandlerFunc = std::function<void(const T*)>;

    explicit TypedEventHandler(HandlerFunc handler);
    void operator()(const Event* e) const;

private:
    HandlerFunc _handler;
};

#endif // TYPED_EVENT_HANDLER_H