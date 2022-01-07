//
// Created by Ivan Kishchenko on 11.04.2021.
//

#pragma once

#include "Service.h"
#include <logging/Logging.h>

class LoggingService : public Service, public std::enable_shared_from_this<LoggingService> {
public:
    const char *name() override {
        return "log";
    }

    int order() override {
        return INT32_MAX;
    }

    void postConstruct(Registry &registry) override {
        logger::setup(registry.getProperties<LoggingProperties>());
    }

    void preDestroy(Registry &registry) override {

    }
};

