//
// Created by Ivan Kishchenko on 10.10.2021.
//

#include "Application.h"
#include "core-service/SchedulerService.h"

class MainApp : public Application {
protected:
    void setup(Registry &registry) override {
        // TODO: init extra libs
        //wiringPiSetup();

        // TODO: register own services
        // registry.addService(std::make_shared<I2CServoDriver>());
        auto &scheduler = registry.getService<SchedulerService>();

        scheduler.scheduleAtFixedRate([]() {
            logger::info("thread IoTPlatformAddIn::schedule");
        }, boost::posix_time::seconds{10}, boost::posix_time::seconds{10});
    }

    void destroy(Registry &registry) override {

    }
};

int main(int argc, char *argv[]) {
    MainApp app;
    app.run(argc, argv);
    return 0;
}

