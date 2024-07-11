#include <memory>
#include "world.h"
#include <fstream>
#include <expected>
#include <vector>
#include <ranges>
#include "voxel.h"
#include <stdexcept>
#include <filesystem>
#include <cstring>

std::expected<std::shared_ptr<World>, std::runtime_error> load_world(const std::filesystem::path& path){


    // TODO let's extract this out the next time we need something similar.
    std::ifstream inFile{path, std::ios::binary};
    if (!inFile.is_open())
        return std::unexpected(std::runtime_error("could not read world file data" + path.string()));
    std::uintmax_t fileSize;
    try{
        fileSize = std::filesystem::file_size(path);
    }catch(std::filesystem::filesystem_error& e){
        return std::unexpected(e);
    }

    char buffer[fileSize];
    inFile.read(buffer, static_cast<std::streamsize>(fileSize));

    //NB we make the assumption that float is a 32 bit here.
    using PortableVoxel = struct {
        struct vector{
            float i, j, k;
        };
        vector position;
        vector velocity;
        float diameter;
        float frictionCoefficient;
        float density;
    };

    std::vector<PortableVoxel> voxels{fileSize / sizeof(PortableVoxel)};
    std::memcpy(voxels.data(), &buffer, fileSize);

    auto world {std::make_shared<World>()};
    world->voxels = std::move(
        std::ranges::to<std::vector<Voxel>>(

            voxels | std::views::transform([](const PortableVoxel& portableVoxel) {
                return Voxel{
                {
                    portableVoxel.position.i,
                    portableVoxel.position.j,
                    portableVoxel.position.k
                    },
                    {
                    portableVoxel.velocity.i,
                    portableVoxel.velocity.j,
                    portableVoxel.velocity.k
                    },
                    portableVoxel.diameter,
                    portableVoxel.frictionCoefficient,
                    portableVoxel.density
                };
            })
        )
    );

    return world;

}
