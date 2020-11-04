#pragma once

#include "RtosUtils/RoutineTask.hpp"
#include "SmartDevice/MemoryDaemon.hpp"
#include "Network/WifiStation.hpp"
#include "Ota/OtaTask.hpp"
#include "Mqtt/MqttTask.hpp"

namespace SmartDevice
{
    class Kernel : public RoutineTask
    {
    public:
        Kernel();

        OtaTask &getOtaTask();

    protected:
        virtual void initTask() override;

        virtual void task() override;

    private:
        void printSystemInfo();

        void initMemoryDaemon();

        void initNetwork();

        void performOta();

        void initMqtt();

    private:
        MemoryDaemon _memoryDaemon;
        WifiStation _station;
        OtaTask _otaTask;
        MqttTask _mqttTask;
    };
};

