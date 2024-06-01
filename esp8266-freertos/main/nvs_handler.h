#include <nvs_flash.h>
uint8_t nvs_write(const char* key, const char* value);
uint8_t nvs_read(const char* key, char* value);
uint8_t nvs_erase(const char* key);


