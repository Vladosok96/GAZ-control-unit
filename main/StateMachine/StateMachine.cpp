#include "esp_log.h"
#include "Estop.h"
#include "FlySkyIBus.h"
#include "StateMachine.h"

static const char* TAG = "STATEMACHINE";

void StateMachine::init() { current_control_mode = EStop; }

StateMachine::Mode StateMachine::get_current_control_mode() { return current_control_mode; }

void StateMachine::set_control_mode(Mode mode) {
    if (current_control_mode == mode)
        return;

    ESP_LOGI(TAG, "Changing mode %s to %s", mode_to_string(current_control_mode), mode_to_string(mode));
    current_control_mode = mode;
}

void StateMachine::set_pause_state(std::string software_state) {
    if (current_control_mode == StateMachine::Mode::Auto && software_state == "Pause") {
        ESP_LOGI(TAG, "Changing mode Auto to Pause");
        current_control_mode = StateMachine::Mode::Pause;
    }
}

const char* StateMachine::mode_to_string(Mode mode) {
    switch (mode) {
        case Mode::EStop:
            return "E-Stop";
        case Mode::Manual:
            return "Manual";
        case Mode::Auto:
            return "Auto";
        case Mode::Pause:
            return "Pause";
        case Mode::Disable:
            return "Disabled";
        default:
            return "";
    }
}

int32_t StateMachine::get_light_id() {
    switch (current_control_mode) {
        case EStop:
        case Disable:
            return 0;
            break;
        case Pause:
            return 1;
            break;
        case Auto:
        case Manual:
            return 2;
            break;
        default:
            return 3;
    }
}
