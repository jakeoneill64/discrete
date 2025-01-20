#ifndef DISCRETE_LOG_H
#define DISCRETE_LOG_H

#include <string>
#include "spdlog/spdlog.h"

enum class Logger{
    SERVER,
    CLIENT,
    LOGGER_COUNT
};

void log(Logger logger, const std::string &message, spdlog::level::level_enum);
void log(Logger logger, const std::string &message);

#endif //DISCRETE_LOG_H