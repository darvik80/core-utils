//
// Created by Kishchenko Ivan on 17/06/2022.
//

#ifndef RPI_ROBOT_IOTPLATFORM_H
#define RPI_ROBOT_IOTPLATFORM_H


#include "BaseService.h"
#include "IotProperties.h"
#include "network/boost/AsyncTcpClient.h"
#include "network/mqtt/MQTTHandler.h"
#include "network/handler/NetworkLogger.h"
#include "network/mqtt/MQTTCodec.h"
#include "network/handler/IdleStateHandler.h"
#include "IotCommand.h"
#include "event/EventManagerService.h"
#include "IotMessage.h"

#include "IotPlatformLogger.h"

class IotPlatform {
protected:
    using MqttClient = network::AsyncClient<network::SslSocket>;
    std::unique_ptr<MqttClient> _client;
    network::mqtt::MQTTAgent::Ptr _agent;
    IotProperties _props;

    em::EventManager::Ptr _eventManager;
protected:
    virtual void onConnect(network::mqtt::MQTTAgent &agent) = 0;

    void onMessage(network::mqtt::MQTTAgent &agent, std::string_view topic, std::string_view data) {
        _eventManager->raiseEvent(IotMessage{topic, 1, data});
    }

    void onAttributes(network::mqtt::MQTTAgent &agent, std::string_view data) {
        iot::log::info("attributes: {}", data);
        _eventManager->raiseEvent(IotMessage{"attributes", 1, data});
    }

    void onConfig(network::mqtt::MQTTAgent &agent, std::string_view data) {
        iot::log::info("config: {}", data);
        _eventManager->raiseEvent(IotMessage{"config", 1, data});
    }

    void onCommand(network::mqtt::MQTTAgent &agent, const IotCommand &cmd) {
        _eventManager->raiseEvent(cmd);
    }

public:
    virtual void postConstruct(Registry &registry) {
        _eventManager = registry.getService<EventManagerService>().shared_from_this();
        _props = registry.getProperties<IotProperties>();

        auto &service = registry.getIoService();
        _agent = std::make_shared<network::mqtt::MQTTAgent>();
        _agent->connect([this](network::mqtt::MQTTAgent &agent) {
            onConnect(agent);
        });

        _client = std::make_unique<MqttClient>(
                service,
                [&service, this](const std::shared_ptr<network::AsyncChannel<network::SslSocket>> &channel) {
                    link(
                            channel,
                            std::make_shared<network::handler::NetworkLogger>(),
                            std::make_shared<network::handler::IdleStateHandler>(service, boost::posix_time::seconds(5),
                                                                                 boost::posix_time::seconds(5)),
                            std::make_shared<network::mqtt::MQTTCodec>(
                                    network::mqtt::MQTTOptions{
                                            .clientId = _props.clientId,
                                            .accessToken = _props.accessToken
                                    }
                            ),
                            _agent
                    );
                },
                _props.keyFile
        );

        _client->connect(_props.address, 8883);
    }

    virtual void preDestroy(Registry &registry) {
        _client->shutdown();
        _client.reset();
    }

    virtual void publish(std::string_view topic, uint8_t qos, std::string_view data) {
        if (_agent) {
            iot::log::debug("pub: {}:{}", topic, data);
            _agent->publish(topic, qos, data);
        }
    }

    virtual void telemetry(uint8_t qos, std::string_view data) = 0;

    virtual ~IotPlatform() = default;
};


#endif //RPI_ROBOT_IOTPLATFORM_H
