//
// Created by Ivan Kishchenko on 11.04.2021.
//

#ifndef ROVER_REGISTRY_H
#define ROVER_REGISTRY_H

#include "Service.h"
#include "Properties.h"
#include "properties/source/CompositePropertiesSource.h"
#include <string>

#include <boost/asio.hpp>
#include <unordered_map>

class Registry {
    friend class LoggingService;

    boost::asio::io_service _service;
    Service::VecPtr _services;
    Properties::VecPtr _properties;

    CompositePropertiesSource _propsSource;
public:
    explicit Registry(std::string_view jsonProps)
            : _propsSource(jsonProps) {
    }

    explicit Registry(std::ifstream &fileJsonProps)
            : _propsSource(fileJsonProps) {
    }

    void addService(const Service::Ptr &service) {
        _services.emplace_back(service);
        std::sort(_services.begin(), _services.end(), OrderedLess<Service>());
    }

    boost::asio::io_service &getIoService() {
        return _service;
    }

    void visitService(const std::function<void(Service &service)> &visitor) {
        for (auto &ptr: _services) {
            visitor(*ptr);
        }
    }


    template<class C>
    C &getService() {
        for (auto &ptr: _services) {
            const C *pC = dynamic_cast<const C *>(ptr.get());
            if (pC) {
                return *const_cast<C *>(pC);
            }
        }

        throw std::invalid_argument(std::string("The service has not been registered ") + typeid(C).name());
    }

    template<class C>
    C getProperties() {
        C props;
        _propsSource.getProperties(props);
        return props;
    }

private:
    friend class Application;
};


#endif //ROVER_REGISTRY_H
