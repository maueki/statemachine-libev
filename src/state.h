
#include <ev++.h>
#include <list>
#include <string>
#include <map>
#include <cassert>
#include <mutex>
#include <queue>
#include <memory>

#include "event.h"
#include "log.h"

namespace seeds {

struct State {
    State(const std::string& name, State* parent = nullptr)
        : name_(name), parent_(parent) {
        if (parent) {
            parent->add_child(this);
        }
    }

    virtual ~State() {
        for (auto&& child : children_) {
            delete child;
        }
    }

    void enter(Event* event) {
        if (parent_)
            parent_->enter_child(event, this);

        log::d("enter state: %s", name_.c_str());
        is_active_ = true;

        do_enter_callback(event);

        if (!children_.empty()) {
            active_child_ = children_.front();
            active_child_->enter(event);
        }
    }

    void exit(Event* event) {
        if (active_child_) {
            active_child_->exit(event);
            active_child_ = nullptr;
        }

        log::d("exit state: %s", name_.c_str());
        is_active_ = false;

        do_exit_callback(event);
    }

    void walk(std::function<void (State*)> fn){
        if (active_child_) {
            active_child_->walk(fn);
        }

        fn(this);
    }

    const bool is_active() const { return is_active_; }

    const State* parent() const { return parent_; }
    State* parent() { return parent_; }

    const std::string name() { return name_; }

private:
    void add_child(State* child) {
        if (!child) return;

        children_.push_back(child);
    }

    virtual void do_enter_callback(Event*) {}
    virtual void do_exit_callback(Event*) {}

protected:
    std::string name_;

    virtual void enter_child(Event* event, State* child) {
        active_child_ = child;

        if (is_active_)
            return;

        // FIXME: Error when having parallel states.

        if (parent_)
            parent_->enter_child(event, this);

        log::d("enter state: %s", name_.c_str());

        is_active_ = true;
        do_enter_callback(event);
    }

private:
    State* parent_;
    bool is_active_ = false;
    std::list<State*> children_;
    State* active_child_;
};

template <typename EVENT_ENUM>
struct StateImpl : public State {
    StateImpl() = delete;

    StateImpl(const std::string& name, State* parent = nullptr)
        : State(name, parent) {}

    void on_entered(std::function<void (EventBase<EVENT_ENUM>*)> fn) {
        on_entered_callbacks_.push_back(fn);
    }

    void on_exited(std::function<void (EventBase<EVENT_ENUM>*)> fn) {
        on_exited_callbacks_.push_back(fn);
    }

    void do_enter_callback(Event* event) override {
        for(auto& fn: on_entered_callbacks_) {
            fn(static_cast<EventBase<EVENT_ENUM>*>(event));
        }
    }

    void do_exit_callback(Event* event) override {
        for(auto& fn: on_exited_callbacks_) {
            fn(static_cast<EventBase<EVENT_ENUM>*>(event));
        }
    }

private:
    std::list<std::function<void(EventBase<EVENT_ENUM>*)>> on_entered_callbacks_;
    std::list<std::function<void(EventBase<EVENT_ENUM>*)>> on_exited_callbacks_;
};

}  // namespace seeds
