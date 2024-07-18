#include "Signals.h"
#include "Wheel.h"

static const char *TAG = "WHEEL";

void Wheel::set_target_angle(float wheel_ratio) {
    target_angle = wheel_degrees * wheel_ratio;

    if (target_angle > 45) {
        Signals::set_right_turn(true);
    }
    else if (target_angle < -45) {
        Signals::set_left_turn(true);
    }
    else {
        Signals::set_right_turn(false);
        Signals::set_left_turn(false);
    }

    enable();
}


int32_t Wheel::get_target_angle() {
    return target_angle;
}

void Wheel::disable() {
    gpio_set_level(enablePin, 0);
}

void Wheel::enable() {
    gpio_set_level(enablePin, 1);
}

void Wheel::init(gpio_num_t enablePin) {
    Wheel::enablePin = enablePin;
    gpio_set_direction(enablePin, GPIO_MODE_OUTPUT);
    disable();
}
