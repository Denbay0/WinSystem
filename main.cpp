// main.cpp
#include "WinSystemInfo.h"
#include <iostream>

int main() {
    SystemInfo sysInfo;
    if (!sysInfo.Initialize()) {
        std::cerr << "�� 㤠���� ���樠����஢��� SystemInfo." << std::endl;
        return 1;
    }

    // ����祭�� ���ଠ樨 � CPU
    CPUInfo cpu;
    if (sysInfo.GetCPUInfo(cpu)) {
        std::cout << "�ந�����⥫� CPU: " << cpu.Manufacturer << std::endl;
        std::cout << "����� CPU: " << cpu.FrequencyGHz << " GHz" << std::endl;
        std::cout << "��਩�� ����� CPU: " << cpu.SerialNumber << std::endl;
        std::cout << "������⢮ 拉� CPU: " << cpu.CoreCount << std::endl;
        std::cout << "������⢮ ��⮪�� CPU: " << cpu.ThreadCount << std::endl;
        std::cout << "����㠫����� ����祭�: " << (cpu.VirtualizationEnabled ? "��" : "���") << std::endl;
    }
    else {
        std::cerr << "�� 㤠���� ������� ���ଠ�� � CPU." << std::endl;
    }

    // ����祭�� ���ଠ樨 � RAM
    RAMInfo ram;
    if (sysInfo.GetRAMInfo(ram)) {
        std::cout << "���� ���: " << ram.TotalMemoryMB << " MB" << std::endl;
        std::cout << "��� �����: " << ram.MemoryType << std::endl;
        std::cout << "������� �����: " << ram.SpeedMHz << " MHz" << std::endl;
        std::cout << "�ᯮ�짮����� ᫮��: " << ram.UsedSlots << std::endl;
        std::cout << "�������� ᫮��: " << ram.FreeSlots << std::endl;
    }
    else {
        std::cerr << "�� 㤠���� ������� ���ଠ�� � RAM." << std::endl;
    }

    // ����祭�� ���ଠ樨 � �⥢�� ������祭���
    std::vector<NetworkInfo> networks;
    if (sysInfo.GetNetworkInfo(networks)) {
        for (const auto& net : networks) {
            std::cout << "��� �⥢��� ������祭��: ";
            switch (net.ConnectionType) {
            case NetworkConnectionType::Wired:
                std::cout << "�஢�����";
                break;
            case NetworkConnectionType::Wireless:
                std::cout << "���஢�����";
                break;
            default:
                std::cout << "�������⭮";
            }
            std::cout << ", ��⮪��: " << net.Protocol << std::endl;
        }
    }
    else {
        std::cerr << "�� 㤠���� ������� ���ଠ�� � �⥢�� ������祭���." << std::endl;
    }

    // ����祭�� ���ଠ樨 � ����㠫���樨
    VirtualizationInfo virt;
    if (sysInfo.GetVirtualizationInfo(virt)) {
        std::cout << "����㠫���஢���: " << (virt.IsVirtualized ? "��" : "���") << std::endl;
        if (virt.IsVirtualized) {
            std::cout << "��� ����ࢨ���: ";
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
                std::cout << "��㣮�";
            }
            std::cout << std::endl;
        }
    }
    else {
        std::cerr << "�� 㤠���� ������� ���ଠ�� � ����㠫���樨." << std::endl;
    }

    return 0;
}
