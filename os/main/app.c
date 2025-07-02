#include "app.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h> // optional

static App* appRegistryHead = NULL;

void App_Register(char* name, void (*app_main)(void), char icon[ICON_SIZE]) {
    App* newApp = (App*)malloc(sizeof(App));
    if (!newApp) return;

    newApp->name = strdup(name);
    if (!newApp->name) {
        free(newApp);
        return;
    }

    newApp->app_main = app_main;
    memcpy(newApp->icon, icon, ICON_SIZE);
    newApp->next = appRegistryHead;
    appRegistryHead = newApp;
}

void App_Unregister(const char* name) {
    App* current = appRegistryHead;
    App* previous = NULL;

    while (current) {
        if (strcmp(current->name, name) == 0) {
            if (previous) {
                previous->next = current->next;
            } else {
                appRegistryHead = current->next;
            }

            free(current->name);
            free(current);
            return;
        }

        previous = current;
        current = current->next;
    }
}


App* App_Find(const char* name) {
    App* current = appRegistryHead;
    while (current) {
        if (strcmp(current->name, name) == 0) return current;
        current = current->next;
    }
    return NULL;
}

void App_Run(const char* name) {
    App* app = App_Find(name);
    if (app && app->app_main) {
        app->app_main();
    }
}

void App_ClearAll(void) {
    App* current = appRegistryHead;
    while (current) {
        App* next = current->next;
        free(current->name);
        free(current);
        current = next;
    }
    appRegistryHead = NULL;
}

App* App_List(void) {
    return appRegistryHead;
}

int App_Count(void) {
    int count = 0;
    App* current = appRegistryHead;
    while (current) {
        count++;
        current = current->next;
    }
    return count;
}
