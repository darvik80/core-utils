//
// Created by Ivan Kishchenko on 11.04.2021.
//

#include "JsonPropertySource.h"

using namespace nlohmann;

JsonPropertySource::JsonPropertySource(std::string_view source) {
    _json = json::parse(source);
}

void JsonPropertySource::getProperties(LoggingProperties &props) {
    if (auto it = _json.find("logging"); it != _json.end()) {
        if (auto key = it->find("level"); key != it->end()) {
            key->get_to(props.level);
        }
        if (auto key = it->find("console"); key != it->end()) {
            key->get_to(props.console);
        }
        if (auto key = it->find("file"); key != it->end()) {
            key->get_to(props.file);
        }
    }
}
