//
// Created by Cyril Melvin Vincent on 11/2/24.
//

#include "TypedEventHandler.h"
#include "InputEvent.cpp"

template<typename T>
TypedEventHandler<T>::TypedEventHandler(HandlerFunc handler)
    : std::function<void(const Event*)>([handler](const Event* e) {
        if (auto specific = dynamic_cast<const T*>(e)) {
            handler(specific);
        }
    }), _handler(handler) {}

template<typename T>
void TypedEventHandler<T>::operator()(const Event* e) const {
    if (auto specific = dynamic_cast<const T*>(e)) {
        _handler(specific);
    }
}

template class TypedEventHandler<InputEvent>;