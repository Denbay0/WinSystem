#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <Wbemidl.h> 
#include <comdef.h>  


enum class NetworkConnectionType {
    Wired,
    Wireless,
    Unknown
};

enum class HypervisorType {
    VMware,
    HyperV,
    VirtualBox,
    KVM,
    Xen,
    Other,
    NotVirtualized
};


struct CPUInfo {
    std::string Manufacturer;
    double FrequencyGHz;
    std::string SerialNumber;
    uint32_t CoreCount;
    uint32_t ThreadCount;
    bool VirtualizationEnabled;
};

struct RAMInfo {
    uint64_t TotalMemoryMB;
    std::string MemoryType;
    uint32_t SpeedMHz;
    uint32_t UsedSlots;
    uint32_t FreeSlots;
};

struct NetworkInfo {
    NetworkConnectionType ConnectionType;
    std::string Protocol;
};

struct VirtualizationInfo {
    bool IsVirtualized;
    HypervisorType Type;
};

class SystemInfo {
public:
    SystemInfo();
    ~SystemInfo();

    bool Initialize();

    bool GetNetworkInfo(std::vector<NetworkInfo>& networks);
    bool GetCPUInfo(CPUInfo& cpuInfo);
    bool GetRAMInfo(RAMInfo& ramInfo);
    bool GetVirtualizationInfo(VirtualizationInfo& virtInfo);

private:

    bool InitializeCOM();
    bool InitializeWMI();

    IWbemLocator* pLoc;
    IWbemServices* pSvc;
};

