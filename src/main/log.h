#ifndef DISCRETE_LOG_H
#define DISCRETE_LOG_H

#include <string>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <vector>
#include <memory>
#include <ranges>

class LoggingContext {
public:
    static void setLoggerName(const std::string& name);
    static const std::string& getLoggerName();

private:
    static LoggingContext& get();
    std::string loggerName;
};

constexpr auto TRACE     = spdlog::level::level_enum::trace;
constexpr auto DEBUG     = spdlog::level::level_enum::debug;
constexpr auto INFO      = spdlog::level::level_enum::info;
constexpr auto WARN      = spdlog::level::level_enum::warn;
constexpr auto ERROR     = spdlog::level::level_enum::err;
constexpr auto CRITICAL  = spdlog::level::level_enum::critical;

template <typename ...Params>
void log(const spdlog::level::level_enum level, const std::string_view &message, Params&& ...messageParams){
    const std::string& loggerName = LoggingContext::getLoggerName();
    auto logger = spdlog::get(loggerName);

    if(!logger){

        static std::vector<spdlog::sink_ptr> sinks{
            std::make_shared<spdlog::sinks::rotating_file_sink_mt> (
                loggerName + ".log",
                1024*1024*64,
                5,
                false
                ),
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
        };

        logger = std::make_shared<spdlog::logger>(loggerName, begin(sinks), end(sinks));

        // TODO add a config entry for the default log level.
        logger->set_level(spdlog::level::debug);
        std::ranges::for_each(sinks, [](const auto& sink)
        {
            sink->set_level(
                spdlog::level::debug
                );
        });

        spdlog::register_logger(logger);
    }

    logger->log(level, std::format(message, std::forward<Params>(messageParams)...));
}

#endif //DISCRETE_LOG_H