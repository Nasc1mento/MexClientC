#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "proxy.h"
#include "power_save.h"

#define WIFI_TASK_0_BIT                ( 1 << 0 )
#define MEX_TASK_1_BIT                 ( 1 << 1 )
#define DEEP_SLEEP_TASK_2_BIT          ( 1 << 2 )

#define ALL_SYNC_BITS ( WIFI_TASK_0_BIT | MEX_TASK_1_BIT | DEEP_SLEEP_TASK_2_BIT )

#define APP_WIFI_SSID      CONFIG_WIFI_SSID
#define APP_WIFI_PASS      CONFIG_WIFI_PASSWORD
#define APP_MAXIMUM_RETRY  CONFIG_MAXIMUM_RETRY

#define APP_BROKER_HOST    CONFIG_BROKER_HOST
#define APP_BROKER_PORT    CONFIG_BROKER_PORT

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

#define PUB_SUCCESS        BIT2

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
        ESP_LOGI(TAG, "got ip:%s",
                 ip4addr_ntoa(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(xEventBits, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta(void *pv)
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
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 APP_WIFI_SSID, APP_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 APP_WIFI_SSID, APP_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler));
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler));
    // vEventGroupDelete(xEventBits);

}

void mex_task(void *pv) {

    
    static int ret = -1;

    mc = mex_connect(APP_BROKER_HOST, 60000);
    publish(&mc, "test", "hello world");
    
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    xEventGroupSetBits(xEventBits, PUB_SUCCESS);
    
}

void sleep_task(void *pv) {
   
    EventBits_t bits = xEventGroupWaitBits(xEventBits,
            PUB_SUCCESS,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    if (bits & PUB_SUCCESS) {
        esp_deep_sleep_set_rf_option(2);
	    esp_deep_sleep(5*1000000);	
    }
}


void app_main()
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta(NULL);
    mex_task(NULL);
    sleep_task(NULL);
}