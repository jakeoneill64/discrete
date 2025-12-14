#include "log.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <vector>
#include <memory>
#include <ranges>

void LoggingContext::setLoggerName(const std::string& name) {
    if (!get().loggerName.empty())
    {
        throw std::runtime_error("Logging context already set");
    }
    get().loggerName = name;
}

const std::string& LoggingContext::getLoggerName() {
    return get().loggerName;
}

LoggingContext& LoggingContext::get() {
    thread_local LoggingContext instance;
    return instance;
}

