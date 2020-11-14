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

#include "SmartDevice/Kernel.hpp"
#include "SmartDevice/DeviceType.hpp"


void * operator new( size_t size )
{
    return pvPortMalloc( size );
}

void * operator new[]( size_t size )
{
    return pvPortMalloc(size);
}

void operator delete( void * ptr )
{
    vPortFree ( ptr );
}

void operator delete[]( void * ptr )
{
    vPortFree ( ptr );
}

//Tag used for logging
static const char* TAG = "TemplateNode";

//Make app_main compatible with C++
extern "C"{ void app_main(void); }

void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_DEBUG);

    ESP_LOGD(TAG, "main starting...123");
    esp_log_level_set("*", ESP_LOG_DEBUG);   

    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    auto &kernel = SmartDevice::Kernel::getInstance();
    kernel.getDeviceInfo().deviceType = SmartDevice::DeviceType::Template;
    kernel.getOtaTask().setBinaryName("test123.bin");
    kernel.start();

    //SmartDevice::NetworkTask networkTask;
    //networkTask.start();

    // WifiStation wifiStation;
    // wifiStation.start();
    // wifiStation.isConnected(true);

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

    //xTaskCreate(&simple_ota_example_task, "ota_example_task", 8192, NULL, 5, NULL);
    
    while(1) { vTaskDelay(1000 / portTICK_PERIOD_MS); }
}
