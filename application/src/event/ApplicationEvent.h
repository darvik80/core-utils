//
// Created by Ivan Kishchenko on 26.04.2021.
//

#ifndef ROVER_APPLICATIONEVENT_H
#define ROVER_APPLICATIONEVENT_H

struct ApplicationStartedEvent {

};

struct ApplicationCloseEvent {
    int signal{0};
};

struct ApplicationShutdownEvent {

};

#endif //ROVER_APPLICATIONEVENT_H
