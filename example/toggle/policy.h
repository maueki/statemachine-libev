#pragma once

struct Policy {
    enum STATE {
        INIT = 0,
        ON,
        OFF,
    };

    enum EVENT {
        INVALID = 0,
        INIT_COMP,
        TOGGLE,
    };

    using state_id_t = STATE;
    using event_id_t = EVENT;

    static std::string to_string(STATE st) {
        switch (st) {
        case INIT:
            return "INIT";
        case ON:
            return "ON";
        case OFF:
            return "OFF";
        }

        return "UNKNOWN";
    }

    static std::string to_string(EVENT ev) {
        switch (ev) {
        case INVALID:
            return "INVALID";
        case INIT_COMP:
            return "INIT_COMP";
        case TOGGLE:
            return "TOGGLE";
        }
        return "UNKNOWN";
    }
};

DEFINE_EVENT(Policy::INIT_COMP);
DEFINE_EVENT(Policy::TOGGLE);
