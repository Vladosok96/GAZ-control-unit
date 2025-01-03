#pragma once

#include <cstdint>

class Throttle {
public:
    static void set_target_value(float value);
    static int32_t get_target_value();
    static void set_can_speed(int32_t speed);
    static int32_t get_can_speed();
    static int32_t get_signed_can_speed(int32_t sign);
    static void set_can_rpm(int32_t rpm);
    static int32_t get_can_rpm();
    static void enable();
    static void disable();
    static int32_t getRelay();
private:
    static inline int32_t target_value = 0;
    static inline int32_t current_speed = 0;
    static inline int32_t current_rpm = 0;
    static inline bool enabled = false;
};
