#include "Buttons.h"
#include "esp_log.h"
#include "esp_timer.h"

static const char *TAG = "BUTTONS";

bool Buttons::get_button_state(BUTTONS button_index) {
    return (buttons_state >> button_index) & 1;
}

void Buttons::set_buttons_state(int32_t state) {
    *(int32_t*)(&buttons_state) = state;
}

bool Buttons::get_remote_button_state(REMOTE_BUTTONS button_index) {
    return (remote_buttons_state >> button_index) & 1;
}

void Buttons::set_remote_buttons_state(int32_t state) {
    *(int32_t*)(&remote_buttons_state) = state;
    remote_last_received = esp_timer_get_time();
}

bool Buttons::remote_connected() {
    return esp_timer_get_time() - remote_last_received < 1000000;
}
