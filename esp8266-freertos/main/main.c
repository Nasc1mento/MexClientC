#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_system.h>
#include <esp_log.h>
#include <esp_netif.h>
#include <esp_event.h>
#include <esp_wifi.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <esp_sntp.h>

#include "proxy.h"
#include "power_save.h"



#define APP_WIFI_SSID      CONFIG_WIFI_SSID
#define APP_WIFI_PASS      CONFIG_WIFI_PASSWORD
#define APP_MAXIMUM_RETRY  CONFIG_MAXIMUM_RETRY

#define APP_BROKER_HOST    CONFIG_BROKER_HOST
#define APP_BROKER_PORT    CONFIG_BROKER_PORT

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

#define SNTP_SERVER_NAME   "a.st1.ntp.br"

static EventGroupHandle_t xEventBits;
struct mex_client mc;

static const char *TAG = "application using mex";

static int s_retry_num = 0;

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
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
    mc = mex_connect(APP_BROKER_HOST, 60000);

    if (mc.st == CONNECTED) {
        ESP_LOGI(TAG, "Connected to broker");
    } else {
        ESP_LOGI(TAG, "Failed to connect to broker");
        return;
    }

    time_t current_time;
    struct tm local_time;
    char datetime_str[20];

    char topic[] = "water-level";

    const char msg_template[] = "{'distance': 83, 'battery': 37, 'timestamp': '%s'}";
    //  const char msg_template[] = "{'distance': %d, 'battery': %d, 'timestamp': '%s'}";
    char msg[sizeof(msg_template) + sizeof(datetime_str)];

    while (mc.st == CONNECTED) {
        time(&current_time);

        setenv("TZ", "BRT+3", 1);
        tzset();
        
        localtime_r(&current_time, &local_time);
        strftime(datetime_str, sizeof(datetime_str), "%Y-%m-%dT%H:%M:%SZ", &local_time);


        // unsigned short int distance = rand() % 101;
        // unsigned short int battery = rand() % 101;

        sprintf(msg, msg_template, datetime_str);
        // sprintf(msg, msg_template, distance, battery, datetime_str);
        publish(&mc, topic, msg);

        ESP_LOGI(TAG, "Message sent: %s to topic: %s", msg, topic);

        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }    
}

void sntp_task() {
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, SNTP_SERVER_NAME);
    sntp_init();

     while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET) { 
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    ESP_LOGI(TAG, "Time synchronized");
}

void sleep_task() {
    deep_sleep(10);
}


void app_main()
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");

    wifi_init_sta();
    sntp_task();    
    mex_task(); 
    // sleep_task();   
}