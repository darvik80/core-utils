//
// Created by Ivan Kishchenko on 26.04.2021.
//

#pragma once

#include "PropertiesSource.h"
#include <cstdlib>

#include <boost/lexical_cast.hpp>

// props: LoggingProperties
#define PROP_LOGGING_LEVEL      "LOGGING_LEVEL"         // LoggingProperties.level
#define PROP_LOGGING_CONSOLE    "LOGGING_CONSOLE"       // LoggingProperties.console
#define PROP_LOGGING_FILE       "LOGGING_FILE"          // LoggingProperties.file
#define PROP_LOGGING_FILE_PATH  "LOGGING_FILE_PATH"     // LoggingProperties.file

// props: JoystickProperties
#define PROP_JOYSTICK_TYPE      "JOYSTICK_TYPE"         // JoystickProperties.type
#define PROP_JOYSTICK_NAME      "JOYSTICK_NAME"         // JoystickProperties.name

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

inline void fromEnv(EnvPropertiesSource &source, JoystickProperties &props) {
    if (auto val = getenv(PROP_JOYSTICK_TYPE); val != nullptr) {
        if (0 == strcasecmp(val, "xbox")) {
            props.type = JoystickType::xbox;
        } else if (0 == strcasecmp(val, "ps3")) {
            props.type = JoystickType::ps3;
        }
    }

    if (auto val = getenv(PROP_JOYSTICK_NAME); val != nullptr) {
        props.name = val;
    }
}