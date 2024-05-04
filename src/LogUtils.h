//
// Created by Gazi on 4/20/2024.
//

#ifndef LOGUTILS_H
#define LOGUTILS_H
#include <source_location>
#include <string>


class LogUtils {
public:
    enum Level {
        DEBUG = 0,
        INFO,
        WARN,
        SEVERE,
        CRITICAL
    };

    static std::string get_level_string(Level level);

    // Log to stdout
    static void log_message(Level level, const std::string& message, const std::source_location& location = std::source_location::current());
};



#endif //LOGUTILS_H
