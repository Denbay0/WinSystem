#pragma once

#include <string>
#include <vector>
#include <windows.h>
#include <Psapi.h>
#include <intrin.h>

class WinSystemInfo {
public:
    enum class NetworkType {
        Wired,
        Wireless,
        Unknown
    };

    enum class MemoryType {
        DDR,
        DDR2,
        DDR3,
        DDR4,
        DDR5,
        Unknown
    };

    struct NetworkInfo {
        NetworkType type;
        std::string protocol;
    };

    struct CPUInfo {
        std::string manufacturer;
        unsigned int frequencyMHz;
        unsigned int cores;
        unsigned int threads;
        bool virtualizationEnabled;
    };

    struct MemoryInfo {
        unsigned long long totalMemoryMB;
        MemoryType type;
        unsigned int speedMHz; 
        unsigned int occupiedSlots; 
        unsigned int freeSlots; 
    };

    struct VirtualizationInfo {
        bool isVirtualMachine;
        std::string hypervisorType; 
    };

    WinSystemInfo();
    std::vector<NetworkInfo> getNetworkInfo();
    CPUInfo getCPUInfo();
    MemoryInfo getMemoryInfo();
    VirtualizationInfo getVirtualizationInfo();
};
