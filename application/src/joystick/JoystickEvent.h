//
// Created by Ivan Kishchenko on 01.08.2021.
//

#pragma once

#include "event/Event.h"
#include <nlohmann/json.hpp>
#include "Joystick.h"

class JoystickEvent : public em::Event, public Joystick {
};

void to_json(nlohmann::json &j, const JoystickEvent &event);

void from_json(const nlohmann::json &j, JoystickEvent &event);
