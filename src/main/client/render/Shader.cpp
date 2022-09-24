//
// Created by Jake M O'Neill on 09/09/2022.
//

#include "Shader.h"
#include "render.h"
#include <fstream>
#include <vector>

Shader::Shader(const char *fileName, const uint32_t& shaderType) {

    std::ifstream ifs(fileName);
    std::string code( (std::istreambuf_iterator<char>(ifs) ),
                         (std::istreambuf_iterator<char>()    ) );

    const char * codeCString = code.c_str();

    glWrap(m_shaderId = glCreateShader(shaderType));

    glWrap(glShaderSource(m_shaderId, 1, &codeCString, nullptr));
    glWrap(glCompileShader(m_shaderId));


    int success;
    glGetShaderiv(m_shaderId, GL_COMPILE_STATUS, &success);
    if(!success){
        char infoLog[1024];
        glGetShaderInfoLog(m_shaderId, 1024, nullptr, infoLog);
        spdlog::get(discrete::CLIENT_LOGGER_NAME)->
            error(std::string{"gl shader compilation error: "} +  infoLog);
    }
}

unsigned int Shader::linkShaders(const Shader& vertexShader, const Shader& fragmentShader) {

    glWrap(unsigned int programId = glCreateProgram());
    glWrap(glAttachShader(programId, vertexShader.m_shaderId));
    glWrap(glAttachShader(programId, fragmentShader.m_shaderId));
    glLinkProgram(programId);

    int success;
    glGetProgramiv(programId, GL_COMPILE_STATUS, &success);
    if(!success){
        char infoLog[1024];
        glGetProgramInfoLog(programId, 1024, nullptr, infoLog);
        spdlog::get(discrete::CLIENT_LOGGER_NAME)->
                error(std::string{"gl shader linking error: "} +  infoLog);
    }

    return programId;

}




