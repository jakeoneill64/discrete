//
// Created by Jake M O'Neill on 09/09/2022.
//

#ifndef MRONEILLSCUBES_SHADER_H
#define MRONEILLSCUBES_SHADER_H

#include <string>
#include "glad/glad.h"
#include "render.h"

class Shader{
public:
    Shader(const char* fileName, const uint32_t& shaderType);

    template<typename ... Param>
    static unsigned int linkShaders(const Shader& shader, const Param& ... shaders) {

        glWrap(unsigned int programId = glCreateProgram());

        return linkShaders(programId, shaders...);
    }

private:
    unsigned int m_shaderId;

    template<typename ... Param>
    static unsigned int linkShaders(unsigned int programId, const Shader& shader, const Param& ... shaders) {

        glWrap(glAttachShader(programId, shader.m_shaderId));

        return linkShaders(programId, shaders...);
    }

    static unsigned int linkShaders(unsigned int programId) {
        glWrap(glLinkProgram(programId));
        return programId;
    }

};



#endif //MRONEILLSCUBES_SHADER_H
