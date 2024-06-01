#include "nvs_handler.h"
#include <esp_log.h>
#include <esp_err.h>
#include <esp_sleep.h>



uint8_t nvs_write(const char* key, const char* value){
    esp_err_t err;
    nvs_handle_t my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return 1;
    }
    err = nvs_set_str(my_handle, key, value);
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "Error (%s) writing value!", esp_err_to_name(err));
        return 1;
    }
    err = nvs_commit(my_handle);
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "Error (%s) committing updates!", esp_err_to_name(err));
        return 1;
    }
    nvs_close(my_handle);
    return 0;
}

uint8_t nvs_read(const char* key, char* value){
    esp_err_t err;
    nvs_handle_t my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return 1;
    }
    size_t required_size;
    err = nvs_get_str(my_handle, key, NULL, &required_size);
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "Error (%s) reading value size!", esp_err_to_name(err));
        return 1;
    }
    err = nvs_get_str(my_handle, key, value, &required_size);
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "Error (%s) reading value!", esp_err_to_name(err));
        return 1;
    }
    nvs_close(my_handle);
    return 0;
}

uint8_t nvs_erase(const char* key){
    esp_err_t err;
    nvs_handle_t my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return 1;
    }
    err = nvs_erase_key(my_handle, key);
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "Error (%s) erasing key!", esp_err_to_name(err));
        return 1;
    }
    err = nvs_commit(my_handle);
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "Error (%s) committing updates!", esp_err_to_name(err));
        return 1;
    }
    nvs_close(my_handle);
    return 0;
}