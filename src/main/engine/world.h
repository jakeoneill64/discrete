#ifndef DISCRETE_WORLD_H
#define DISCRETE_WORLD_H

#include "voxel.h"
#include <vector>
#include <memory>
#include <filesystem>
#include <expected>

std::expected<std::shared_ptr<World>, std::runtime_error> load_world(const std::filesystem::path& path);

class World {
public:
    //Lookup within a sphere, or specified bounds

private:
    //KDTree<Voxel> for efficient spacial lookup
};


#endif //DISCRETE_WORLD_H
