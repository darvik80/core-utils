//
// Created by Ivan Kishchenko on 13.02.2022.
//

#pragma once

#include "Properties.h"
#include "joystick/Joystick.h"

struct JoystickProperties : Properties {
    JoystickType type{JoystickType::detect};
    std::string name{"/dev/input/js0"};
};