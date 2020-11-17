#include "SmartDevice/Kernel.hpp"

#include <functional>

#include "esp_log.h"

#include "SystemUtils/Utils.hpp"

#include "RtosUtils/MapTaskSafe.hpp"

#include "SmartMessage/MessageTopicProcessor.hpp"
#include "SmartMessage/MessageDomain.hpp"

#include "BaseSmartMessage/NotifyDeviceAvailableBuilder.hpp"
#include "BaseSmartMessage/RegisterDeviceBuilder.hpp"

static const char *TAG = "Kernel";

namespace SmartDevice
{
    Kernel &Kernel::getInstance()
    {
        static Kernel kernel;
        return kernel;
    }

    Kernel::Kernel()
        :   RoutineTask("SmartDevice:Kernel", 5, 500, 1024 * 4)
    {
        initLogMessageRouter();
    }

    Kernel::~Kernel(){}

    OtaTask &Kernel::getOtaTask()
    {
        return _otaTask;
    }

    MqttTask &Kernel::getMqttTask()
    {
        return _mqttTask;
    }

    DeviceInfo &Kernel::getDeviceInfo()
    {
        return _deviceInfo;
    }

    void Kernel::initLogMessageRouter()
    {
        //TODO
        // _systemLogMessageRouter.appendRouteCallback([&](const char *buffer){});
        _systemLogMessageRouter.init();
    }

    void Kernel::printSystemInfo()
    {
        ESP_LOGI(TAG, "Starting...");
        ESP_LOGI(TAG, "System info:\n\tESP-IDF version: [%s]\n\tAPP Version: [%s]\n\tMAC: [%s]\n\tClientId: [%s]", 
            getDeviceInfo().appDescriptor.idfVersion.c_str(),
            getDeviceInfo().appDescriptor.appVersion.c_str(),
            getDeviceInfo().mac.c_str(),
            getDeviceInfo().clientId.c_str());
    }

    void Kernel::initDeviceInfo()
    {
        if(getDeviceInfo().deviceType == DeviceType::Type::Unknown)
        {
            ESP_LOGE(TAG, "DeviceType: [Unknown]. Stopping kernel init...");
            deleteTask();
        }
        getDeviceInfo().appDescriptor = System::Utils::EspIdf::GetAppDescriptor();
        getDeviceInfo().chipInfo = System::Utils::EspIdf::GetChipInfo();

        getDeviceInfo().mac = System::Utils::MAC::GetClientMac();
        getDeviceInfo().clientId = System::Utils::MAC::GetClientId();
    }

    void Kernel::initMemoryDaemon()
    {
        ESP_LOGI(TAG, "Starting  memory daemon...");
        _memoryDaemon.setUpdateMemoryStateFunction([&](int currentHeapSizePercent, int minHeapSizePercent){
            getDeviceInfo().deviceMemory.setCurrentFreeHeapPercent(currentHeapSizePercent);
            getDeviceInfo().deviceMemory.setMinHeapFreePercent(minHeapSizePercent);
        });
        _memoryDaemon.start();
    }

    void Kernel::initNetwork()
    {
        ESP_LOGI(TAG, "Starting Wifi...");
        _station.start();
        _station.isConnected(true); //wait for connection
    }

    void Kernel::performOta()
    {
        ESP_LOGI(TAG, "Performing OTA...");

        _otaTask.start();
        _otaTask.waitForExecuted();
    }

    void Kernel::initMqtt()
    {
        _mqttTask.setClientId(System::Utils::MAC::GetClientId());
        _mqttTask.setMessageProcessor(std::bind(&MessageManager::process, &_messageManager, std::placeholders::_1));

        _mqttTask.setOnConnectedCallback([&](){
            _mqttTask.appendMessage(BaseSmartMessage::RegisterDeviceBuilder().build());
        });

        _mqttTask.start();
        _mqttTask.waitForInitialized();
    }

    void Kernel::registerMessageHandlers()
    {
        //_messageManager.registerMessage()
    }

    void Kernel::registerMessageSenders()
    {
        _routineMessageSenderTask.setMessageAppender(std::bind(&MqttTask::appendMessage, &_mqttTask, std::placeholders::_1));
        _routineMessageSenderTask.registerRoutineMessage( std::make_shared<BaseSmartMessage::NotifyDeviceAvailableBuilder>(), 5000 );
        _routineMessageSenderTask.start();
    }

    void Kernel::initTask()
    {
        ESP_LOGI(TAG, "Kernel initialising...");

        initDeviceInfo();

        printSystemInfo();

        initMemoryDaemon();

        initNetwork();

        performOta();

        registerMessageHandlers();

        initMqtt();

        registerMessageSenders();

        ESP_LOGI(TAG, "Kernel ready!");
    }

    void Kernel::task()
    {
        // while(1)
        // {
        //     vTaskDelay(pdMS_TO_TICKS(500));
        // }
    }
}