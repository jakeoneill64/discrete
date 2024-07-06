//
// Created by Jake M O'Neill on 10/09/2022.
//

#include "log.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <vector>
#include <memory>
#include "constants.h"

void log(Logger logger, const std::string &message, spdlog::level::level_enum log_level){

    static std::array
            <
                std::string,
                static_cast<size_t>(Logger::LOGGER_COUNT)
            >
            logger_names{discrete::SERVER_LOGGER_NAME, discrete::CLIENT_LOGGER_NAME};

    if(logger == Logger::LOGGER_COUNT) return;

    std::string logger_name{logger_names[static_cast<int>(logger)]};
    auto spd_logger = spdlog::get(logger_name);

    if(!spd_logger){

        static std::vector<spdlog::sink_ptr> sinks{
#ifdef DISCRETE_DEBUG
            std::make_shared<spdlog::sinks::rotating_file_sink_mt> ("discrete.log", 1024*1024*1024, 5, false),
#endif
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
        };

        spd_logger = std::make_shared<spdlog::logger>(logger_name, begin(sinks), end(sinks));
        spdlog::register_logger(spd_logger);
    }

    spd_logger->log(log_level, message);

}

