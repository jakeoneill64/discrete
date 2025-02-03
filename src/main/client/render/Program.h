#ifndef DISCRETE_PROGRAM_H
#define DISCRETE_PROGRAM_H

#include <string>

struct Program {
    const std::string name;
    const unsigned int id;
};

struct ProgramSource {
    const std::string name;
    const std::string vertexSrc;
    const std::string fragmentSrc;
};

template <>
struct std::hash<Program> {
    std::size_t operator()(const Program& program) const noexcept {
        return std::hash<std::string>{}(program.name);
    }
};

Program buildGLProgram(const ProgramSource& source);

#endif //DISCRETE_PROGRAM_H
