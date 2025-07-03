#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "driver/gpio.h"

#define DEVA_EMPTY_ID      0x000000
#define DEVA_SPEAKER_ID    0x000001
#define DEVA_MICROPHONE_ID 0x000002
#define DEVA_STORAGE_ID    0x000003

typedef enum {
    DEVA_TYPE_EMPTY,
    DEVA_TYPE_SPEAKER,
    DEVA_TYPE_MICROPHONE,
    DEVA_TYPE_STORAGE
} DevaType;

typedef struct {
    int pin_power;
    int pin_ground;
    int pin_data_p;
    int pin_data_n;
} DevaGPIO;

typedef struct {
    DevaType type;
    uint32_t moduleID;
    bool isStream;
    bool connected;
    DevaGPIO gpio;
} DevaPort;

#define MAX_DEVA_PORTS 4

const DevaPort* deva_get_ports(void);
DevaPort* deva_get_port(int index);
int deva_find_port_by_id(uint32_t moduleID);
int deva_send_command(int port, const char* command, char* returndata);

void deva_poll_all_ports(void);
void deva_init_ports(void);
