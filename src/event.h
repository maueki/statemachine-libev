#pragma once

#include <functional>

template <typename EVENT, int EVENT_NO>
struct _EventCreator {};

namespace seeds {

class Event {};

template <typename EVENT_ENUM>
class EventBase : public Event {
    EVENT_ENUM event_type_;
    std::function<void()> on_delete_fn_;

public:
    EventBase(EVENT_ENUM event_type)
        : event_type_(event_type) {}

    ~EventBase() {
        if (on_delete_fn_) on_delete_fn_();
    }

    EVENT_ENUM type() const { return event_type_; };

    void on_delete(std::function<void()> fn) { on_delete_fn_ = fn; }
};

template <typename EVENT_ENUM, EVENT_ENUM EVENT>
class EventImpl : public EventBase<EVENT_ENUM> {
    static_assert(EVENT != static_cast<EVENT_ENUM>(0), "EVENT is DISABLE");

    EventImpl()
        : EventBase<EVENT_ENUM>(EVENT) {}

public:
    using CallbackType = std::function<void()>;
    static const EVENT_ENUM event_type = EVENT;

    static EventImpl* create() { return new EventImpl(); }

    void exec(std::function<void()> fn) { fn(); }
};

#define DEFINE_EVENT(EVENT)                                            \
    template <>                                                        \
    struct _EventCreator<decltype(EVENT), static_cast<int>(EVENT)> {   \
        using EVENT_CLASS = seeds::EventImpl<decltype(EVENT), EVENT>;  \
        static seeds::Event* create() {                                \
            return seeds::EventImpl<decltype(EVENT), EVENT>::create(); \
        }                                                              \
    };

template <typename EVENT_ENUM, EVENT_ENUM EVENT, typename DATATYPE>
class EventImplWithData : public EventBase<EVENT_ENUM> {
    static_assert(EVENT != static_cast<EVENT_ENUM>(0), "EVENT is DISABLE");

    EventImplWithData(const DATATYPE& data)
        : EventBase<EVENT_ENUM>(EVENT)
        , data(data) {}

public:
    const DATATYPE data;
    using CallbackType = std::function<void(DATATYPE)>;
    static const EVENT_ENUM event_type = EVENT;

    static EventImplWithData* create(const DATATYPE& data) {
        return new EventImplWithData(data);
    }

    void exec(CallbackType fn) { fn(data); }
};

#define DEFINE_EVENT_WITH_DATA(EVENT, DATATYPE)                         \
    template <>                                                         \
    struct _EventCreator<decltype(EVENT), static_cast<int>(EVENT)> {    \
        using EVENT_CLASS =                                             \
            seeds::EventImplWithData<decltype(EVENT), EVENT, DATATYPE>; \
        static seeds::Event* create(const DATATYPE& arg) {              \
            return EVENT_CLASS::create(arg);                            \
        }                                                               \
    };

template <typename EVENT, typename EVENT_ENUM>
inline EVENT* event_cast(EventBase<EVENT_ENUM>* ev) {
    if (ev->EventType() == static_cast<EVENT_ENUM>(EVENT::event_type)) {
        return static_cast<EVENT*>(ev);
    }

    return nullptr;
}

}  // seeds
