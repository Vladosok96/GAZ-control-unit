#include "Gearbox.h"

static const char *TAG = "GEARBOX";

void Gearbox::set_target_gear(int gear) { target_gear = gear; }
int Gearbox::get_target_gear() { return target_gear; }
void Gearbox::set_current_gear(int gear) { current_gear = gear; }
int Gearbox::get_current_gear() { return current_gear; }
void Gearbox::set_ready_state(bool state) { is_ready = state; }
bool Gearbox::get_ready_state() { return target_gear; }
