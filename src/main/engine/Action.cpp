#include "Action.h"

template<typename T>
ActionHelper<T>::Type::Type(T t) {
    this->data = t;
}