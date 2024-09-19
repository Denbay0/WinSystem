// main.cpp
#include "WinSystemInfo.h"
#include <iostream>

int main() {
    SystemInfo sysInfo;
    if (!sysInfo.Initialize()) {
        std::cerr << "Не удалось инициализировать SystemInfo." << std::endl;
        return 1;
    }

    // Получение информации о CPU
    CPUInfo cpu;
    if (sysInfo.GetCPUInfo(cpu)) {
        std::cout << "Производитель CPU: " << cpu.Manufacturer << std::endl;
        std::cout << "Частота CPU: " << cpu.FrequencyGHz << " GHz" << std::endl;
        std::cout << "Серийный номер CPU: " << cpu.SerialNumber << std::endl;
        std::cout << "Количество ядер CPU: " << cpu.CoreCount << std::endl;
        std::cout << "Количество потоков CPU: " << cpu.ThreadCount << std::endl;
        std::cout << "Виртуализация включена: " << (cpu.VirtualizationEnabled ? "Да" : "Нет") << std::endl;
    }
    else {
        std::cerr << "Не удалось получить информацию о CPU." << std::endl;
    }

    // Получение информации о RAM
    RAMInfo ram;
    if (sysInfo.GetRAMInfo(ram)) {
        std::cout << "Общая ОЗУ: " << ram.TotalMemoryMB << " MB" << std::endl;
        std::cout << "Тип памяти: " << ram.MemoryType << std::endl;
        std::cout << "Скорость памяти: " << ram.SpeedMHz << " MHz" << std::endl;
        std::cout << "Использованные слоты: " << ram.UsedSlots << std::endl;
        std::cout << "Свободные слоты: " << ram.FreeSlots << std::endl;
    }
    else {
        std::cerr << "Не удалось получить информацию о RAM." << std::endl;
    }

    // Получение информации о сетевых подключениях
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
                std::cout << "Неизвестно";
            }
            std::cout << ", Протокол: " << net.Protocol << std::endl;
        }
    }
    else {
        std::cerr << "Не удалось получить информацию о сетевых подключениях." << std::endl;
    }

    // Получение информации о виртуализации
    VirtualizationInfo virt;
    if (sysInfo.GetVirtualizationInfo(virt)) {
        std::cout << "Виртуализирована: " << (virt.IsVirtualized ? "Да" : "Нет") << std::endl;
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
                std::cout << "Другое";
            }
            std::cout << std::endl;
        }
    }
    else {
        std::cerr << "Не удалось получить информацию о виртуализации." << std::endl;
    }

    return 0;
}
