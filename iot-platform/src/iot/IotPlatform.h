//
// Created by Kishchenko Ivan on 17/06/2022.
//

#ifndef RPI_ROBOT_IOTPLATFORM_H
#define RPI_ROBOT_IOTPLATFORM_H


#include "BaseService.h"
#include "IotProperties.h"
#include "network/mqtt/MQTTHandler.h"
#include "network/mqtt/MQTTCodec.h"
#include "IotCommand.h"
#include "IotMessage.h"
#include "core-service/EventBusService.h"
#include "network/boost/AsyncTcpClient.h"

class IotPlatform {
protected:
    using MqttClient = network::AsyncClient<network::SslSocket>;
    std::unique_ptr<MqttClient> _client{};
    network::mqtt::MQTTAgent::Ptr _agent{};
    IotProperties _props{};

    bus::EventBus::Ptr _eventManager{};
protected:
    virtual void onPopulateOptions(network::mqtt::MQTTAgent &agent, network::mqtt::MQTTOptions& options) {}
    virtual void onConnect(network::mqtt::MQTTAgent &agent) = 0;

    void onMessage(network::mqtt::MQTTAgent &agent, std::string_view topic, std::string_view data);

    void onAttributes(network::mqtt::MQTTAgent &agent, std::string_view data);

    void onConfig(network::mqtt::MQTTAgent &agent, std::string_view data);

    void onCommand(network::mqtt::MQTTAgent &agent, const IotCommand &cmd);

public:
    virtual void postConstruct(Registry &registry);

    virtual void preDestroy(Registry &registry) {
        _client->shutdown();
        _client.reset();
    }

    virtual void publish(std::string_view topic, uint8_t qos, std::string_view data);

    virtual void telemetry(uint8_t qos, std::string_view data) = 0;

    virtual ~IotPlatform() = default;
};


#endif //RPI_ROBOT_IOTPLATFORM_H
