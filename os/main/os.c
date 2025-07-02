#include <stdio.h>
#include "app.h"
#include "Registery.h"



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

}

