#ifndef DISCRETE_ENTITYMOVEFORWARD_H
#define DISCRETE_ENTITYMOVEFORWARD_H

class EntityMoveForward: public Action<> {
public:
    void operator()(std::shared_ptr<World> world) const override {}
};

#endif // DISCRETE_ENTITYMOVEFORWARD_H