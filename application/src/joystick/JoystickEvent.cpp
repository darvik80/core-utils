//
// Created by Ivan Kishchenko on 01.08.2021.
//

#include "JoystickEvent.h"

void to_json(nlohmann::json &j, const JoystickEvent &event) {
    j = {
            {"rb",         event.rb},
            {"lb",         event.lb},
            {"rt",         event.rt},
            {"lt",         event.lt},
            {"btn-a",      event.btnA},
            {"btn-x",      event.btnX},
            {"btn-b",      event.btnB},
            {"btn-Y",      event.btnY},
            {"btn-back",   event.btnBack},
            {"btn-start",  event.btnStart},
            {"btn-xbox",   event.btnXbox},
            {"left-axis",  {{"x", event.axis[axis_left].x},  {"y", event.axis[axis_left].y}}},
            {"mid-axis",   {{"x", event.axis[axis_mid].x},   {"y", event.axis[axis_mid].y}}},
            {"right-axis", {{"x", event.axis[axis_right].x}, {"y", event.axis[axis_right].y}}}
    };
}

void from_json(const nlohmann::json &j, JoystickEvent &event) {
    event.rb = j.at("rb").get<bool>();
    event.lb = j.at("lb").get<bool>();

    event.rb = j.at("rt").get<int>();
    event.lb = j.at("lt").get<int>();

    event.btnA = j.at("btn-a").get<bool>();
    event.btnX = j.at("btn-x").get<bool>();
    event.btnB = j.at("btn-b").get<bool>();
    event.btnY = j.at("btn-y").get<bool>();

    event.btnBack = j.at("btn-back").get<bool>();
    event.btnStart = j.at("btn-start").get<bool>();
    event.btnXbox = j.at("btn-xbox").get<bool>();

    auto axis = j.at("left-axis");
    event.axis[axis_left].x = axis.at("x").get<int>();
    event.axis[axis_left].y = axis.at("y").get<int>();

    axis = j.at("right-axis");
    event.axis[axis_right].x = axis.at("x").get<int>();
    event.axis[axis_right].y = axis.at("y").get<int>();

    axis = j.at("mid-axis");
    event.axis[axis_mid].x = axis.at("x").get<int>();
    event.axis[axis_mid].y = axis.at("y").get<int>();
}
