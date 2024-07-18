#pragma once

class Gearbox {
public:
    static void set_target_gear(int gear);
    static int get_target_gear();
    static void set_current_gear(int gear);
    static int get_current_gear();
    static void set_ready_state(bool state);
    static bool get_ready_state();
private:
    static inline int target_gear = 0;
    static inline int current_gear = 0;
    static inline bool is_ready = false;
    
};
