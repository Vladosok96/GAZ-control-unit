#include "Signals.h"

void Signals::set_left_turn(bool state) { left_turn = state; }

void Signals::set_right_turn(bool state) { right_turn = state; }

void Signals::set_high_beam(bool state) { high_beam = state; }

void Signals::set_beep(bool state) { beep = state; }

void Signals::set_alarm(bool state) { alarm = state; }

void Signals::set_sound_signal(bool state) { sound_signal = state; }

int32_t Signals::get_signals_bitmask() {
    int32_t signals_bitmask = 0;

    signals_bitmask |= (left_turn ? 1 : 0) << SignalBits::LEFT;
    signals_bitmask |= (right_turn ? 1 : 0) << SignalBits::RIGHT;
    signals_bitmask |= (high_beam ? 1 : 0) << SignalBits::LIGHT;
    signals_bitmask |= (beep ? 1 : 0) << SignalBits::BEEP;
    signals_bitmask |= (alarm ? 1 : 0) << SignalBits::ALARM;
    signals_bitmask |= (sound_signal ? 1 : 0) << SignalBits::SIGNAL;

    return signals_bitmask;
}

const char* Signals::get_turns() {
    if (left_turn && right_turn) {
        return "left&right";
    }
    else if (left_turn) {
        return "left";
    }
    else if (right_turn) {
        return "right";
    }

    return "0";
}
