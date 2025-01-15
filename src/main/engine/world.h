#ifndef DISCRETE_WORLD_H
#define DISCRETE_WORLD_H

#include "voxel.h"
#include <vector>
#include <memory>
#include <filesystem>
#include <expected>

struct World{
    std::vector<Voxel> voxels;
//    std::vector<Entity> m_entities;
};

std::expected<std::shared_ptr<World>, std::runtime_error> load_world(const std::filesystem::path& path);


#endif //DISCRETE_WORLD_H
