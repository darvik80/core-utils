//
// Created by Kishchenko Ivan on 23/11/2022.
//

#include "JoystickProperties.h"

void fromJson(JsonPropertiesSource &source, JoystickProperties &props) {
    if (auto it = source.getJson().find("joystick"); it != source.getJson().end()) {
        if (auto key = it->find("type"); key != it->end()) {
            auto type = key->get<std::string>();
            if (type == "xbox") {
                props.type = JoystickType::xbox;
            } else if (type == "ps3") {
                props.type = JoystickType::ps3;
            }
        }

        if (auto key = it->find("name"); key != it->end()) {
            key->get_to(props.name);
        }
    }
}

void fromEnv(EnvPropertiesSource &source, JoystickProperties &props) {
    if (auto val = getenv(PROP_JOYSTICK_TYPE); val != nullptr) {
#ifdef WIN32
        if (0 == strcmp(val, "xbox")) {
            props.type = JoystickType::xbox;
        } else if (0 == strcmp(val, "ps3")) {
            props.type = JoystickType::ps3;
        }
#else
        if (0 == strcasecmp(val, "xbox")) {
            props.type = JoystickType::xbox;
        } else if (0 == strcasecmp(val, "ps3")) {
            props.type = JoystickType::ps3;
        }
#endif
    }

    if (auto val = getenv(PROP_JOYSTICK_NAME); val != nullptr) {
        props.name = val;
    }
}