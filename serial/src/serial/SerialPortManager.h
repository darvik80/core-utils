//
// Created by Ivan Kishchenko on 09.04.2021.
//

#ifndef ROVER_SERIALPORTMANAGER_H
#define ROVER_SERIALPORTMANAGER_H

#include "SerialPortInfo.h"

class SerialPortManager {
public:
    std::vector<SerialPortInfo> listPorts();
};


#endif //ROVER_SERIALPORTMANAGER_H
