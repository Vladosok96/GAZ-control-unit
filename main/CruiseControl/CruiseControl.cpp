#include "CruiseControl.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "Gearbox.h"
#include "Brake.h"
#include "Throttle.h"
#include "esp_timer.h"
#include "esp_log.h"

static const char *TAG = "cruise";


void CruiseControl::enable() {
    enabled = true;
}

void CruiseControl::disable() {
    enabled = false;
    current_state = PARKING;
    target_gear = 0;
    is_starting = true;
    next_gear = 0;
    current_clutch_state = Clutch::Press;
    target_brake = 1;
    target_rpm = 1000;
    target_speed = 0;
}

void CruiseControl::check_stop_park() {
    if (dirrection > 0 && Throttle::get_signed_can_speed(dirrection) - target_speed > 2) {
        current_state = STOPPING;
    }
    if (dirrection < 0 && Throttle::get_signed_can_speed(dirrection) - target_speed < -2) {
        current_state = STOPPING;
    }
    if (Throttle::get_can_speed() < 2 && target_speed == 0) {
        current_state = PARKING;
    }
}

int CruiseControl::calculate_rpm(int gear, float speed) {
    int calculated_rpm = gear_coefficients[gear - 1] * gearbox_coefficient * abs(speed);
    if (calculated_rpm < 800) calculated_rpm = 800;
    return calculated_rpm;
}

void CruiseControl::cruise_update_task(void *args) {
    while (true) {
        vTaskDelay(10 / portTICK_PERIOD_MS);
        
        if (!enabled)
            continue;

        switch (current_state) {
        case PARKING:
            current_clutch_state = Clutch::Press;
            target_brake = 0.4;
            target_gear = 0;
            target_rpm = 1000;
            dirrection = 0;
            is_starting = true;
            if (target_speed > 0) {
                current_state = CLUTCHPUSH;
                dirrection = 1;
                next_gear = 1;
            }
            if (target_speed < 0) {
                current_state = CLUTCHPUSH;
                dirrection = -1;
                next_gear = 6;
            }
            ESP_LOGI(TAG, "parking: c_gear %d t_rpm %d c_rpm %d t_speed %d c_speed %d", Gearbox::get_current_gear(), target_rpm, (int)Throttle::get_can_rpm(), (int)target_speed, (int)Throttle::get_can_speed());
            break;
        case CLUTCHPUSH:
            current_clutch_state = Clutch::Press;
            if (is_starting) {
                target_brake = 0.4;
            }
            else {
                target_brake = 0;
            }
            target_rpm = 1000;
            if (Clutch::isUnengage()) {
                current_state = GEAR;
                target_gear = next_gear;
            }
            check_stop_park();
            ESP_LOGI(TAG, "clutchpush: c_gear %d t_gear %d t_rpm %d c_rpm %d t_speed %d c_speed %d", Gearbox::get_current_gear(), target_gear, target_rpm, (int)Throttle::get_can_rpm(), (int)target_speed, (int)Throttle::get_can_speed());
            break;
        case GEAR:
            current_clutch_state = Clutch::Press;
            if (is_starting) {
                target_brake = 0.4;
            }
            else {
                target_brake = 0;
            }
            target_rpm = 1000;

            // Нужно доделать блок переключения передач
            if (Gearbox::get_current_gear() == target_gear) {
                current_state = RPM;
            }
            check_stop_park();
            ESP_LOGI(TAG, "gear: c_gear %d t_gear %d t_rpm %d c_rpm %d t_speed %d c_speed %d", Gearbox::get_current_gear(), target_gear, target_rpm, (int)Throttle::get_can_rpm(), (int)target_speed, (int)Throttle::get_can_speed());
            break;
        case RPM:
            current_clutch_state = Clutch::Press;
            if (is_starting) {
                target_brake = 0.4;
            }
            else {
                target_brake = 0;
            }
            target_rpm = calculate_rpm(target_gear, Throttle::get_can_speed());
            if (target_rpm < 800) {
                target_rpm = 800;
            }
            if (abs(Throttle::get_can_rpm() - target_rpm) < 500) {
                current_state = RELEASE;
            }
            check_stop_park();
            ESP_LOGI(TAG, "rpm: t_rpm %d c_rpm %d dif %ld", target_rpm, (int)Throttle::get_can_rpm(), abs(Throttle::get_can_rpm() - target_rpm));
            break;
        case RELEASE:
            current_clutch_state = Clutch::Release;
            if (is_starting && Clutch::isUnengage()) {
                target_brake = 0;    
            }
            else {
                is_starting = false;
                target_brake = 0;
            }
            target_rpm = calculate_rpm(target_gear, Throttle::get_can_speed());
            if (target_rpm < 800) {
                target_rpm = 800;
            }
            if (Clutch::isEngage()) {
                current_state = DRIVE;
            }
            if (Throttle::get_can_speed() > abs(target_speed) && abs(target_speed) <= 7) {
                current_state = CLUTCHHOLD;
            }
            check_stop_park();
            ESP_LOGI(TAG, "release: t_rpm %d c_rpm %d", target_rpm, (int)Throttle::get_can_rpm());
            break;
        case CLUTCHHOLD:
            current_clutch_state = Clutch::SlowPress;
            target_brake = 0.2;
            target_rpm = calculate_rpm(target_gear, Throttle::get_can_speed());
            if (target_rpm < 800) {
                target_rpm = 800;
            }
            if (abs(target_speed) - Throttle::get_can_speed() > 2) {
                current_state = RELEASE;
            }
            check_stop_park();
            ESP_LOGI(TAG, "clutchhold: t_rpm %d c_rpm %d", target_rpm, (int)Throttle::get_can_rpm());
            break;
        case DRIVE:
            current_clutch_state = Clutch::Release;
            target_brake = 0;

            target_rpm = calculate_rpm(target_gear, target_speed);
            
            if (Throttle::get_can_rpm() < 1000 && target_gear > 1 && target_gear <= 5) {
                current_state = CLUTCHPUSH;
                next_gear = target_gear - 1;
            }
            if (Throttle::get_can_rpm() > 2000 && target_gear < 5) {
                current_state = CLUTCHPUSH;
                next_gear = target_gear + 1;
            }
            check_stop_park();
            ESP_LOGI(TAG, "drive: t_rpm %d c_rpm %d c_speed %ld", target_rpm, (int)Throttle::get_can_rpm(), Throttle::get_can_speed());
            break;
        case STOPPING:
            current_clutch_state = Clutch::SlowPress;
            target_brake = 0.2;
            target_rpm = calculate_rpm(target_gear, target_speed);
            if (abs(target_speed) - Throttle::get_can_speed() > 2) {
                current_state = RELEASE;
            }
            if (Throttle::get_can_speed() < 2 && target_speed == 0) {
                current_state = PARKING;
            }
            ESP_LOGI(TAG, "stopping: c_gear %d t_rpm %d c_rpm %d t_speed %d c_speed %d dif %f", Gearbox::get_current_gear(), target_rpm, (int)Throttle::get_can_rpm(), (int)target_speed, (int)Throttle::get_can_speed(), (float)(Throttle::get_can_speed() - target_speed));
            break;
        }

        // ESP_LOGW(TAG, "cur %d tar %d", Gearbox::get_current_gear(), target_gear);

        Gearbox::set_target_gear(target_gear);
        Clutch::setState(current_clutch_state);
        Brake::setBrakeTorque(target_brake);
        Throttle::set_target_value(target_rpm);
    } 
}

void CruiseControl::set_target_speed(float speed) {
    target_speed = speed;
}

void CruiseControl::init() {
    gear_coefficients[0] = 3.786;
    gear_coefficients[1] = 2.188;
    gear_coefficients[2] = 1.304;
    gear_coefficients[3] = 1.0;
    gear_coefficients[4] = 0.794;
    gear_coefficients[5] = 3.28;
    gearbox_coefficient = 30.9;

    current_state = PARKING;
    target_gear = 0;
    next_gear = 0;
    current_clutch_state = Clutch::Press;
    target_brake = 1;
    target_rpm = 1000;
    target_speed = 0;
    dirrection = 0;
    xTaskCreate(cruise_update_task, "cruise_update", 8192, NULL, 5, NULL);
}