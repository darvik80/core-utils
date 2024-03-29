//
// Created by Ivan Kishchenko on 09.04.2021.
//

#pragma once

#include "BaseService.h"
#include "Registry.h"
#include "Properties.h"
#include "core-service/ApplicationEvent.h"

class Application : public BaseService {
protected:
    virtual void setup(Registry &registry) = 0;

    virtual void destroy(Registry &registry) {}

public:
    const char *name() override {
        return "application";
    }

    int order() override {
        return 0;
    }

    void run(int argc, char **argv);

    void postConstruct(Registry &registry) override;

    void run(Registry &registry);

    void preDestroy(Registry &registry) override;
};
