#include "Program.h"
#include "glad/glad.h"

#include <vector>
#include <ranges>

Program buildGLProgram(const ProgramSource& source){

    unsigned int programId{glCreateProgram()};
    std::vector<unsigned int> shaderIds{};
    const auto& compileShader =
            [&programId, &shaderIds]
            (const std::string& source, const unsigned int& type){

        auto shaderId{glCreateShader(type)};
        shaderIds.push_back(shaderId);
        const auto cStringSource{source.c_str()};
        glShaderSource(shaderId, 1, &cStringSource, nullptr);
        glCompileShader(shaderId);
        int success;
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);

        if(!success){
            char infoLog[1024];
            glGetShaderInfoLog(shaderId, 1024, nullptr, infoLog);
//            spdlog::get(discrete::CLIENT_LOGGER_NAME)->
//                    error(std::string{"gl shader compilation error: "} +  infoLog);
            return 0u;
        }
        glAttachShader(programId, shaderId);
        return shaderId;
    };

    compileShader(source.vertexSrc, GL_VERTEX_SHADER);
    compileShader(source.fragmentSrc, GL_FRAGMENT_SHADER);

    glLinkProgram(programId);

    int success;
    glGetProgramiv(programId, GL_COMPILE_STATUS, &success);
    if(!success){
        char infoLog[1024];
        glGetProgramInfoLog(programId, 1024, nullptr, infoLog);
    }

    std::ranges::for_each(shaderIds, [](unsigned int id){glDeleteShader(id);})

    return {
        source.name,
        programId
    };
}