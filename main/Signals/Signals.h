#pragma once

#include "inttypes.h"

class Signals {
public:
    static void set_left_turn(bool state);
    static void set_right_turn(bool state);
    static void set_high_beam(bool state);
    static void set_beep(bool state);
    static void set_alarm(bool state);
    static void set_sound_signal(bool state);
    static int32_t get_signals_bitmask();
    static const char* get_turns();
private:
    enum SignalBits {
        LEFT = 0,
        RIGHT,
        LIGHT,
        BEEP,
        ALARM,
        SIGNAL,
    };

    static inline bool left_turn = false;
    static inline bool right_turn = false;
    static inline bool high_beam = false;
    static inline bool beep = false;
    static inline bool alarm = false;
    static inline bool sound_signal = false;
};