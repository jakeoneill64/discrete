#ifndef DISCRETE_LOG_H
#define DISCRETE_LOG_H

#include <string>
#include "spdlog/spdlog.h"

class LoggingContext {
public:
    static void setLoggerName(const std::string& name);
    static const std::string& getLoggerName();

private:
    static LoggingContext& get();
    std::string loggerName;
};
void log(const std::string &message, spdlog::level::level_enum);

#endif //DISCRETE_LOG_H