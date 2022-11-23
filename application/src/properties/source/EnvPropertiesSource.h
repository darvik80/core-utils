//
// Created by Ivan Kishchenko on 26.04.2021.
//

#pragma once

#include <cstdlib>
#include <boost/lexical_cast.hpp>

#include "PropertiesSource.h"
#include "properties/LoggingProperties.h"

// props: LoggingProperties
#define PROP_LOGGING_LEVEL      "LOGGING_LEVEL"         // LoggingProperties.level
#define PROP_LOGGING_CONSOLE    "LOGGING_CONSOLE"       // LoggingProperties.console
#define PROP_LOGGING_FILE       "LOGGING_FILE"          // LoggingProperties.file
#define PROP_LOGGING_FILE_PATH  "LOGGING_FILE_PATH"     // LoggingProperties.file

class EnvPropertiesSource : public PropertiesSource {

};

inline void fromEnv(EnvPropertiesSource &source, LoggingProperties &props) {
    if (auto val = getenv(PROP_LOGGING_LEVEL); val != nullptr) {
        props.level = val;
    }
    if (auto val = getenv(PROP_LOGGING_CONSOLE); val != nullptr) {
        props.console = boost::lexical_cast<bool>(val);
    }
    if (auto val = getenv(PROP_LOGGING_FILE); val != nullptr) {
        props.file = boost::lexical_cast<bool>(val);
    }
    if (auto val = getenv(PROP_LOGGING_FILE_PATH); val != nullptr) {
        props.filePath = val;
    }
}
