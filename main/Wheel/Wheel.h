#pragma once

#include <cstdint>
#include "driver/gpio.h"
#include "RPCCommunication.h"

class Wheel {
public:
    static void set_target_angle(float wheel_ratio);
    static int32_t get_target_angle();
    static void enable();
    static void disable();
    static void init(gpio_num_t enablePin);
private:
    static inline gpio_num_t enablePin = GPIO_NUM_NC;
    static inline int32_t target_angle = 0;
    static inline float wheel_degrees = 620.0;
};
