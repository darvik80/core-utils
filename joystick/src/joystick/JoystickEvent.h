//
// Created by Ivan Kishchenko on 01.08.2021.
//

#pragma once

#include "Joystick.h"

#include <nlohmann/json.hpp>

class JoystickEvent : public Joystick {
};

void to_json(nlohmann::json &j, const JoystickEvent &event);

void from_json(const nlohmann::json &j, JoystickEvent &event);
