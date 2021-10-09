//
// Created by Ivan Kishchenko on 15.05.2021.
//

#ifndef CORE_UTILS_PROTOCOL_H
#define CORE_UTILS_PROTOCOL_H


const uint8_t MSG_MAGIC = 0x7e;

enum ConnState {
    IDLE,
    WAIT_SYNC,
    CONN
};

enum SystemMessage {
    MSG_SYNC = 0x01,
    MSG_CONN = 0x02,
    MSG_PING = 0x03,
    MSG_PONG = 0x04,
};

#endif //CORE_UTILS_PROTOCOL_H
