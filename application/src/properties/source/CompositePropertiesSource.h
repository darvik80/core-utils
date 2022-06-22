//
// Created by Ivan Kishchenko on 26.04.2021.
//

#pragma once

#include "JsonPropertiesSource.h"
#include "EnvPropertiesSource.h"
#include <utility>
#include <vector>

class CompositePropertiesSource : public PropertiesSource {
    JsonPropertiesSource _jsonSource;
    EnvPropertiesSource _envSource{};
public:
    explicit CompositePropertiesSource(std::string_view json)
            : _jsonSource(json) {
    }

    explicit CompositePropertiesSource(std::ifstream &stream)
            : _jsonSource(stream) {
    }

    template<class Props>
    void getProperties(Props &props) {
        fromJson(_jsonSource, props);
        fromEnv(_envSource, props);
    }

};
