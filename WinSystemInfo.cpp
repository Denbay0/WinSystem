#include "WinSystemInfo.h"
#include <iphlpapi.h>
#include <vector>
#include <string>
#include <iostream>
#include <memory>

#pragma comment(lib, "iphlpapi.lib")

WinSystemInfo::WinSystemInfo() {}

std::vector<WinSystemInfo::NetworkInfo> WinSystemInfo::getNetworkInfo() {
    std::vector<NetworkInfo> networks;
    IP_ADAPTER_INFO* pAdapterInfo = nullptr;
    IP_ADAPTER_INFO* pAdapter = nullptr;
    ULONG ulOutBufLen = 0;

    // Get the required buffer size
    GetAdaptersInfo(nullptr, &ulOutBufLen);
    pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
    if (pAdapterInfo == nullptr) {
        std::cerr << "Memory allocation failure." << std::endl;
        return networks;
    }

    // Get adapter information
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_SUCCESS) {
        pAdapter = pAdapterInfo;
        while (pAdapter) {
            NetworkInfo netInfo;
            switch (pAdapter->Type) {
            case MIB_IF_TYPE_ETHERNET:
                netInfo.type = NetworkType::Wired;
                netInfo.protocol = "Ethernet";
                break;
            case IF_TYPE_IEEE80211:
                netInfo.type = NetworkType::Wireless;
                netInfo.protocol = "Wi-Fi";
                break;
            default:
                netInfo.type = NetworkType::Unknown;
                netInfo.protocol = "Unknown";
                break;
            }
      
            if (std::find_if(networks.begin(), networks.end(), [&netInfo](const NetworkInfo& existing) {
                return existing.type == netInfo.type && existing.protocol == netInfo.protocol;
                }) == networks.end()) {
                networks.push_back(netInfo);
            }
            pAdapter = pAdapter->Next;
        }
    }

    free(pAdapterInfo);
    return networks;
}

WinSystemInfo::CPUInfo WinSystemInfo::getCPUInfo() {
    CPUInfo info = {};

    
    int cpuInfo[4] = { 0 };
    __cpuid(cpuInfo, 0);

    char vendor[13];
    memcpy(vendor, &cpuInfo[1], 4);
    memcpy(vendor + 4, &cpuInfo[3], 4);
    memcpy(vendor + 8, &cpuInfo[2], 4);
    vendor[12] = '\0';

    info.manufacturer = vendor;

    info.frequencyMHz = 0; 

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    info.cores = sysInfo.dwNumberOfProcessors;

    
    DWORD bufSize = 0;
    GetLogicalProcessorInformation(nullptr, &bufSize);
    std::unique_ptr<BYTE[]> buffer(new BYTE[bufSize]);
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION>(buffer.get());

    if (GetLogicalProcessorInformation(ptr, &bufSize)) {
        info.threads = 0;
        DWORD offset = 0;
        while (offset < bufSize) {
            if (ptr->Relationship == RelationProcessorCore) {
                info.threads += GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);
            }
            offset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
            ptr = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION>(reinterpret_cast<BYTE*>(ptr) + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));
        }
    }

    
    int cpuId[4] = { 0 };
    __cpuid(cpuId, 1);
    info.virtualizationEnabled = (cpuId[2] & (1 << 5)) != 0;

    return info;
}

WinSystemInfo::MemoryInfo WinSystemInfo::getMemoryInfo() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(memInfo);
    if (GlobalMemoryStatusEx(&memInfo)) {
        MemoryInfo info;
        info.totalMemoryMB = memInfo.ullTotalPhys / (1024 * 1024);

        
        info.type = MemoryType::Unknown;
        info.speedMHz = 0; 
        info.occupiedSlots = 0; 
        info.freeSlots = 0; 

        return info;
    }

    return MemoryInfo{};
}

WinSystemInfo::VirtualizationInfo WinSystemInfo::getVirtualizationInfo() {
    VirtualizationInfo info;

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    info.isVirtualMachine = (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64);
    info.hypervisorType = "Unknown"; 

    return info;
}
