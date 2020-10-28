#include <string>
#include <string.h>

#include "esp_system.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "esp_ota_ops.h"
#include "esp_https_ota.h"
#include "esp_http_client.h"

#include "Network/WifiStation.hpp"

#define CONFIG_EXAMPLE_FIRMWARE_UPGRADE_URL "https://192.168.1.20:8070/test.bin"

//Tag used for logging
static const char* TAG = "TemplateNode";

extern const uint8_t server_cert_pem_start[] asm("_binary_ca_cert_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_ca_cert_pem_end");

//Make app_main compatible with C++
extern "C"{ void app_main(void); }

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    }
    return ESP_OK;
}

void simple_ota_example_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting OTA example");

    esp_http_client_config_t config;

    config.url = CONFIG_EXAMPLE_FIRMWARE_UPGRADE_URL;
    config.cert_pem = (char *)server_cert_pem_start;
    config.event_handler = _http_event_handler;


    while (1) {
        esp_err_t ret = esp_https_ota(&config);
        if (ret == ESP_OK) {
            ESP_LOGI(TAG,"Firmware upgraded, restarting...");
            esp_restart();
        } else {
            ESP_LOGE(TAG, "Firmware upgrade failed");
        }

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    ESP_LOGD(TAG, "main starting...123");
    esp_log_level_set("*", ESP_LOG_DEBUG);   

    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    WifiStation wifiStation;
    wifiStation.start();
    wifiStation.isConnected(true);

    //vTaskDelay(pdMS_TO_TICKS(3000));

    //Mqtt mqttClient2;
    //mqttClient2.init("mqtt://192.168.123.124", "", "");

    // ESP_LOGI(TAG, "Test...");
    // try{
    //   WiFi &wifi = WiFi::getInstance();
    //   wifi.startWifi(WIFI_MODE_STA, DEFAULT_SSID, DEFAULT_PWD);
    //   ESP_LOGI(TAG, "waiting for sta connection...");
    //   wifi.waitStationConnected();
    //   ESP_LOGI(TAG, "sta connected, disconnecting...");
    // }catch(WiFiException &e){
    //   ESP_LOGW(TAG, "%s", e.what());
    // }

    xTaskCreate(&simple_ota_example_task, "ota_example_task", 8192, NULL, 5, NULL);
    
    while(1) { vTaskDelay(1000 / portTICK_PERIOD_MS); }
}
