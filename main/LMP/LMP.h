#pragma once

#include "Gearbox.h"
#include "Wheel.h"
#include "Throttle.h"
#include "Brake.h"
#include "Signals.h"

class LMP {
public:
    static char* get_lmp_data();
    static inline char buffer[512];
};
