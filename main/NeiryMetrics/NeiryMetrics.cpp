#include "NeiryMetrics.h"

static const char *TAG = "NEIRYMETRICS";

float NeiryMetrics::get_fatigue_score() { return fatigue_score; }
float NeiryMetrics::get_gravity_score() { return gravity_score; }
float NeiryMetrics::get_concentration_score() { return concentration_score; }
float NeiryMetrics::get_relaxation_score() { return relaxation_score; }
float NeiryMetrics::get_accumulated_fatigue() { return accumulated_fatigue; }
int32_t NeiryMetrics::get_fatigue_growth_rate() { return fatigue_growth_rate; }

float* NeiryMetrics::get_accelerometer_coordinates() { return accelerometer; }
float* NeiryMetrics::get_gyroscope_coordinates() { return gyroscope; }

void NeiryMetrics::set_api_metrics(
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
) {
    NeiryMetrics::fatigue_score = fatigueScore;
    NeiryMetrics::gravity_score = gravityScore;
    NeiryMetrics::concentration_score = concentrationScore;
    NeiryMetrics::relaxation_score = relaxationScore;
    NeiryMetrics::accumulated_fatigue = accumulatedFatigue;
    NeiryMetrics::fatigue_growth_rate = fatigueGrowthRate;

    NeiryMetrics::accelerometer[0] = accelerometerX;
    NeiryMetrics::accelerometer[1] = accelerometerY;
    NeiryMetrics::accelerometer[2] = accelerometerZ;

    NeiryMetrics::gyroscope[0] = gyroscopeX;
    NeiryMetrics::gyroscope[1] = gyroscopeY;
    NeiryMetrics::gyroscope[2] = gyroscopeZ;
}
