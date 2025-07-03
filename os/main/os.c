#include <stdio.h>
#include "app.h"
#include "Registery.h"
#include "deva.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void deva_task(void* param) {
    deva_init_ports();

    while (1) {
        deva_poll_all_ports();
        vTaskDelay(pdMS_TO_TICKS(1000));  // Poll every second
    }
}


void change_setting(const char* temp){
  
}

void setUpMainApps(void){
  App_Register("clock", NULL, NULL);
  App_Register("timer", NULL, NULL);
  App_Register("settings", NULL, NULL);
}
void setUpMainSettings(void){
  registry_register_float("brightness", 1.0f, change_setting);
}

void app_main(void)
{
  registry_init();
  setUpMainSettings();
  setUpMainApps();
  deva_init_ports();
   xTaskCreate(deva_task,         // Task function
                "deva_task",       // Task name
                4096,              // Stack size (bytes)
                NULL,              // Parameter
                5,                 // Priority
                NULL);             // Task handle (optional)

}

