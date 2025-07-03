#include "deva.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BIT_DELAY_US 104 // ~9600 baud
#define HANDSHAKE_TIMEOUT_US 200000
#define DEVA_RESPONSE_MAX 32

static DevaPort ports[MAX_DEVA_PORTS] = {
    { .type = DEVA_TYPE_EMPTY, .moduleID = DEVA_EMPTY_ID, .connected = false, .gpio = {18, 19, 21, 22} },
    { .type = DEVA_TYPE_EMPTY, .moduleID = DEVA_EMPTY_ID, .connected = false, .gpio = {23, 25, 26, 27} },
    { .type = DEVA_TYPE_EMPTY, .moduleID = DEVA_EMPTY_ID, .connected = false, .gpio = {32, 33, 34, 35} },
    { .type = DEVA_TYPE_EMPTY, .moduleID = DEVA_EMPTY_ID, .connected = false, .gpio = {13, 14, 15, 16} },
};

const DevaPort* deva_get_ports(void) { return ports; }
DevaPort* deva_get_port(int index) {
    if (index < 0 || index >= MAX_DEVA_PORTS) return NULL;
    return &ports[index];
}
int deva_find_port_by_id(uint32_t moduleID) {
    for (int i = 0; i < MAX_DEVA_PORTS; ++i)
        if (ports[i].connected && ports[i].moduleID == moduleID) return i;
    return -1;
}

// === GPIO TRANSMISSION ===
static void send_byte(int pin, uint8_t byte) {
    gpio_set_level(pin, 0); usleep(BIT_DELAY_US); // Start
    for (int i = 0; i < 8; i++) {
        gpio_set_level(pin, (byte >> i) & 1);
        usleep(BIT_DELAY_US);
    }
    gpio_set_level(pin, 1); usleep(BIT_DELAY_US); // Stop
}
static void send_string(int pin, const char* str) {
    gpio_set_level(pin, 1); usleep(BIT_DELAY_US);
    for (int i = 0; str[i]; i++) send_byte(pin, str[i]);
    send_byte(pin, '\n');
}

// === SEND COMMAND ===
int deva_send_command(int port, const char* command, char* returndata) {
    if (port < 0 || port >= MAX_DEVA_PORTS) return -1;
    DevaPort* p = &ports[port];
    if (!p->connected || p->gpio.pin_data_p < 0) return -2;

    gpio_set_direction(p->gpio.pin_data_p, GPIO_MODE_OUTPUT);
    send_string(p->gpio.pin_data_p, command);

    if (returndata)
        snprintf(returndata, 64, "sent: %s", command);

    return 0;
}

// === CONNECTION DETECTION ===
static bool detect_connection(int i) {
    DevaPort* p = &ports[i];
    int dp = p->gpio.pin_data_p;
    int dn = p->gpio.pin_data_n;

    gpio_set_direction(dp, GPIO_MODE_INPUT);
    gpio_set_pull_mode(dp, GPIO_PULLDOWN_ONLY);
    gpio_set_direction(dn, GPIO_MODE_OUTPUT);
    gpio_set_level(dn, 0);

    int64_t start = esp_timer_get_time();
    while (gpio_get_level(dp) == 0) {
        if (esp_timer_get_time() - start > HANDSHAKE_TIMEOUT_US) return false;
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    gpio_set_level(dn, 1); usleep(10000);

    char id_str[DEVA_RESPONSE_MAX] = {0};
    int pos = 0, bit;
    uint8_t b;

    while (pos < DEVA_RESPONSE_MAX - 1) {
        while (gpio_get_level(dp) == 1);
        usleep(BIT_DELAY_US / 2);
        b = 0;
        for (int i = 0; i < 8; i++) {
            usleep(BIT_DELAY_US);
            bit = gpio_get_level(dp);
            b |= (bit << i);
        }
        usleep(BIT_DELAY_US);
        if (b == '\n') break;
        id_str[pos++] = (char)b;
    }
    id_str[pos] = '\0';
    gpio_set_level(dn, 0);

    if (strlen(id_str) == 0) return false;
    uint32_t id = strtoul(id_str, NULL, 10);
    p->connected = true;
    p->moduleID = id;
    p->type =
        (id == DEVA_SPEAKER_ID)    ? DEVA_TYPE_SPEAKER :
        (id == DEVA_MICROPHONE_ID) ? DEVA_TYPE_MICROPHONE :
        (id == DEVA_STORAGE_ID)    ? DEVA_TYPE_STORAGE : DEVA_TYPE_EMPTY;

    return true;
}

// === DISCONNECTION DETECTION ===
static bool detect_disconnection(int i) {
    DevaPort* p = &ports[i];
    int dp = p->gpio.pin_data_p;

    gpio_set_direction(dp, GPIO_MODE_INPUT);
    gpio_set_pull_mode(dp, GPIO_PULLDOWN_ONLY);

    // If line is LOW for 200ms, assume disconnect
    int64_t start = esp_timer_get_time();
    while (esp_timer_get_time() - start < HANDSHAKE_TIMEOUT_US) {
        if (gpio_get_level(dp) == 1) return false;
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    p->connected = false;
    p->type = DEVA_TYPE_EMPTY;
    p->moduleID = DEVA_EMPTY_ID;
    return true;
}

// === INIT & POLLING ===
void deva_init_ports(void) {
    for (int i = 0; i < MAX_DEVA_PORTS; ++i) {
        gpio_reset_pin(ports[i].gpio.pin_power);
        gpio_reset_pin(ports[i].gpio.pin_ground);
        gpio_reset_pin(ports[i].gpio.pin_data_p);
        gpio_reset_pin(ports[i].gpio.pin_data_n);

        gpio_set_direction(ports[i].gpio.pin_power, GPIO_MODE_OUTPUT);
        gpio_set_direction(ports[i].gpio.pin_ground, GPIO_MODE_OUTPUT);
        gpio_set_level(ports[i].gpio.pin_power, 1);
        gpio_set_level(ports[i].gpio.pin_ground, 0);
    }
}

void deva_poll_all_ports(void) {
    for (int i = 0; i < MAX_DEVA_PORTS; ++i) {
        if (!ports[i].connected) {
            if (detect_connection(i)) {
	      printf("[DEVA] Port %d connected (ID: %60X)\n", i, (int)ports[i].moduleID);
            }
        } else {
            if (detect_disconnection(i)) {
                printf("[DEVA] Port %d disconnected\n", i);
            }
        }
    }
}
