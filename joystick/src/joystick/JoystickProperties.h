//
// Created by Ivan Kishchenko on 13.02.2022.
//

#pragma once

#include "Properties.h"
#include "Joystick.h"

#include "properties/source/JsonPropertiesSource.h"
#include "properties/source/EnvPropertiesSource.h"

struct JoystickProperties : Properties {
    JoystickType type{JoystickType::detect};
    std::string name{"/dev/input/js0"};
};

void fromJson(JsonPropertiesSource &source, JoystickProperties &props);

// props: JoystickProperties
#define PROP_JOYSTICK_TYPE      "JOYSTICK_TYPE"         // JoystickProperties.type
#define PROP_JOYSTICK_NAME      "JOYSTICK_NAME"         // JoystickProperties.name

void fromEnv(EnvPropertiesSource &source, JoystickProperties &props);