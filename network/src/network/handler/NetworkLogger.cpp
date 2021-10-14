//
// Created by Ivan Kishchenko on 13.10.2021.
//

#include "NetworkLogger.h"
#include "network/NetworkLogging.h"
#include <iostream>
#include <iomanip>


char byte2hex[256][4];
std::string hexpadding[16];
char byte2char[256][2];
std::string bytepadding[16];

void init() {
    for (int i = 0; i < 256; i++) {
        sprintf(byte2hex[i], " %02x", i);
    }

    for (int i = 0; i < 16; i++) {
        hexpadding[i].clear();
        for (int j = 0; j < 16 - i; j++) {
            hexpadding[i] += "   ";
        }
    }

    for (int i = 0; i < 256; i++) {
        if (i <= 0x1f || i >= 0x7f) {
            byte2char[i][0] = '.';
        } else {
            byte2char[i][0] = (char) i;
        }
        byte2char[i][1] = 0x00;
    }

    for (int i = 0; i < 16; i++) {
        bytepadding[i].clear();
        for (int j = 0; j < 16 - i; j++) {
            bytepadding[i] += " ";
        }
    }
}

std::once_flag flag;

std::string NetworkLogger::dump(const ByteBuf &buf) {
    std::call_once(flag, init);
    if (buf.empty()) {
        return {};
    }

    std::stringstream str;

    str << std::endl;
    str << "         +-------------------------------------------------+" << std::endl;
    str << "         |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f |" << std::endl;
    str << "+--------+-------------------------------------------------+----------------+";

    std::size_t startIndex = 0, endIndex = buf.size();
    std::size_t i = 0;
    for (i = 0; i < endIndex; i++) {
        auto relIdx = i - startIndex;
        auto relIdxMod16 = relIdx & 15;
        if (relIdxMod16 == 0) {
            str << std::endl << "|" << std::setw(8) << std::hex << relIdx << "|";
        }

        str << byte2hex[buf[i]];
        if (relIdxMod16 == 15) {
            str << " |";
            for (size_t j = i - 15; j <= i; j++) {
                str << byte2char[buf[j]];
            }
            str << "|";
        }
    }

    if (((i - startIndex) & 15) != 0) {
        auto remainder = buf.size() & 15;
        str << hexpadding[remainder];
        str << " |";
        for (size_t j = i - remainder; j < i; j++) {
            str << byte2char[buf[j]];
        }
        str << bytepadding[remainder];
        str << "|";
    }
    str << std::endl << "+--------+-------------------------------------------------+----------------+";

    return str.str();
}

void NetworkLogger::handleRead(const ByteBuf &event) {
    network::log::info("read:{}{}", event.size(), dump(event));
    trigger(event);
}

void NetworkLogger::handleWrite(const ByteBuf &event) {
    network::log::info("write:{}{}", event.size(), dump(event));
    write(event);
}
