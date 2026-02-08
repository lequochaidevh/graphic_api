#pragma once

#include <nvml.h>
#include <iostream>

class NVDIA_Debugger {
 private:
    /* data */
 public:
    NVDIA_Debugger(/* args */);
    ~NVDIA_Debugger();

    void printGPUStats() {
        nvmlInit();  // Initialize NVML

        nvmlDevice_t device;
        // Get handle for the first GPU (index 0)
        nvmlDeviceGetHandleByIndex(0, &device);

        // 1. Get Temperature
        unsigned int temp;
        nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp);

        // 2. Get Utilization (% GPU Used)
        nvmlUtilization_t utilization;
        nvmlDeviceGetUtilizationRates(device, &utilization);

        std::cout << "GPU Temp: " << temp << " ^C   -   "
                  << "GPU Usage: " << utilization.gpu << " %" << std::endl;

        nvmlShutdown();  // Clean up
    };
};

NVDIA_Debugger::NVDIA_Debugger(/* args */) {}

NVDIA_Debugger::~NVDIA_Debugger() {}
