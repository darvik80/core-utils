//
// Created by Ivan Kishchenko on 09.04.2021.
//

#ifndef ROVER_SERIALPORTINFO_H
#define ROVER_SERIALPORTINFO_H

#include <string>

struct SerialPortInfo {

    /*! Address of the serial port (this can be passed to the constructor of Serial). */
    std::string port{};

    /*! Human readable description of serial device if available. */
    std::string description{};

    /*! Hardware ID (e.g. VID:PID of USB serial devices) or "n/a" if not available. */
    std::string hardwareId{};

};


#endif //ROVER_SERIALPORTINFO_H
