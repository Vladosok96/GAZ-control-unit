#include "Throttle.h"

static const char *TAG = "THROTTLE";

void Throttle::set_target_value(float value) {
    target_value = value;
}

int32_t Throttle::get_target_value() {
    return target_value;
}

void Throttle::set_can_speed(int32_t speed) {
    current_speed = speed;
}

int32_t Throttle::get_can_speed() {
    return current_speed;
}

int32_t Throttle::get_signed_can_speed(int32_t sign) {
    return current_speed * sign;
}

void Throttle::set_can_rpm(int32_t rpm) {
    current_rpm = rpm;
}

int32_t Throttle::get_can_rpm() {
    return current_rpm;
}

int32_t Throttle::getRelay() {
    return enabled ? 1 : 0;
}

void Throttle::enable() {
    enabled = true;
}

void Throttle::disable() {
    enabled = false;
}