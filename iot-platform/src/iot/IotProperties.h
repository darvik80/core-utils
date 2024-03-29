//
// Created by Ivan Kishchenko on 27.02.2022.
//

#pragma once

#include <string>
#include "properties/source/JsonPropertiesSource.h"
#include "properties/source/EnvPropertiesSource.h"

struct IotProperties : Properties {
    std::string address;

    std::string clientId;
    std::string accessToken;

    std::string username;
    std::string password;

    std::string keyFile;

    std::string registryId;
    std::string deviceId;
};

inline void fromJson(JsonPropertiesSource &source, IotProperties &props) {
    if (auto it = source.getJson().find("iot-mqtt"); it != source.getJson().end()) {
        if (auto key = it->find("address"); key != it->end()) {
            key->get_to(props.address);
        }
        if (auto key = it->find("access-token"); key != it->end()) {
            key->get_to(props.accessToken);
        }
        if (auto key = it->find("username"); key != it->end()) {
            key->get_to(props.username);
        }
        if (auto key = it->find("password"); key != it->end()) {
            key->get_to(props.password);
        }
        if (auto key = it->find("client-id"); key != it->end()) {
            key->get_to(props.clientId);
        }
        if (auto key = it->find("key-file"); key != it->end()) {
            key->get_to(props.keyFile);
        }

        if (auto key = it->find("registry-id"); key != it->end()) {
            props.registryId = key.value();
        }

        if (auto key = it->find("device-id"); key != it->end()) {
            props.deviceId = key.value();
        }

        if (props.deviceId.empty()) {
            props.deviceId = props.clientId;
        }

        if (props.clientId.empty()) {
            props.clientId = props.deviceId;
        }
    }
}

inline void fromEnv(EnvPropertiesSource &source, IotProperties &props) {

}