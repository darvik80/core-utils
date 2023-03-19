//
// Created by Ivan Kishchenko on 27.12.2022.
//
#include "IotPlatform.h"
#include "IotPlatformLogger.h"
#include "network/handler/NetworkLogger.h"
#include "network/handler/IdleStateHandler.h"

#include <boost/algorithm/string.hpp>

#define IOT_TOPIC_RPC "/rpc/req"
#define IOT_TOPIC_CONFIG "/config"
#define IOT_TOPIC_TELEMETRY "/telemetry"

void IotPlatform::onMessage(network::mqtt::MQTTAgent &agent, std::string_view topic, std::string_view data) {
    _eventManager->send(IotMessage{
            .topic = topic.data(),
            .qos = 1,
            .message = data.data(),
    });
}

void IotPlatform::onConfig(const IotConfig& cfg) {
    _eventManager->send(cfg);
}

void IotPlatform::onRpc(const IotRpcRequest &req) {
    _eventManager->send(req);
}

void IotPlatform::postConstruct(Registry &registry) {
    _eventManager = registry.getService<EventBusService>().shared_from_this();
    _props = registry.getProperties<IotProperties>();

    auto &service = registry.getIoService();
    _agent = std::make_shared<network::mqtt::MQTTAgent>();
    _agent->connect([this](network::mqtt::MQTTAgent &agent) {
        onConnect(agent);
    });

    _client = std::make_unique<MqttClient>(
            service,
            [&service, this](const std::shared_ptr<MqttChannel> &channel) {
                auto options = network::mqtt::MQTTOptions{
                        .clientId = _props.clientId,
                        .accessToken = _props.accessToken,
                        .username = _props.username,
                        .password = _props.password
                };
                onPopulateOptions(*_agent, options);
                link(
                        channel,
                        std::make_shared<network::handler::NetworkLogger>(),
                        std::make_shared<network::handler::IdleStateHandler>(
                                service,
                                boost::posix_time::seconds(5),
                                boost::posix_time::seconds(5)
                        ),
                        std::make_shared<network::mqtt::MQTTCodec>(options),
                        _agent
                );
            },
            _props.keyFile
    );

    _client->connect(_props.address, MqttPort);
}

void IotPlatform::publish(std::string_view topic, uint8_t qos, std::string_view data) {
    if (_agent) {
        iot::log::debug("pub: {}:{}:{}", topic, qos, data);
        _agent->publish(topic, qos, data);
    }
}

void IotPlatform::subscribe(std::string_view topic, uint8_t qos, const fnIoTCallback &callback) {
    if (_agent) {
        iot::log::debug("sub: {}:{}", topic, qos);
        _agent->callback(topic,
                         [callback](network::mqtt::MQTTAgent &agent, std::string_view topic, std::string_view data) {
                             callback(topic, data);
                         });
        _agent->subscribe(topic, qos);
    }
}

IotDevice::IotDevice(std::string_view registryId, std::string_view deviceId) : _registryId(registryId), _deviceId(deviceId) {

}

void IotDevice::onPopulateOptions(network::mqtt::MQTTAgent &agent, network::mqtt::MQTTOptions &options) {

}

void IotDevice::onConnect(network::mqtt::MQTTAgent &agent) {
    std::string prefix = fmt::format("/{}/{}", _registryId, _deviceId);

    agent.callback(prefix + IOT_TOPIC_CONFIG, [this](auto &agent, std::string_view topic, std::string_view data) {
        iot::log::debug("cfg: {}:{}", topic, data);
        IotConfig cfg;
        from_json(nlohmann::json::parse(data), cfg);
        onConfig(cfg);
    });
    agent.callback(prefix + IOT_TOPIC_RPC, [this](auto &agent, std::string_view topic, std::string_view data) {
        iot::log::debug("rpc: {}:{}", topic, data);
        IotRpcRequest req;
        from_json(nlohmann::json::parse(data), req);
        onRpc(req);
    });

    agent.subscribe(prefix + IOT_TOPIC_CONFIG, 1);
    agent.subscribe(prefix + IOT_TOPIC_RPC, 1);

}

void IotDevice::telemetry(uint8_t qos, std::string_view data) {
    IotPlatform::publish(fmt::format("/{}/{}/{}", _registryId, _deviceId, IOT_TOPIC_TELEMETRY), qos, data);
}

IotRegistry::IotRegistry(std::string_view registryId) : _registryId(registryId) {

}

void IotRegistry::onConnect(network::mqtt::MQTTAgent &agent) {
    std::string prefix = fmt::format("/{}/+", _registryId);

    agent.callback(prefix + IOT_TOPIC_RPC, [this](auto &agent, std::string_view topic, std::string_view data) {
        iot::log::debug("rpc: {}:{}", topic, data);
        IotRpcRequest rpc;
        from_json(nlohmann::json::parse(data), rpc);
        onRpc(rpc);
    });

    agent.callback(prefix + IOT_TOPIC_TELEMETRY, [this](auto &agent, std::string_view topic, std::string_view data) {
        iot::log::debug("telemetry: {}:{}", topic, data);

        IotTelemetry telemetry;

        std::vector<std::string> parts;
        telemetry.deviceId = boost::split(parts, topic, boost::is_any_of("/")).back();
        telemetry.message = nlohmann::json::parse(data);
        onTelemetry(telemetry);
    });

    agent.subscribe(prefix + IOT_TOPIC_CONFIG, 1);
    agent.subscribe(prefix + IOT_TOPIC_RPC, 1);

}

void IotRegistry::onTelemetry(const IotTelemetry& data) {
    _eventManager->send(data);
}


void IotRegistry::config(std::string_view deviceId, const IotConfig &cfg) {
    IotPlatform::publish(fmt::format("/{}/{}", _registryId, IOT_TOPIC_CONFIG), 1, cfg.data.dump());
}

void IotRegistry::rpc(std::string_view deviceId, const IotRpcReply &cfg) {

}
