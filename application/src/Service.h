//
// Created by Ivan Kishchenko on 11.04.2021.
//

#ifndef ROVER_SERVICE_H
#define ROVER_SERVICE_H

#include <memory>
#include <vector>

#include "Order.h"

class Registry;

class Service : public Ordered {
public:
    typedef std::shared_ptr<Service> Ptr;
    typedef std::vector<Ptr> VecPtr;
public:
    virtual const char *name() = 0;

public:
    virtual void postConstruct(Registry &registry) = 0;

    virtual void preDestroy(Registry &registry) = 0;

};


#endif //ROVER_SERVICE_H
