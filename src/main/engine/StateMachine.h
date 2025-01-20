#ifndef DISCRETE_STATEMACHINE_H
#define DISCRETE_STATEMACHINE_H

#include "Action.h"
#include <memory>
#include <string>

// Seems a bit weird to be using Action as the state here.. maybe something to consider.
// debatable whether this pattern is even a good fit here.
class StateMachine {

private:

    struct StateWrapper {
        std::shared_ptr<Action<>> state;
        std::shared_ptr<Action<>> onComplete;
        std::shared_ptr<Action<>> onErrored;
    };

public:

    StateMachine(const std::string& templateName, std::shared_ptr<Action<>> initialState);
    std::shared_ptr<StateWrapper> currentState;

};


#endif //DISCRETE_STATEMACHINE_H
