//
// Created by Ivan Kishchenko on 27.12.2022.
//
#include "IotPlatform.h"
#include "IotPlatformLogger.h"
#include "network/handler/NetworkLogger.h"
#include "network/handler/IdleStateHandler.h"

void IotPlatform::onMessage(network::mqtt::MQTTAgent &agent, std::string_view topic, std::string_view data) {
    _eventManager->send(IotMessage{
            .topic = topic.data(),
            .qos = 1,
            .message = data.data(),
    });
}

void IotPlatform::onCommand(network::mqtt::MQTTAgent &agent, const IotCommand &cmd) {
    _eventManager->send(cmd);
}

void IotPlatform::onAttributes(network::mqtt::MQTTAgent &agent, std::string_view data) {
    iot::log::info("attributes: {}", data);
    _eventManager->send(IotMessage{"attributes", 1, data.data()});
}

void IotPlatform::onConfig(network::mqtt::MQTTAgent &agent, std::string_view data) {
    iot::log::info("config: {}", data);
    _eventManager->send(IotMessage{"config", 1, data.data()});
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
            [&service, this](const std::shared_ptr<network::AsyncChannel<network::SslSocket>> &channel) {
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
                        std::make_shared<network::handler::IdleStateHandler>(service, boost::posix_time::seconds(5),
                                                                             boost::posix_time::seconds(5)),
                        std::make_shared<network::mqtt::MQTTCodec>(options),
                        _agent
                );
            },
            _props.keyFile
    );

    _client->connect(_props.address, 8883);
}

void IotPlatform::publish(std::string_view topic, uint8_t qos, std::string_view data) {
    if (_agent) {
        iot::log::debug("pub: {}:{}", topic, data);
        _agent->publish(topic, qos, data);
    }
}
