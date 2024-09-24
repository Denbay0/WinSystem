// main.cpp
#include "WinSystemInfo.h"
#include <iostream>

int main() {
    SystemInfo sysInfo;
    if (!sysInfo.Initialize()) {
        std::cerr << "Не удалось инициализировать SystemInfo." << std::endl;
        return 1;
    }

    // Информация о процессоре CPU
    CPUInfo cpu;
    if (sysInfo.GetCPUInfo(cpu)) {
        std::cout << "Производитель CPU: " << cpu.Manufacturer << std::endl;
        std::cout << "Частота CPU: " << cpu.FrequencyGHz << " GHz" << std::endl;
        std::cout << "Серийный номер CPU: " << cpu.SerialNumber << std::endl;
        std::cout << "Количество ядер CPU: " << cpu.CoreCount << std::endl;
        std::cout << "Количество потоков CPU: " << cpu.ThreadCount << std::endl;
        std::cout << "Виртуализация: " << (cpu.VirtualizationEnabled ? "Да" : "Нет") << std::endl;
    }
    else {
        std::cerr << "Не удалось получить информацию о CPU." << std::endl;
    }

    // Информация о RAM
    RAMInfo ram;
    if (sysInfo.GetRAMInfo(ram)) {
        std::cout << "Объем памяти: " << ram.TotalMemoryMB << " MB" << std::endl;
        std::cout << "Тип памяти: " << ram.MemoryType << std::endl;
        std::cout << "Частота памяти: " << ram.SpeedMHz << " MHz" << std::endl;
        std::cout << "Используемых слотов: " << ram.UsedSlots << std::endl;
        std::cout << "Свободных слотов: " << ram.FreeSlots << std::endl;
    }
    else {
        std::cerr << "Не удалось получить информацию о RAM." << std::endl;
    }

    // Информация о сетевых интерфейсах
    std::vector<NetworkInfo> networks;
    if (sysInfo.GetNetworkInfo(networks)) {
        for (const auto& net : networks) {
            std::cout << "Тип сетевого подключения: ";
            switch (net.ConnectionType) {
            case NetworkConnectionType::Wired:
                std::cout << "Проводное";
                break;
            case NetworkConnectionType::Wireless:
                std::cout << "Беспроводное";
                break;
            default:
                std::cout << "Неизвестное";
            }
            std::cout << ", Протокол: " << net.Protocol << std::endl;
        }
    }
    else {
        std::cerr << "Не удалось получить информацию о сетевых интерфейсах." << std::endl;
    }

    // Информация о виртуализации
    VirtualizationInfo virt;
    if (sysInfo.GetVirtualizationInfo(virt)) {
        std::cout << "Виртуализировано: " << (virt.IsVirtualized ? "Да" : "Нет") << std::endl;
        if (virt.IsVirtualized) {
            std::cout << "Тип гипервизора: ";
            switch (virt.Type) {
            case HypervisorType::VMware:
                std::cout << "VMware";
                break;
            case HypervisorType::HyperV:
                std::cout << "Hyper-V";
                break;
            case HypervisorType::VirtualBox:
                std::cout << "VirtualBox";
                break;
            case HypervisorType::KVM:
                std::cout << "KVM";
                break;
            case HypervisorType::Xen:
                std::cout << "Xen";
                break;
            default:
                std::cout << "Неизвестно";
            }
            std::cout << std::endl;
        }
    }
    else {
        std::cerr << "Не удалось получить информацию о виртуализации." << std::endl;
    }

    return 0;
}
