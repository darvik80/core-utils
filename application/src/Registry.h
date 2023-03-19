//
// Created by Ivan Kishchenko on 11.04.2021.
//

#pragma once

#include "event/Asio.h"
#include "Service.h"
#include "Properties.h"
#include "properties/source/CompositePropertiesSource.h"
#include <string>

#include "event/Asio.h"
#include <unordered_map>

class Registry {
    friend class LoggingService;

    bus::IOService _service;
    Service::VecPtr _services;
    Properties::VecPtr _properties;

    CompositePropertiesSource _propsSource;
public:
    explicit Registry(std::string_view jsonProps);

    explicit Registry(std::ifstream &fileJsonProps);

    void addService(const Service::Ptr &service);

    template<typename C, typename... T>
    inline C &createService(T &&... all) {
        auto service = std::make_shared<C>(std::forward<T>(all)...);
        addService(service);

        return *service;
    }


        bus::IOService &getIoService();

    void visitService(const std::function<void(Service &service)> &visitor);


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
