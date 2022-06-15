//
// Created by Kishchenko Ivan on 15/06/2022.
//

#pragma once

#include <string_view>

class MQTTUtils {
public:
    static bool compareTopics(std::string_view topic, std::string_view origin);
};
