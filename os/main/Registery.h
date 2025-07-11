#pragma once
#include <stdbool.h>

typedef enum {
    Registry_Bool,
    Registry_Int,
    Registry_Float,
} Registry_Type;

typedef struct Registry {
    char* name;  // Key
    Registry_Type type;
    union {
        bool b;
        int i;
        float f;
    } value;
    void (*on_change)(const char* name); // Optional callback
    struct Registry* next;
} Registry;

void registry_init(void);
void registry_free(void);

bool registry_register_bool(const char* name, bool initial_value, void (*on_change)(const char*));
bool registry_register_int(const char* name, int initial_value, void (*on_change)(const char*));
bool registry_register_float(const char* name, float initial_value, void (*on_change)(const char*));

bool registry_set_bool(const char* name, bool value);
bool registry_set_int(const char* name, int value);
bool registry_set_float(const char* name, float value);

bool registry_get_bool(const char* name, bool* out);
bool registry_get_int(const char* name, int* out);
bool registry_get_float(const char* name, float* out);

Registry* registry_find(const char* name);
void registry_for_each(void (*callback)(Registry*));
