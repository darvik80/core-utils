//
// Created by Ivan Kishchenko on 24.04.2021.
//

#include "BaseService.h"
#include "Registry.h"

BaseService::BaseService() : _logger(service_logger::get()) {}

BaseService::BaseService(logger_type &logger) : _logger(logger) {}

void BaseService::postConstruct(Registry &registry) {
    debug("post constructor");
}

void BaseService::preDestroy(Registry &registry) {
    debug("pre destroy");
}

