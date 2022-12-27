//
// Created by Ivan Kishchenko on 27.12.2022.
//

#include "IotPlatformService.h"
#include "IotProperties.h"
#include "ThingsBoardIotPlatform.h"
#include "CreartsIotPlatform.h"

void IotPlatformService::postConstruct(Registry &registry) {
    BaseService::postConstruct(registry);

    auto props = registry.getProperties<IotProperties>();
    switch (props.type) {
        case IotType::ThingsBoard:
            _delegate = std::make_unique<ThingsBoardIotPlatform>();
            break;
        case IotType::Crearts:
            _delegate = std::make_unique<CreartsIotPlatform>(props.deviceId);
            break;
        case IotType::Yandex:
        default:
            _delegate.reset();
            break;

    }
    if (_delegate) {
        registry.getService<EventBusService>().subscribe<IotTelemetry>(shared_from_this());
        registry.getService<EventBusService>().subscribe<IotMessage>(shared_from_this());
        _delegate->postConstruct(registry);
    }
}

void IotPlatformService::onEvent(const IotTelemetry &telemetry) {
    debug("telemetry: {}", telemetry.message);
    _delegate->telemetry(telemetry.qos, telemetry.message);
}

void IotPlatformService::onEvent(const IotMessage &msg) {
    debug("publish: {}", msg.message);
    _delegate->publish(msg.topic, msg.qos, msg.message);
}

void IotPlatformService::preDestroy(Registry &registry) {
    if (_delegate) {
        _delegate->preDestroy(registry);
    }
}
