#pragma once

class RPCCommunication {
public:
    static void set_accelerator_torque(int torque);
    static void set_accelerator_relay(int state);
    static void set_gearbox_state(int32_t is_ready, int32_t gear);
    static void set_rpm_speed(int32_t rpm, int32_t speed);
    static void rpc_communication_task(void *args);
    static void lmp_communication_task(void *args);
private:
    static inline int accelerator = 0;
    static inline int accelerator_relay = 0;
};
