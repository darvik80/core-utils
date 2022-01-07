//
// Created by Ivan Kishchenko on 09.04.2021.
//

#pragma once

#include "BaseService.h"
#include "Registry.h"
#include "Properties.h"
#include "event/ApplicationEvent.h"

class Application : public BaseService {
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
