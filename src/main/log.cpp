//
// Created by Jake M O'Neill on 10/09/2022.
//

#include "log.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <vector>
#include <memory>


void logging::configure(const std::string& loggerName){
    spdlog::flush_every(std::chrono::seconds(3));
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt> (loggerName + ".log", 1024*1024*5, 5, false));
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    auto combined_logger = std::make_shared<spdlog::logger>(loggerName, begin(sinks), end(sinks));
    spdlog::register_logger(combined_logger);
}

