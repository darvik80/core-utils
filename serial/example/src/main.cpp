
#include "serial/SerialLogger.h"
#include "serial/SerialPortManager.h"

int main(int argc, char *argv[]) {
    auto props = logging::LoggingProperties{};
    logging::setup(props);

    SerialPortManager manager;

    std::vector<std::string> ports;
    for (const auto& port : manager.listPorts()) {
        ports.push_back(port.port);
    }
    serial::log::debug("Ports: {}", ports);

    return 0;
}