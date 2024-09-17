#include "WinSystemInfo.h"
#include <iostream>
#include <vector>

void printNetworkInfo(const std::vector<WinSystemInfo::NetworkInfo>& networks) {
    std::cout << "Network Information:" << std::endl;
    for (const auto& network : networks) {
        std::cout << "Network Type: "
            << (network.type == WinSystemInfo::NetworkType::Wired ? "Wired" :
                network.type == WinSystemInfo::NetworkType::Wireless ? "Wireless" : "Unknown")
            << ", Protocol: " << network.protocol << std::endl;
    }
}

void printMemoryInfo(const WinSystemInfo::MemoryInfo& info) {
    std::cout << "Memory Information:" << std::endl;
    std::cout << "Total Memory: " << info.totalMemoryMB << " MB" << std::endl;
    std::cout << "Type: " << (info.type == WinSystemInfo::MemoryType::DDR ? "DDR" :
        info.type == WinSystemInfo::MemoryType::DDR2 ? "DDR2" :
        info.type == WinSystemInfo::MemoryType::DDR3 ? "DDR3" :
        info.type == WinSystemInfo::MemoryType::DDR4 ? "DDR4" :
        info.type == WinSystemInfo::MemoryType::DDR5 ? "DDR5" : "Unknown") << std::endl;
    std::cout << "Speed: " << info.speedMHz << " MHz" << std::endl;
    std::cout << "Occupied Slots: " << info.occupiedSlots << std::endl;
    std::cout << "Free Slots: " << info.freeSlots << std::endl;
}

void printCPUInfo(const WinSystemInfo::CPUInfo& info) {
    std::cout << "CPU Information:" << std::endl;
    std::cout << "Manufacturer: " << info.manufacturer << std::endl;
    std::cout << "Frequency: " << info.frequencyMHz << " MHz" << std::endl;
    std::cout << "Cores: " << info.cores << std::endl;
    std::cout << "Threads: " << info.threads << std::endl;
    std::cout << "Virtualization: " << (info.virtualizationEnabled ? "Enabled" : "Disabled") << std::endl;
}

void printVirtualizationInfo(const WinSystemInfo::VirtualizationInfo& info) {
    std::cout << "Virtualization Information:" << std::endl;
    std::cout << "Virtual Machine: " << (info.isVirtualMachine ? "Yes" : "No") << std::endl;
    std::cout << "Hypervisor Type: " << info.hypervisorType << std::endl; 
}

int main() {
    WinSystemInfo systemInfo;

    auto networkInfo = systemInfo.getNetworkInfo();
    auto memoryInfo = systemInfo.getMemoryInfo();
    auto cpuInfo = systemInfo.getCPUInfo();
    auto virtualizationInfo = systemInfo.getVirtualizationInfo();

    printNetworkInfo(networkInfo);
    printMemoryInfo(memoryInfo);
    printCPUInfo(cpuInfo);
    printVirtualizationInfo(virtualizationInfo);

    return 0;
}
