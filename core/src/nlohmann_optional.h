//
// Created by Ivan Kishchenko on 02/07/2023.
//

#pragma once

#include <nlohmann/json.hpp>
#include <optional>

namespace nlohmann {
    template<typename T>
    struct adl_serializer<std::optional<T>> {
        static void to_json(json &j, const std::optional<T> &opt) {
            if (opt == std::nullopt) {
                //j = nullptr;
            } else {
                j = *opt; // this will call adl_serializer<T>::to_json which will
                // find the free function to_json in T's namespace!
            }
        }

        static void from_json(const json &j, std::optional<T> &opt) {
            if (j.is_null()) {
                opt = std::nullopt;
            } else {
                opt = j.get<T>(); // same as above, but with
                // adl_serializer<T>::from_json
            }
        }
    };

}
