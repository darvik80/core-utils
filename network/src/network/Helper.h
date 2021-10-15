//
// Created by Ivan Kishchenko on 15.10.2021.
//

#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <algorithm>

namespace network {
    template<typename T>
    constexpr T htonT(T value) noexcept {
#if __BYTE_ORDER == __LITTLE_ENDIAN
        char *ptr = reinterpret_cast<char *>(&value);
        std::reverse(ptr, ptr + sizeof(T));
#endif
        return value;
    }

    class Helper {
    public:
        static uint64_t swapEndian(uint64_t val) {
            return htonT(val);
        }

        static uint32_t swapEndian(uint32_t val) {
            return htonT(val);
        }

        static uint16_t swapEndian(uint16_t val) {
            return htonT(val);
        }
    };

}