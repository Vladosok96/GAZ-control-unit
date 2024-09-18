#pragma once

#include <stdint.h>

class NeiryMetrics {
public:
    static float get_fatigue_score();
    static float get_gravity_score();
    static float get_concentration_score();
    static float get_relaxation_score();
    static float get_accumulated_fatigue();
    static int32_t get_fatigue_growth_rate();

    static float* get_accelerometer_coordinates();
    static float* get_gyroscope_coordinates();

    static void set_api_metrics(
        double fatigueScore, 
        double gravityScore, 
        double concentrationScore, 
        double relaxationScore, 
        double accumulatedFatigue, 
        int32_t fatigueGrowthRate, 
        double accelerometerX, 
        double accelerometerY, 
        double accelerometerZ, 
        double gyroscopeX, 
        double gyroscopeY, 
        double gyroscopeZ
    );
private:
    static inline float fatigue_score = 0.0, gravity_score = 0.0, concentration_score = 0.0, relaxation_score = 0.0, accumulated_fatigue = 0.0;
    static inline int32_t fatigue_growth_rate = 0;

    static inline float accelerometer[3] = {0.0, 0.0, 0.0}; // X, Y, Z
    static inline float gyroscope[3] = {0.0, 0.0, 0.0}; //
};
