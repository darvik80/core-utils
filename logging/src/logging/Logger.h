//
// Created by Kishchenko, Ivan on 5/6/21.
//

#ifndef CORE_UTILS_LOGGING_H
#define CORE_UTILS_LOGGING_H

#include <boost/log/trivial.hpp>
#include <fmt/core.h>

namespace logging {
    struct LoggingProperties {
        std::string level{"debug"};
        bool console{false};
        bool file{false};
    };

    void setup(LoggingProperties &properties);

#define DECLARE_LEVEL(level) \
    template <class... A> \
    static void level(const std::string& fmt, A&&... args) { \
        BOOST_LOG_TRIVIAL(level) << fmt::format(fmt, std::forward<A>(args)...); \
}

    DECLARE_LEVEL(trace)

    DECLARE_LEVEL(debug)

    DECLARE_LEVEL(info)

    DECLARE_LEVEL(warning)

    DECLARE_LEVEL(error)

    DECLARE_LEVEL(fatal)
}

#endif //CORE_UTILS_LOGGING_H
