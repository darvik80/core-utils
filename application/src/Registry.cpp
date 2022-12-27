//
// Created by Ivan Kishchenko on 27.12.2022.
//
#include "Registry.h"

Registry::Registry(std::string_view jsonProps)
        : _propsSource(jsonProps) {
}

Registry::Registry(std::ifstream &fileJsonProps)
        : _propsSource(fileJsonProps) {
}

void Registry::addService(const Service::Ptr &service) {
    _services.emplace_back(service);
    std::sort(_services.begin(), _services.end(), OrderedLess<Service>());
}

bus::IOService &Registry::getIoService() {
    return _service;
}

void Registry::visitService(const std::function<void(Service &)> &visitor) {
    for (auto &ptr: _services) {
        visitor(*ptr);
    }
}
