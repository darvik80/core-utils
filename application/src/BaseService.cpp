//
// Created by Ivan Kishchenko on 24.04.2021.
//

#include "BaseService.h"
#include "Registry.h"

void BaseService::postConstruct(Registry &registry) {
    debug("post constructor");
}

void BaseService::preDestroy(Registry &registry) {
    debug("pre destroy");
}

