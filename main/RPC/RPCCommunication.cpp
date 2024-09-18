#include "esp_log.h"
#include "Gearbox.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "RPCClient.h"
#include "RPCCommunication.h"
#include "Signals.h"
#include "Throttle.h"
#include "Wheel.h"
#include "Buttons.h"
#include "LMP.h"

static const char *TAG = "RPCCOMMUNICATION";

void RPCCommunication::set_accelerator_torque(int torque) { accelerator = torque; }
void RPCCommunication::set_accelerator_relay(int state) { accelerator_relay = state; }

void RPCCommunication::set_gearbox_state(int32_t is_ready, int32_t gear) {
    // ESP_LOGI(TAG, "set_gearbox_state %d %d", (int) is_ready, (int)gear);
    Gearbox::set_ready_state(is_ready);
    Gearbox::set_current_gear(gear);
}

void RPCCommunication::set_rpm_speed(int32_t rpm, int32_t speed) {
    Throttle::set_can_rpm(rpm);
    Throttle::set_can_speed(speed);
}

void RPCCommunication::rpc_communication_task(void *args) {
    struct sockaddr_in dest_addr_gearbox;
    dest_addr_gearbox.sin_addr.s_addr = inet_addr("192.168.1.103");
    dest_addr_gearbox.sin_family = AF_INET;
    dest_addr_gearbox.sin_port = htons(90);

    struct sockaddr_in dest_addr_wheel;
    dest_addr_wheel.sin_addr.s_addr = inet_addr("192.168.1.105");
    dest_addr_wheel.sin_family = AF_INET;
    dest_addr_wheel.sin_port = htons(90);

    struct sockaddr_in dest_addr_control_panel;
    dest_addr_control_panel.sin_addr.s_addr = inet_addr("192.168.1.106");
    dest_addr_control_panel.sin_family = AF_INET;
    dest_addr_control_panel.sin_port = htons(90);

    struct sockaddr_in dest_addr_relay;
    dest_addr_relay.sin_addr.s_addr = inet_addr("192.168.1.104");
    dest_addr_relay.sin_family = AF_INET;
    dest_addr_relay.sin_port = htons(90);

    while (true) {
        if (Buttons::get_button_state(Buttons::BUTTONS::GEARBOX)) {
            RPCClient::send_request("set_gear", Gearbox::get_target_gear(), dest_addr_gearbox);
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }

        RPCClient::send_request("angle_set", Wheel::get_target_angle(), dest_addr_wheel);
        vTaskDelay(10 / portTICK_PERIOD_MS);

        RPCClient::send_request("play_sound", 0, dest_addr_wheel);
        vTaskDelay(10 / portTICK_PERIOD_MS);

        RPCClient::send_request("throttle_set", Throttle::get_target_value(), Throttle::getRelay(), dest_addr_wheel);
        vTaskDelay(10 / portTICK_PERIOD_MS);

        RPCClient::send_request("set_light", StateMachine::get_light_id(), dest_addr_control_panel);
        vTaskDelay(10 / portTICK_PERIOD_MS);

        RPCClient::send_request("set_values", Signals::get_signals_bitmask(), dest_addr_relay);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
