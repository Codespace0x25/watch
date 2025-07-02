#pragma once

#define ICON_SIZE (8 * 8)

typedef struct App {
    char* name;
    void (*app_main)(void);
    char icon[ICON_SIZE];
    struct App* next;
} App;

// Register a new app
void App_Register(char* name, void (*app_main)(void), char icon[ICON_SIZE]);

// Unregister an app by name
void App_Unregister(const char* name);

// Find an app by name
App* App_Find(const char* name);

// Run an app by name
void App_Run(const char* name);

// Clear all registered apps
void App_ClearAll(void);

// Get the head of the list (for iteration)
App* App_List(void);

// Count all registered apps
int App_Count(void);
