#include "deva.h"

// Macros to make initialization easier
#define MakeGpio(pwr, gnd, dp, dn) \
    (DevaGPIO){ .pin_power = (pwr), .pin_ground = (gnd), .pin_data_p = (dp), .pin_data_n = (dn) }

#define MakePort(type, id, stream, connected, gpio_struct) \
    (DevaPort){ .type = (type), .moduleID = (id), .isStream = (stream), .connected = (connected), .gpio = (gpio_struct) }

static DevaPort ports[MAX_DEVA_PORTS] = {
    MakePort(DEVA_TYPE_EMPTY, DEVA_EMPTY_ID, false, false, MakeGpio(-1, -1, -1, -1)),
    MakePort(DEVA_TYPE_EMPTY, DEVA_EMPTY_ID, false, false, MakeGpio(-1, -1, -1, -1)),
    MakePort(DEVA_TYPE_EMPTY, DEVA_EMPTY_ID, false, false, MakeGpio(-1, -1, -1, -1)),
    MakePort(DEVA_TYPE_EMPTY, DEVA_EMPTY_ID, false, false, MakeGpio(-1, -1, -1, -1))
};
