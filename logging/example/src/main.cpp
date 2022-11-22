//
// Created by Ivan Kishchenko on 01.08.2021.
//

#include "Logger.h"

int main(int argc, char *argv[]) {
    logger::LoggingProperties logProps;
    logProps.level = "info";
    logger::setup(logProps);

    logger::info("Info");
    logger::warning("Warning");
    //logger::error("Error");

    bus::log::info("em::info");
    bus::log::warning("em::warn");

    mqtt::log::info("mqtt::info");
    mqtt::log::warning("mqtt::warn");

    return 0;
}