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

void log(const std::string &message, spdlog::level::level_enum log_level){

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

        // TODO add a config entry for the log level.
        logger->set_level(spdlog::level::debug);
        std::ranges::for_each(sinks, [](const auto& sink)
        {
            sink->set_level(
                spdlog::level::debug
                );
        });

        spdlog::register_logger(logger);
    }

    logger->log(log_level, message);

}

