//
// Created by Kishchenko Ivan on 17/06/2022.
//

#pragma once

#include <string>
#include <nlohmann/json.hpp>

struct IotMessage {
    std::string topic;
    int qos{0};
    std::string message;
};

struct IotConfig {
    nlohmann::json data;
};

inline void from_json(const nlohmann::json &j, IotConfig &cfg) {
    j.at("data").get_to(cfg.data);
}

inline void to_json(nlohmann::json &j, const IotConfig &cfg) {
    j = {
            {"data", cfg.data},
    };
}

struct IotTelemetry {
    std::string deviceId;
    nlohmann::json message;
};

struct IotRpcRequest {
    std::string name;
    nlohmann::json params;
    std::string uuid;
};

inline void from_json(const nlohmann::json &j, IotRpcRequest &req) {
    j.at("method").get_to(req.name);
    j.at("params").get_to(req.params);
    j.at("uuid").get_to(req.uuid);
}

inline void to_json(nlohmann::json &j, const IotRpcRequest &req) {
    j = {
            {"method", req.name},
            {"params", req.params},
            {"uuid",   req.uuid},
    };
}

struct IotRpcReply {
    std::string name;
    nlohmann::json params;
    std::string uuid;
};

inline void to_json(nlohmann::json &j, const IotRpcReply &reply) {
    j = {
            {"method", reply.name},
            {"params", reply.params},
            {"uuid",   reply.uuid},
    };
}
