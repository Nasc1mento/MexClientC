#ifndef POWER_SAVE_H
#define POWER_SAVE_H

#include "esp_sleep.h"

void deep_sleep(unsigned int seconds) {

    uint64_t time_in_us = seconds * 1000000;

    esp_deep_sleep_set_rf_option(2);
	esp_deep_sleep(time_in_us);	
}

#endif // POWER_SAVE