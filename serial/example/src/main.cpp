
#include "serial/SerialLogger.h"
#include "serial/SerialPortManager.h"

int main(int argc, char *argv[]) {
    logger::LoggingProperties logProps;
    logProps.level = "info";
    logger::setup(logProps);

    SerialPortManager manager;

    std::vector<std::string> ports;
    for (const auto &port: manager.listPorts()) {
        ports.push_back(port.port);
    }
    serial::log::debug("Ports: {}", ports);

    return 0;
}