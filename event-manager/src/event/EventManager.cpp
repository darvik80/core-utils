#include "EventManager.h"
#include "EventManagerLogger.h"

namespace em {
    void EventManager::raiseEvent(const Event &event) {
        em::log::debug("event {}:{}", typeid(event.source()).name(), typeid(event).name());
        if (auto iter = _signals.find(typeid(event)); iter != _signals.end()) {
            iter->second(event);
        } else {
            em::log::warning("dropped event {}", typeid(event).name());
        }
    };
}