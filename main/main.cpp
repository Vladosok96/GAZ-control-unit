#include "ADC.h"
#include "Brake.h"
#include "Clutch.h"
#include "driver/gpio.h"
#include "Engine.h"
#include "Estop.h"
#include <esp_check.h>
#include "esp_event.h"
#include <esp_log.h>
#include "esp_netif.h"
#include <esp_timer.h>
#include "Ethernet.h"
#include "FlySkyIBus.h"
#include <freertos/FreeRTOS.h>
#include "Gearbox.h"
#include "hardware.h"
#include "jsonrpc-lean/server.h"
#include "LCD.h"
#include "lowpass_filter.h"
#include "Motors.h"
#include "NVStorage.h"
#include "RPCCommunication.h"
#include "Signals.h"
#include "StateMachine.h"
#include "Throttle.h"
#include "Webserver.h"
#include "Buttons.h"
#include "Wheel.h"
#include "PCCommunication.h"
#include "udp_server.h"
#include "CruiseControl.h"
#include "RPCClient.h"

static const char *TAG = "MAIN";

LowPassFilter wheel_filter = LowPassFilter(0.4);

void remote_control_task(void *args) {
    while (true) {
        LCD::collectFaults(FlySkyIBus::available(), Buttons::remote_connected(), Estop::normal());
        
        if (Buttons::get_button_state(Buttons::AUTOPILOT)) {
            if (FlySkyIBus::available() && Estop::normal()) {
                LCD::resetFaults();
                if (FlySkyIBus::readChannel(8) < 1500) {
                    StateMachine::set_control_mode(StateMachine::Mode::Manual);
                } else {
                    StateMachine::set_control_mode(StateMachine::Mode::Auto);
                }
            } else {
                StateMachine::set_control_mode(StateMachine::Mode::EStop);
            }

            // if (Buttons::remote_connected() && FlySkyIBus::available() && Estop::normal()) {
            //     if (Buttons::get_remote_button_state(Buttons::STOP)) {
            //         StateMachine::set_control_mode(StateMachine::Mode::EStop);
            //     }
            //     else if (Buttons::get_remote_button_state(Buttons::START)) {
            //         LCD::resetFaults();
            //         if (FlySkyIBus::readChannel(8) < 1500) {
            //             StateMachine::set_control_mode(StateMachine::Mode::Manual);
            //         } else {
            //             StateMachine::set_control_mode(StateMachine::Mode::Auto);
            //         }
            //     }
            //     else if (Buttons::get_remote_button_state(Buttons::PAUSE)) {
            //         LCD::resetFaults();
            //         StateMachine::set_control_mode(StateMachine::Mode::Pause);
            //     }
            // } else {
            //     StateMachine::set_control_mode(StateMachine::Mode::EStop);
            // }
        } else {
            StateMachine::set_control_mode(StateMachine::Mode::Disable);
        }

        int dir_coefficient = 0;
        float speed_coefficient = 0;
        switch (StateMachine::get_current_control_mode()) {
        case StateMachine::Mode::Manual:
            Signals::set_sound_signal(FlySkyIBus::readChannel(9) > 1500);
            Wheel::set_target_angle((float)(FlySkyIBus::readChannel(0) - 1500) / 500);

            
            if (FlySkyIBus::readChannel(4) < 1300) {
                dir_coefficient = 0;
            }
            else if (FlySkyIBus::readChannel(4) < 1600) {
                dir_coefficient = 1;
            }
            else {
                dir_coefficient = -1;
            }
            speed_coefficient = (float)(FlySkyIBus::readChannel(2) - 1000) / 1000 * 20;
            if (speed_coefficient < 1) {
                speed_coefficient = 0;
            }
            CruiseControl::set_target_speed(speed_coefficient * dir_coefficient);

            Engine::set_state(FlySkyIBus::readChannel(5) > 1500);

            Throttle::enable();

            CruiseControl::enable();

            break;

        case StateMachine::Mode::Auto:
            Wheel::set_target_angle(wheel_filter(PCCommunication::get_target_wheel_angle()));
            CruiseControl::set_target_speed(PCCommunication::get_target_speed());
            // Engine::set_state(FlySkyIBus::readChannel(5) > 1500);
            Signals::set_sound_signal(FlySkyIBus::readChannel(9) < 1500);
            Engine::set_state(true);
            // Signals::set_sound_signal(true);
            CruiseControl::enable();
            Throttle::enable();
            break;

        case StateMachine::Mode::Pause:
            Signals::set_left_turn(false);
            Signals::set_right_turn(false);
            CruiseControl::enable();
            CruiseControl::set_target_speed(0.0);
            // CruiseControl::disable();
            // Brake::setBrakeTorque(0.4);
            Throttle::disable();
            // Clutch::setState(Clutch::ClutchState::Press);
            Wheel::enable();
            Engine::set_state(true);
            // Signals::set_sound_signal(FlySkyIBus::readChannel(9) > 1500);
            Signals::set_sound_signal(false);
            break;

        case StateMachine::Mode::Disable:
            Signals::set_left_turn(false);
            Signals::set_right_turn(false);
            CruiseControl::disable();
            Brake::setBrakeTorque(0);
            Throttle::disable();
            Clutch::setState(Clutch::ClutchState::Release);
            Wheel::disable();
            break;
        
        case StateMachine::Mode::EStop:
            // Signals::set_sound_signal(FlySkyIBus::readChannel(9) > 1500);
            Signals::set_sound_signal(false);
            Signals::set_left_turn(false);
            Signals::set_right_turn(false);
            CruiseControl::disable();
            Brake::setBrakeTorque(1);
            Clutch::setState(Clutch::ClutchState::Press);
            Wheel::disable();
            Throttle::disable();
            Engine::set_state(false);
            if (FlySkyIBus::readChannel(1) > 1800) {
                Estop::set_available();
            }
            break;
        
        default:
            break;
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

extern "C" void app_main(void) {

    Estop::init(PIN_ESTOP_BUTTON);
    FlySkyIBus::init();

    NVStorage::init();
    NVStorage::loadSystemParams();
    NVStorage::loadClutchMotorParams();
    NVStorage::loadBrakeMotorParams();

    set_clutch_motor_params();
    set_brake_motor_params();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    Ethernet::init(PIN_ETH_MISO, PIN_ETH_MOSI, PIN_ETH_SCK, PIN_ETH_CS, PIN_ETH_INT, SPI_BUS_ETH);

    Webserver::init();

    Wheel::init(PIN_WHEEL_EN);

    RPCClient::init();

    xTaskCreate(RPCCommunication::rpc_communication_task, "RPC TX", 8192, NULL, 5, NULL);
    xTaskCreate(RPCCommunication::lmp_communication_task, "LMP TX", 8192, NULL, 5, NULL);

    Engine::init(PIN_ENGINE_IGNITION, PIN_ENGINE_STARTER);

    ADC::init();

    motors_init();

    StateMachine::init();
    CruiseControl::init();

    LCD::init(GPIO_NUM_7, GPIO_NUM_15, GPIO_NUM_16, GPIO_NUM_6, GPIO_NUM_NC, true);

    clutch_motor.setControlMode(BrushedMotor::ControlMode::Position);

    Brake::setParams(NVStorage::brakeMotorParams.velLimit, NVStorage::brakeMotorParams.zeroPos);

    Clutch::setParams(NVStorage::clutchMotorParams.disabledPos, NVStorage::clutchMotorParams.zeroPos, NVStorage::clutchMotorParams.fullEngPos, NVStorage::clutchMotorParams.minEngPos, NVStorage::clutchMotorParams.pressedPos);
    Clutch::init();
    
    start_udp_server();

    xTaskCreate(remote_control_task, "RC TASK", 6144, NULL, 1, NULL);
}
