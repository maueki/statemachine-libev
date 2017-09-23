#pragma once

#include "event.h"
#include "log.h"

namespace seeds {

struct State;

struct Transition {
    Transition(State* source = nullptr, State* target = nullptr)
        : source_(source)
        , target_(target) {}

    // virtual bool event_test(Event* ev) = 0;

    State* source_state() { return source_; }
    const State* source_state() const { return source_; }

    State* target_state() { return target_; }
    const State* target_state() const { return target_; }

    virtual void transition(Event* ev) = 0;

private:
    State* source_;
    State* target_;
};

template <typename EVENT_CLASS>
struct TransitionImpl : public Transition {
    explicit TransitionImpl(State* source = nullptr, State* target = nullptr)
        : Transition(source, target)
        , func_list_() {}

    void on_transition(typename EVENT_CLASS::CallbackType fn) {
        func_list_.append(fn);
    }

    void on_transition_failed(typename EVENT_CLASS::CallbackType fn) {
        failed_func_list_.append(fn);
    }

protected:
    // bool event_test(Event* ev) override {
    //     auto event = static_cast<EVENT_CLASS*>(ev);
    //     if (event->type() != EVENT_CLASS::event_type) return false;

    //     return true;
    // }

    void transition(Event* ev) override {
        for (auto fn : func_list_) {
            auto event = static_cast<EVENT_CLASS*>(ev);
            event->exec(fn);
        }
    }

private:
    std::list<typename EVENT_CLASS::CallbackType> func_list_;
    std::list<typename EVENT_CLASS::CallbackType> failed_func_list_;
};

}  // seeds
