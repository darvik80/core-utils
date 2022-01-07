//
// Created by Ivan Kishchenko on 27.04.2021.
//

#pragma once

#include <queue>

class Ordered {
public:
    virtual int order() = 0;
};

template<typename T>
struct OrderedLess
{
    bool operator()(std::shared_ptr<T> first, std::shared_ptr<T> second) const {
        int orderFirst = 0;
        if (const auto obj = dynamic_cast<Ordered*>(first.get()); obj) {
            orderFirst = obj->order();
        }

        int orderSecond= 0;
        if (const auto obj = dynamic_cast<Ordered*>(second.get()); obj) {
            orderSecond = obj->order();
        }

        return orderFirst > orderSecond;
    }
};

template<typename T>
using PriorityVector = std::priority_queue<T, std::vector<T>, OrderedLess<T>>;
