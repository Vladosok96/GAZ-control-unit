#include "LMP.h"
#include "esp_log.h"

static const char *TAG = "LMP";

char* LMP::get_lmp_data() {
    char transmission_state = 'P';

    switch (Gearbox::get_current_gear())
    {
    case 0:
        transmission_state = 'N';
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        transmission_state = 'D';
        break;
    case 6:
        transmission_state = 'R';
        break;
    }

    sprintf(
        buffer,
        "\"TransmissionState\":\"%c\","
        "\"SteeringRotation\":\"%ld\","
        "\"RequestedAcceleratorPower\":\"%d\","
        "\"RequestedBrakePower\":\"%d\","
        "\"Velocity\":\"%.1f\","
        "\"TurnSignalState\":\"%s\"",
        transmission_state, Wheel::get_target_angle(), int(Throttle::get_target_value() / 30), int(Brake::get_brake_torque() * 50), Throttle::get_can_speed() / 3.6, Signals::get_turns()
    );

    return buffer;
}