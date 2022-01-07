//
// Created by Ivan Kishchenko on 24.04.2021.
//

#pragma once

#include "Service.h"
#include "Registry.h"

class BaseService : public Service {
public:
    int order() override {
        return 0;
    }

    void postConstruct(Registry &registry) override;

    void preDestroy(Registry& registry) override;

protected:
    template <class... A>
    void trace(const std::string& fmt, A&&... args) {
        logger::trace("[{}] {}", name(), fmt::format(fmt, std::forward<A>(args)...));
    }

    template <class... A>
    void debug(const std::string& fmt, A&&... args) {
        logger::debug("[{}] {}", name(), fmt::format(fmt, std::forward<A>(args)...));
    }

    template <class... A>
    void info(const std::string& fmt, A&&... args) {
        logger::info("[{}] {}", name(), fmt::format(fmt, std::forward<A>(args)...));
    }

    template <class... A>
    void warning(const std::string& fmt, A&&... args) {
        logger::warning("[{}] {}", name(), fmt::format(fmt, std::forward<A>(args)...));
    }

    template <class... A>
    void error(const std::string& fmt, A&&... args) {
        logger::error("[{}] {}", name(), fmt::format(fmt, std::forward<A>(args)...));
    }

    template <class... A>
    void fatal(const std::string& fmt, A&&... args) {
        logger::fatal("[{}] {}", name(), fmt::format(fmt, std::forward<A>(args)...));
    }
};

template<typename T>
class BaseServiceShared : public BaseService, public std::enable_shared_from_this<T> {};
