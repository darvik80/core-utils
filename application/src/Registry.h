//
// Created by Ivan Kishchenko on 11.04.2021.
//

#ifndef ROVER_REGISTRY_H
#define ROVER_REGISTRY_H

#include "Service.h"
#include "Properties.h"
#include <string>

#include <boost/asio.hpp>
#include <unordered_map>
#include <properties/source/PropertySource.h>

class Registry {
    friend class LoggingService;

    boost::asio::io_service _service;
    Service::VecPtr _services;
    Properties::VecPtr _properties;

    PropertySource::Ptr _propsSource;

protected:
    void addService(const Service::Ptr &service) {
        _services.emplace_back(service);
        std::sort(_services.begin(), _services.end(), OrderedLess<Service>());
    }

public:
    explicit Registry(PropertySource::Ptr propsSource)
            : _propsSource(std::move(propsSource)) {
    }


    boost::asio::io_service &getIoService() {
        return _service;
    }

    void visitService(const std::function<void(Service& service)>& visitor) {
        for (auto &ptr : _services) {
            visitor(*ptr);
        }
    }


    template<class C>
    C &getService() {
        for (auto &ptr : _services) {
            const C *pC = dynamic_cast<const C *>(ptr.get());
            if (pC) {
                return *const_cast<C *>(pC);
            }
        }

        throw std::invalid_argument(std::string("The service has not been registered ") + typeid(C).name());
    }

    template<class C>
    C &getProperties() {
        for (auto &ptr : _properties) {
            const C *pC = dynamic_cast<const C *>(ptr.get());
            if (pC) {
                return *const_cast<C *>(pC);
            }
        }

        auto ptr = std::make_shared<C>();
        _propsSource->getProperties(*ptr);
        _properties.emplace_back(ptr);

        return *ptr;
    }

private:
    friend class Application;
};


#endif //ROVER_REGISTRY_H
