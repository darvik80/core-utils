//
// Created by Kishchenko, Ivan on 5/6/21.
//

#pragma once

#include <boost/log/trivial.hpp>
#include <fmt/ranges.h>
#include <fmt/core.h>

#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/global_logger_storage.hpp>

BOOST_LOG_GLOBAL_LOGGER(g_logger, boost::log::sources::severity_logger_mt<boost::log::trivial::severity_level>);

#define DECLARE_LEVEL(LOGGER, LEVEL) \
    template <class... A> \
    static void LEVEL(const std::string& fmt, A&&... args) { \
        BOOST_LOG_SEV(LOGGER::get(), boost::log::trivial::LEVEL) << fmt::format(fmt, std::forward<A>(args)...); \
}

#define LOG_COMPONENT_SETUP(COMPONENT, LOGGER)                                                  \
    BOOST_LOG_INLINE_GLOBAL_LOGGER_CTOR_ARGS(                                                   \
        LOGGER,                                                                                 \
        boost::log::sources::severity_channel_logger_mt<boost::log::trivial::severity_level>,   \
        (boost::log::keywords::channel = #COMPONENT)                                            \
    );                                                                                          \
namespace COMPONENT::log {                                                                      \
                                                                                                \
    DECLARE_LEVEL(LOGGER, trace)                                                                \
                                                                                                \
    DECLARE_LEVEL(LOGGER, debug)                                                                \
                                                                                                \
    DECLARE_LEVEL(LOGGER, info)                                                                 \
                                                                                                \
    DECLARE_LEVEL(LOGGER, warning)                                                              \
                                                                                                \
    DECLARE_LEVEL(LOGGER, error)                                                                \
}

namespace logger {
    struct LoggingProperties {
        std::string level{"debug"};
        bool console{true};
        bool file{false};
    };

    void setup(LoggingProperties &properties);

    DECLARE_LEVEL(g_logger, trace)

    DECLARE_LEVEL(g_logger, debug)

    DECLARE_LEVEL(g_logger, info)

    DECLARE_LEVEL(g_logger, warning)

    DECLARE_LEVEL(g_logger, error)

    DECLARE_LEVEL(g_logger, fatal)
}

