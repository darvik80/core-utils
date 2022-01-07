//
// Created by Ivan Kishchenko on 24.04.2021.
//

#pragma once

#include "Service.h"
#include "Registry.h"

LOG_COMPONENT_SETUP(service, service_logger)

class BaseService : public Service {
    logger_type& _logger;
public:
    BaseService() : _logger(service_logger::get()) {

    }

    explicit BaseService(logger_type &logger) : _logger(logger) {}

    int order() override {
        return 0;
    }

    void postConstruct(Registry &registry) override;

    void preDestroy(Registry &registry) override;

protected:
    template<class... A>
    void trace(const std::string &fmt, A &&... args) {
        BOOST_LOG_SEV(_logger, boost::log::trivial::trace) << fmt::format(fmt, std::forward<A>(args)...);
    }

    template<class... A>
    void debug(const std::string &fmt, A &&... args) {
        BOOST_LOG_SEV(_logger, boost::log::trivial::debug) << fmt::format(fmt, std::forward<A>(args)...);
    }

    template<class... A>
    void info(const std::string &fmt, A &&... args) {
        BOOST_LOG_SEV(_logger, boost::log::trivial::info) << fmt::format(fmt, std::forward<A>(args)...);
    }

    template<class... A>
    void warning(const std::string &fmt, A &&... args) {
        BOOST_LOG_SEV(_logger, boost::log::trivial::warning) << fmt::format(fmt, std::forward<A>(args)...);
    }

    template<class... A>
    void error(const std::string &fmt, A &&... args) {
        logger::error("[{}] {}", name(), fmt::format(fmt, std::forward<A>(args)...));
    }

    template<class... A>
    void fatal(const std::string &fmt, A &&... args) {
        BOOST_LOG_SEV(_logger, boost::log::trivial::fatal) << fmt::format(fmt, std::forward<A>(args)...);
    }
};

template<typename T>
class BaseServiceShared : public BaseService, public std::enable_shared_from_this<T> {
public:
    BaseServiceShared() = default;

    explicit BaseServiceShared(logger_type &logger)
            : BaseService(logger) {}
};
