//
// Created by Ivan Kishchenko on 10.10.2021.
//

#include "Application.h"

class MainApp : public Application {
protected:
    void setup(Registry &registry) override {
        // TODO: init extra libs
        //wiringPiSetup();

        // TODO: register own services
        // registry.addService(std::make_shared<I2CServoDriver>());
    }
};

int main(int argc, char *argv[]) {
    MainApp app;
    app.run(argc, argv);
    return 0;
}

