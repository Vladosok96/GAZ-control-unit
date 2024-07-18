#pragma once

#include "Clutch.h"

class CruiseControl {
public:
    static void init();
    static void set_target_speed(float speed);
    static void enable();
    static void disable();

private:
    enum cruiseStates {
        PARKING,
        CLUTCHPUSH,
        GEAR,
        RPM,
        RELEASE,
        CLUTCHHOLD,
        DRIVE,
        STOPPING
    };
    static int calculate_rpm(int gear, float speed);
    static inline cruiseStates current_state;
    static inline int target_gear;
    static inline int next_gear;
    static inline Clutch::ClutchState current_clutch_state;
    static inline float target_brake;
    static inline int target_rpm;
    static inline float target_speed;
    static inline float gear_coefficients[6];
    static inline float gearbox_coefficient;
    static inline bool enabled = false;
    static inline int dirrection;
    static void cruise_update_task(void *args);
    static void check_stop_park();
};