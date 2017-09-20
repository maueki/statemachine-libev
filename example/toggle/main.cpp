
#include <future>
#include <ev++.h>
#include <signal.h>

#include "statemachine.h"
#include "policy.h"
#include "log.h"

using namespace seeds;

struct MyStateMachine : public StateMachine<Policy> {
    using ST = Policy::state_id_t;
    using EV = Policy::event_id_t;

    MyStateMachine(ev::loop_ref loop) : StateMachine("Root", loop) {
        create_states({ST::INIT, ST::ON, ST::OFF});

        add_transition<EV::INIT_COMP>(ST::INIT, ST::OFF);

        add_transition<EV::TOGGLE>(ST::OFF, ST::ON);
        add_transition<EV::TOGGLE>(ST::ON, ST::OFF);

        on_state_entered(ST::INIT, [this](Event *) { send<EV::INIT_COMP>(); });

        on_state_entered(ST::OFF,
                         [this](Event *) { log::d("ST::OFF entered"); });

        on_state_entered(ST::ON, [this](Event *) { log::d("ST::ON entered"); });
    }
};

int main(int argc, char *argv[]) {
    ev::dynamic_loop main_loop;

    MyStateMachine sm(main_loop);

    sm.start();

    auto f = std::async(std::launch::async, [&sm] {
        using EV = Policy::event_id_t;
        using namespace std::chrono_literals;

        for (int i = 0; i < 10; ++i) {
            std::this_thread::sleep_for(500ms);
            sm.send<EV::TOGGLE>();
        }

        raise(SIGTERM);
    });

    main_loop.run(0);

    return 0;
}
