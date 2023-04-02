//
// Created by Kishchenko Ivan on 17/06/2022.
//

#ifndef RPI_ROBOT_IOTPLATFORM_H
#define RPI_ROBOT_IOTPLATFORM_H


#include "BaseService.h"
#include "IotProperties.h"
#include "network/mqtt/MQTTHandler.h"
#include "network/mqtt/MQTTCodec.h"
#include "IotMessage.h"
#include "core-service/EventBusService.h"
#include "network/asio/AsyncTcpClient.h"

typedef std::function<void(std::string_view topic, std::string_view data)> fnIoTCallback;

class IotPlatform : public BaseService {
protected:
#if CONFIG_IOT_SSL == 1
    using MqttClient = network::AsyncClient<network::SslSocket>;
    using MqttChannel = network::AsyncChannel<network::SslSocket>;
    const int MqttPort = 8883;
#else
    using MqttClient = network::AsyncClient<network::TcpSocket>;
    using MqttChannel = network::AsyncChannel<network::TcpSocket>;
    const int MqttPort = 1883;
#endif
    std::unique_ptr<MqttClient> _client{};
    network::mqtt::MQTTAgent::Ptr _agent{};
    IotProperties _props{};

    bus::EventBus::Ptr _eventManager{};
protected:
    virtual void onPopulateOptions(network::mqtt::MQTTAgent &agent, network::mqtt::MQTTOptions &options) {}

    virtual void onConnect(network::mqtt::MQTTAgent &agent) = 0;

    void onMessage(network::mqtt::MQTTAgent &agent, std::string_view topic, std::string_view data);

    void onConfig(const IotConfig& data);

    void onRpc(const IotRpcRequest &req);

public:
    void postConstruct(Registry &registry) override;

    void preDestroy(Registry &registry) override {
        _client->shutdown();
        _client.reset();
    }

    virtual void publish(std::string_view topic, uint8_t qos, std::string_view data);

    virtual void subscribe(std::string_view topic, uint8_t qos, const fnIoTCallback &callback);

    virtual ~IotPlatform() = default;
};

class IotDevice : public IotPlatform {
    std::string _registryId;
    std::string _deviceId;
protected:
    void onPopulateOptions(network::mqtt::MQTTAgent &agent, network::mqtt::MQTTOptions &options) override;

    void onConnect(network::mqtt::MQTTAgent &agent) override;

public:
    IotDevice(std::string_view registryId, std::string_view deviceId);

    const char *name() override {
        return "iot-device";
    }

    virtual void telemetry(uint8_t qos, std::string_view data);
};

class IotRegistry : public IotPlatform {
    std::string _registryId;
protected:
    void onConnect(network::mqtt::MQTTAgent &agent) override;
    void onTelemetry(const IotTelemetry& data);
public:
    IotRegistry() {}

    const char *name() override {
        return "iot-registry";
    }

    void postConstruct(Registry &registry) override;

    virtual void config(std::string_view deviceId, const IotConfig& cfg);
    virtual void rpc(std::string_view deviceId, const IotRpcReply& cfg);
};


#endif //RPI_ROBOT_IOTPLATFORM_H
