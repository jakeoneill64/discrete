//
// Created by Jake M O'Neill on 09/09/2022.
//

#include "Shader.h"
#include <fstream>
#include "GLFW/glfw3.h"

Shader::Shader(const char *fileName, const uint32_t& shaderType) {

    std::ifstream ifs(fileName);
    std::string code( (std::istreambuf_iterator<char>(ifs) ),
                         (std::istreambuf_iterator<char>()    ) );

    const char * codeCString = code.c_str();

    m_shaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_shaderId, 1, &codeCString, nullptr);
    glCompileShader(m_shaderId);

}

Shader::Shader(std::string &fileName, const uint32_t &shaderType) : Shader(fileName.c_str(), shaderType){}

void Shader::enable() const {
    glUseProgram(m_shaderId);
}


