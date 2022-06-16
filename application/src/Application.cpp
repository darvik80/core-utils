//
// Created by Ivan Kishchenko on 11.04.2021.
//

#include "Application.h"

#include "logging/LoggingService.h"
#include "event/EventManagerService.h"
#include "event/ApplicationEvent.h"
#include "scheduler/SchedulerService.h"

#include <fstream>
#include <filesystem>

using namespace boost;

void Application::run(int argc, char **argv) {
    std::string path = std::filesystem::exists("settings.json") ? "settings.json" : "../etc/settings.json";
    std::ifstream props(path);
    Registry registry(props);

    postConstruct(registry);
    run(registry);
    preDestroy(registry);
}

void Application::postConstruct(Registry &registry) {
    auto now = boost::posix_time::microsec_clock::local_time();

    // { System Services
    registry.addService(std::make_shared<LoggingService>());
    registry.addService(std::make_shared<SchedulerService>(registry.getIoService()));
    registry.addService(std::make_shared<EventManagerService>());
    // } System Services

    setup(registry);

    registry.visitService([&registry](auto &service) {
        service.postConstruct(registry);
    });

    registry.getService<EventManagerService>().subscribe<ApplicationStartedEvent>([this, now](const ApplicationStartedEvent &event) -> bool {
        info("application started, {}ms", (boost::posix_time::microsec_clock::local_time()-now).total_milliseconds());
        return true;
    });
}

void Application::run(Registry &registry) {
    auto &ioc = registry.getIoService();
    asio::signal_set signals(ioc);
    signals.add(SIGINT);
    signals.add(SIGTERM);
#if defined(SIGQUIT)
    signals.add(SIGQUIT);
#endif

    auto &eventManager = registry.getService<EventManagerService>();

    signals.async_wait(
            [&eventManager](boost::system::error_code ec, int signal) {
                eventManager.raiseEvent(ApplicationCloseEvent{signal});
            }
    );

    eventManager.subscribe<ApplicationCloseEvent>([&ioc, this](const ApplicationCloseEvent &event) -> bool {
        std::string signal = "unknown";
        switch (event.getSignal()) {
            case SIGTERM:
                signal = "SIGTERM";
                break;
#if defined(SIGQUIT)
            case SIGQUIT:
                signal = "SIGQUIT";
                break;
#endif
            case SIGINT:
                signal = "SIGINT";
                break;
            default:
                break;
        }
        info("handle signal: {}", signal);
        ioc.stop();
        return true;
    });

    eventManager.subscribe<ApplicationShutdownEvent>([this](const ApplicationShutdownEvent &event) -> bool {
        info("shutdown");
        return true;
    });

    eventManager.raiseEvent(ApplicationStartedEvent{});
    registry.getIoService().run();
}

void Application::preDestroy(Registry &registry) {
    registry.visitService([&registry](auto &service) {
        service.preDestroy(registry);
    });
    registry.getService<EventManagerService>().raiseEvent(ApplicationShutdownEvent{});
}

