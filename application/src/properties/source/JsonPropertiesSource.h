//
// Created by Ivan Kishchenko on 11.04.2021.
//

#pragma once

#include "PropertiesSource.h"
#include <nlohmann/json.hpp>
#include <fstream>

class JsonPropertiesSource : public PropertiesSource {
    nlohmann::json _json;
public:
    explicit JsonPropertiesSource(std::string_view source) {
        _json = nlohmann::json::parse(source);
    }

    explicit JsonPropertiesSource(std::ifstream &stream) {
        if (stream.is_open()) {
            stream >> _json;
        }
    }

    [[nodiscard]] const nlohmann::json &getJson() const {
        return _json;
    }
};

inline void fromJson(JsonPropertiesSource &source, LoggingProperties &props) {
    if (auto it = source.getJson().find("logging"); it != source.getJson().end()) {
        if (auto key = it->find("level"); key != it->end()) {
            key->get_to(props.level);
        }
        if (auto key = it->find("console"); key != it->end()) {
            key->get_to(props.console);
        }
        if (auto key = it->find("file"); key != it->end()) {
            key->get_to(props.file);
        }
        if (auto key = it->find("file-path"); key != it->end()) {
            key->get_to(props.filePath);
        }
    }
}

inline void fromJson(JsonPropertiesSource &source, JoystickProperties &props) {
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