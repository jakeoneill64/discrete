//
// Created by Jake M O'Neill on 09/09/2022.
//

#ifndef MRONEILLSCUBES_SHADER_H
#define MRONEILLSCUBES_SHADER_H

#include <string>

class Shader{
public:
    Shader(const char* fileName, const uint32_t& shaderType);
    Shader(std::string& fileName, const uint32_t& shaderType);
    void enable() const;
private:
    unsigned int m_shaderId;
};


#endif //MRONEILLSCUBES_SHADER_H
