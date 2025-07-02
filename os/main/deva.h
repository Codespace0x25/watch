#pragma once
#include <stdint.h>
#include <stdbool.h>

// Deva IDs
#define DEVA_EMPTY_ID      0x000000
#define DEVA_SPEAKER_ID    0x000001
#define DEVA_MICROPHONE_ID 0x000002
#define DEVA_STORAGE_ID    0x000003

// Deva module type enum
typedef enum {
    DEVA_TYPE_EMPTY,
    DEVA_TYPE_SPEAKER,
    DEVA_TYPE_MICROPHONE,
    DEVA_TYPE_STORAGE
} DevaType;

// GPIO info
typedef struct {
    int pin_power;
    int pin_ground;
    int pin_data_p;
    int pin_data_n;
} DevaGPIO;

// Main port structure
typedef struct {
    DevaType type;
    uint32_t moduleID;
    bool isStream;
    bool connected;
    DevaGPIO gpio;
} DevaPort;

// Max ports if needed
#define MAX_DEVA_PORTS 4
