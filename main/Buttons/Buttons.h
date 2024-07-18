#pragma once

#include <stdint.h>

class Buttons {
public:
    enum BUTTONS {
        BRAKES = 0,
        CLUTCH = 1,
        WHEEL = 2,
        AUTOPILOT = 3,
        GEARBOX = 4,
    };

    enum REMOTE_BUTTONS {
        STOP = 0,
        PAUSE = 1,
        START = 2,
    };

    static bool get_button_state(BUTTONS button_index);
    static bool get_remote_button_state(REMOTE_BUTTONS button_index);
    static void set_buttons_state(int32_t state);
    static void set_remote_buttons_state(int32_t state);
    static bool remote_connected();
private:
    static inline uint32_t buttons_state = 0;
    static inline uint32_t remote_buttons_state = 0;
    static inline int64_t remote_last_received = 0;
};
