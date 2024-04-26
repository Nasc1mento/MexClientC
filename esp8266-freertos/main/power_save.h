#ifndef POWER_SAVE_H
#define POWER_SAVE_H

#include "esp_sleep.h"

void deep_sleep(uint8_t seconds) {
    esp_deep_sleep_set_rf_option(2); //
	esp_deep_sleep(seconds*1000000);	
}

#endif // POWER_SAVE