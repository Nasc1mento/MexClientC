#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

#include <esp_system.h>
#include <esp_log.h>
#include <esp_netif.h>
#include <esp_event.h>
#include <esp_sleep.h>
#include <esp_sntp.h>
#include <esp_wifi.h>

#include <nvs.h>
#include <nvs_flash.h>

#include "mex.h"

#define RANDOM
#define POWER_SAVE

// #define SNTP
// #define ERASE_NVS

#define APP_WIFI_SSID      CONFIG_WIFI_SSID
#define APP_WIFI_PASS      CONFIG_WIFI_PASSWORD
#define APP_MAXIMUM_RETRY  CONFIG_MAXIMUM_RETRY

#define APP_BROKER_HOST    CONFIG_BROKER_HOST
#define APP_BROKER_PORT    CONFIG_BROKER_PORT

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

#define SNTP_SERVER_NAME   "br.pool.ntp.org"


static EventGroupHandle_t xEventBits;
struct mex_client mc;


static const char *TAG = "application using mex";

static int s_retry_num = 0;

clock_t start, end;
double cpu_time_used;

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < APP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(xEventBits, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:%s", ip4addr_ntoa(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(xEventBits, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta()
{
    xEventBits = xEventGroupCreate();

    tcpip_adapter_init();

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = APP_WIFI_SSID,
            .password = APP_WIFI_PASS
        },
    };

    if (strlen((char *)wifi_config.sta.password)) {
        wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");


    EventBits_t bits = xEventGroupWaitBits(xEventBits,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s", APP_WIFI_SSID, APP_WIFI_PASS);
                 
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s", APP_WIFI_SSID, APP_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler));
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler));
    
    vEventGroupDelete(xEventBits);
}



void mex_task() {
    mc = create_connection(APP_BROKER_HOST, 60000);

    if (mc.st == CONNECTED) {
        ESP_LOGI(TAG, "Connected to broker");
    } else {
        ESP_LOGI(TAG, "Failed to connect to broker");
        return;
    }

    time_t now;
    struct tm local_time;
    // char datetime_str[20];

    char topic[] = "water-level";
    const char msg_template[] = "{'distance': %d, 'battery': %d, 'timestamp': '%.2f'}";
    char msg[sizeof(msg_template) + sizeof(datetime_str)];

        
        #ifdef RANDOM
            unsigned short int distance = rand() % 101;
            unsigned short int battery = rand() % 101;
            sprintf(msg, msg_template, distance, battery, datetime_str);
        #else
            unsigned short int distance = 10;
            unsigned short int battery = 80;
            sprintf(msg, msg_template, distance, battery, datetime_str);
        #endif

        publish(&mc, topic, msg);
        
        ESP_LOGI(TAG, "Message sent: %s to topic: %s", msg, topic);

    return;
}






void set_time() {
    if (sntp_get_sync_status() != SNTP_SYNC_STATUS_RESET) {
        ESP_LOGI(TAG, "Time already synchronized");
        return;
    }

    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, SNTP_SERVER_NAME);
    sntp_init();

    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET) { 
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    setenv("TZ", "BRT+3", 1);
    tzset();

    ESP_LOGI(TAG, "Time synchronized");
}

void app_main()
{
    esp_err_t err = nvs_flash_init();

    #ifdef SNTP

    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    ESP_ERROR_CHECK( err ); 

    nvs_handle_t nvs_handle;
    err = nvs_open("storage", NVS_READWRITE, &nvs_handle);

    #ifdef ERASE_NVS
        nvs_flash_erase();
    #else // ERASE_NVS
    

    if (err != ESP_OK) {
        ESP_LOGI(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "NVS handle opened\n");


        err = nvs_get_i32(nvs_handle, "sleep_counter", &sleep_counter);

        switch (err) {
            case ESP_OK:
                ESP_LOGI(TAG, "Restart counter = %d\n", sleep_counter);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                ESP_LOGI(TAG, "sleep_counter is not initialized yet!\n");
                break;
            default :
                ESP_LOGI(TAG, "Error (%s) reading!\n", esp_err_to_name(err));
        } 
    }

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");

    wifi_init_sta();
    if (sleep_counter == 0) {
        set_time();
    }

    mex_task(); 
    #ifdef POWER_SAVE
        sleep_counter++;
        err = nvs_set_i32(nvs_handle, "sleep_counter", sleep_counter);
        if (err != ESP_OK) {
            ESP_LOGI(TAG, "Error (%s) writing!\n", esp_err_to_name(err));
        } else {
            ESP_LOGI(TAG, "sleep_counter updated\n");
        }

        err = nvs_commit(nvs_handle);
        if (err != ESP_OK) {
            ESP_LOGI(TAG, "Error (%s) committing!\n", esp_err_to_name(err));
        } else {
            ESP_LOGI(TAG, "Changes committed\n");
        }
        nvs_close(nvs_handle);
        deep_sleep(10);

    #endif // POWER_SAVE
    #endif // ERASE_NVS
    #else

    ESP_ERROR_CHECK( err );
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");

    wifi_init_sta();
    mex_task();
    #ifdef POWER_SAVE
        deep_sleep(10);
    #endif



    #endif
} 