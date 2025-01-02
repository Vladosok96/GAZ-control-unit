#include "PCCommunication.h"
#include "esp_log.h"

static const char *TAG = "PCCOMMUNICATION";

float PCCommunication::get_target_speed() { return target_speed; }

float PCCommunication::get_target_wheel_angle() { return target_wheel_angle; }

void PCCommunication::set_parameters(double speed, double angle) {
    PCCommunication::target_speed = speed;
    PCCommunication::target_wheel_angle = angle;
    ESP_LOGW(TAG, "wheel: %f speed: %f", angle, speed);
}
