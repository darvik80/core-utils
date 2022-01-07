//
// Created by Ivan Kishchenko on 26.04.2021.
//

#pragma once

#include "PropertySource.h"
#include <cstdlib>

#include <boost/lexical_cast.hpp>

// props: LoggingProperties
#define PROP_LOGGING_LEVEL      "LOGGING_LEVEL"         // LoggingProperties.level
#define PROP_LOGGING_CONSOLE    "LOGGING_CONSOLE"       // LoggingProperties.console
#define PROP_LOGGING_FILE       "LOGGING_FILE"          // LoggingProperties.file

class EnvPropertySource : public PropertySource {
public:
    void getProperties(LoggingProperties &props) override {
        if (auto val = getenv(PROP_LOGGING_LEVEL); val != nullptr) {
            props.level = val;
        }
        if (auto val = getenv(PROP_LOGGING_CONSOLE); val != nullptr) {
            props.console = boost::lexical_cast<bool>(val);
        }
        if (auto val = getenv(PROP_LOGGING_FILE); val != nullptr) {
            props.file = boost::lexical_cast<bool>(val);
        }
    }
};
