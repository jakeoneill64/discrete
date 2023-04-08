#ifndef DISCRETE_RENDER_H
#define DISCRETE_RENDER_H

#include <unordered_map>
#include "mesh.h"

namespace render {

    void draw(glm::mat4 view, const std::vector<discrete::Mesh> &meshes, unsigned int shader_program_id);

    //TODO we need to figure out how to get the type of these.
    unsigned int create_shader_program(const std::unordered_map<unsigned int, const char*> &shadersByType);

}

#endif //DISCRETE_RENDER_He