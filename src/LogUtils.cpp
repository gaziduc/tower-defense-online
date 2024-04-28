//
// Created by Gazi on 4/20/2024.
//

#include "LogUtils.h"

#include <source_location>
#include <chrono>
#include <iostream>

std::string LogUtils::get_level_string(Level level) {
    switch (level) {
        case DEBUG:
            return "DEBUG";
        case INFO:
            return "INFO";
        case WARN:
            return "WARN";
        case SEVERE:
            return "SEVERE";
        default:
            return "DEFAULT";
    }
}

void LogUtils::log_message(Level level, const std::string &message, const std::source_location& location) {
    std::time_t timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm tm = *std::localtime(&timestamp);
    std::cout << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << ' ' << get_level_string(level) << " [" << location.file_name() << ':' << location.line() << "] " << message << std::endl;
}
