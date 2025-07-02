#include "Registery.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef bool reg_bool_t;
typedef int reg_int_t;
typedef float reg_float_t;

static Registry* registry_head = NULL;

void registry_init(void) {
    registry_head = NULL;
}

void registry_free(void) {
    Registry* current = registry_head;
    while (current) {
        Registry* next = current->next;
        free(current->name);
        free(current);
        current = next;
    }
    registry_head = NULL;
}

Registry* registry_find(const char* name) {
    Registry* current = registry_head;
    while (current) {
        if (strcmp(current->name, name) == 0)
            return current;
        current = current->next;
    }
    return NULL;
}

static bool internal_register(const char* name, Registry_Type type, void (*on_change)(const char*)) {
    if (registry_find(name)) return false;

    Registry* entry = calloc(1, sizeof(Registry));
    if (!entry) return false;

    entry->name = strdup(name);
    entry->type = type;
    entry->on_change = on_change;
    entry->next = registry_head;
    registry_head = entry;
    return true;
}

#define IMPL_REGISTER(NAME, TYPE, FIELD, ENUM_TAG)                                \
bool registry_register_##NAME(const char* name, TYPE initial, void (*on_change)(const char*)) { \
    if (!internal_register(name, ENUM_TAG, on_change)) return false;              \
    Registry* r = registry_find(name);                                            \
    if (!r) return false;                                                         \
    r->value.FIELD = initial;                                                     \
    return true;                                                                  \
}

#define IMPL_SET(NAME, TYPE, FIELD, ENUM_TAG)                                     \
bool registry_set_##NAME(const char* name, TYPE val) {                            \
    Registry* r = registry_find(name);                                            \
    if (!r || r->type != ENUM_TAG) return false;                                  \
    r->value.FIELD = val;                                                         \
    if (r->on_change) r->on_change(name);                                         \
    return true;                                                                  \
}

#define IMPL_GET(NAME, TYPE, FIELD, ENUM_TAG)                                     \
bool registry_get_##NAME(const char* name, TYPE* out) {                           \
    Registry* r = registry_find(name);                                            \
    if (!r || r->type != ENUM_TAG || !out) return false;                          \
    *out = r->value.FIELD;                                                        \
    return true;                                                                  \
}

// Use macros to define all typed accessors
IMPL_REGISTER(bool, reg_bool_t, b, Registry_Bool)
IMPL_REGISTER(int,  reg_int_t,  i, Registry_Int)
IMPL_REGISTER(float, reg_float_t, f, Registry_Float)

IMPL_SET(bool, reg_bool_t, b, Registry_Bool)
IMPL_SET(int,  reg_int_t,  i, Registry_Int)
IMPL_SET(float, reg_float_t, f, Registry_Float)

IMPL_GET(bool, reg_bool_t, b, Registry_Bool)
IMPL_GET(int,  reg_int_t,  i, Registry_Int)
IMPL_GET(float, reg_float_t, f, Registry_Float)

void registry_for_each(void (*callback)(Registry*)) {
    Registry* current = registry_head;
    while (current) {
        callback(current);
        current = current->next;
    }
}
