//
// Created by Ivan Kishchenko on 11.04.2021.
//

#pragma once

#include <string>
#include <string_view>
#include <memory>
#include "properties/LoggingProperties.h"

class PropertySource {
public:
    typedef std::shared_ptr<PropertySource> Ptr;
public:
    virtual void getProperties(LoggingProperties& props) = 0;
    ~PropertySource() = default;
};
