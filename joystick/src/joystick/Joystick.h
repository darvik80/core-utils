//
// Created by Ivan Kishchenko on 02.08.2021.
//

#pragma once

#include <string>
#include <string_view>

enum class JoystickType {
    detect,
    xbox,
    ps3,
    gamepad,
};

enum AxisId {
    axis_left,
    axis_mid,
    axis_right,
};

struct JoystickAxis {
    int x{0}, y{0};
};

struct Joystick {
    JoystickType type;
    JoystickAxis axis[3];

    bool rb{false};
    bool lb{false};

    int lt{0};
    int rt{0};

    bool btnA{false};
    bool btnX{false};
    bool btnB{false};
    bool btnY{false};

    bool btnBack{false};
    bool btnXbox{false};
    bool btnStart{false};
};
