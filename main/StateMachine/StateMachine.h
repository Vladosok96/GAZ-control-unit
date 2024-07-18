#pragma once

#include <string>

class StateMachine {
public:
    enum Mode {
        EStop = 0,
        Manual = 1,
        Auto = 2,
        Pause = 3,
        Disable = 4,
    };

    static void init();
    static Mode get_current_control_mode();
    static void set_control_mode(Mode mode);
    static void set_pause_state(std::string software_state);
    static const char* mode_to_string(Mode mode);
    static int32_t get_light_id();
private:
    static inline Mode current_control_mode;
};
