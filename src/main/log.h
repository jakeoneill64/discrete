//
// Created by Jake M O'Neill on 10/09/2022.
//

#ifndef MRONEILLSCUBES_LOG_H
#define MRONEILLSCUBES_LOG_H

#include <string>
#include "spdlog/spdlog.h"

enum class Logger{
    SERVER,
    CLIENT,
    LOGGER_COUNT
};

void log(Logger logger, const std::string &message, spdlog::level::level_enum);
void log(Logger logger, const std::string &message);

#endif //MRONEILLSCUBES_LOG_H