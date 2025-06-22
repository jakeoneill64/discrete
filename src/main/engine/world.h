#ifndef DISCRETE_WORLD_H
#define DISCRETE_WORLD_H

#include <memory>
#include <filesystem>
#include <expected>
#include <unordered_map>
#include <cinttypes>

#include "Entity.h"

class World {
public:
    std::byte voxels;

private:
    std::unordered_map<uint32_t, Entity> m_entitiesById;
};

struct WorldView
{
    //immutable view of a world. memory efficient.
    std::unordered_map<uint32_t, Entity> m_entitiesById;
};


std::expected<std::shared_ptr<World>, std::runtime_error> load_world(const std::filesystem::path& path);


#endif //DISCRETE_WORLD_H