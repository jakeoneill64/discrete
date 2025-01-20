#include "EntityMove.h"
#include <memory>

EntityMoveForward::EntityMoveForward(): EntityMove({0, 1, 0}){}
EntityMoveBackward::EntityMoveBackward(): EntityMove({0, -1, 0}){}
EntityMoveLeft::EntityMoveLeft(): EntityMove({-1, 0, 0}){}
EntityMoveRight::EntityMoveRight(): EntityMove({1, 0, 0}){}

void EntityMove::operator()(std::shared_ptr<World> world) const {

}
