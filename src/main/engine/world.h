#ifndef DISCRETE_WORLD_H
#define DISCRETE_WORLD_H

#include "voxel.h"
#include <vector>
#include <memory>
#include <filesystem>
#include <expected>


class World {
public:
    std::byte voxels;

private:
    //KDTree<Voxel> for efficient spacial lookup
};

struct WorldView
{
    //immutable view of a world. memory efficient.
};


std::expected<std::shared_ptr<World>, std::runtime_error> load_world(const std::filesystem::path& path);


#endif //DISCRETE_WORLD_H