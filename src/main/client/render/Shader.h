//
// Created by Jake M O'Neill on 09/09/2022.
//

#ifndef MRONEILLSCUBES_SHADER_H
#define MRONEILLSCUBES_SHADER_H

#include <string>
#include "glad/glad.h"
#include "render.h"

//TODO this shouldn't be a class
class Shader{
public:
    Shader(const char* fileName, const uint32_t& shaderType);

    static unsigned int linkShaders(const Shader& vertexShader, const Shader& fragmentShader);

private:
    unsigned int m_shaderId;

    static unsigned int linkShaders(unsigned int programId) {
        glWrap(glLinkProgram(programId));
        return programId;
    }

};



#endif //MRONEILLSCUBES_SHADER_H
