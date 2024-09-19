#include "WinSystemInfo.h"
#include <windows.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <iostream>

#pragma comment(lib, "wbemuuid.lib")

SystemInfo::SystemInfo() : pLoc(nullptr), pSvc(nullptr) {}

SystemInfo::~SystemInfo() {
    if (pSvc) pSvc->Release();
    if (pLoc) pLoc->Release();
    CoUninitialize();
}

bool SystemInfo::Initialize() {
    return InitializeCOM() && InitializeWMI();
}

bool SystemInfo::InitializeCOM() {
    HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres)) {
        std::cerr << "Не удалось инициализировать библиотеку COM. Код ошибки = 0x"
            << std::hex << hres << std::endl;
        return false;
    }

    hres = CoInitializeSecurity(
        NULL,
        -1,                          
        NULL,                        
        NULL,                        
        RPC_C_AUTHN_LEVEL_DEFAULT,    
        RPC_C_IMP_LEVEL_IMPERSONATE,   
        NULL,                        
        EOAC_NONE,                   
        NULL                         
    );

    if (FAILED(hres)) {
        std::cerr << "Не удалось инициализировать безопасность. Код ошибки = 0x"
            << std::hex << hres << std::endl;
        CoUninitialize();
        return false;
    }

    return true;
}

bool SystemInfo::InitializeWMI() {
    HRESULT hres;

    hres = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID*)&pLoc);

    if (FAILED(hres)) {
        std::cerr << "Не удалось создать объект IWbemLocator. Код ошибки = 0x"
            << std::hex << hres << std::endl;
        CoUninitialize();
        return false;
    }

    hres = pLoc->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"), 
        NULL,                    
        NULL,                    
        0,                       
        NULL,                    
        0,                       
        0,                       
        &pSvc                    
    );

    if (FAILED(hres)) {
        std::cerr << "Не удалось подключиться к WMI. Код ошибки = 0x"
            << std::hex << hres << std::endl;
        pLoc->Release();
        CoUninitialize();
        return false;
    }

    hres = CoSetProxyBlanket(
        pSvc,                        
        RPC_C_AUTHN_WINNT,          
        RPC_C_AUTHZ_NONE,            
        NULL,                        
        RPC_C_AUTHN_LEVEL_CALL,      
        RPC_C_IMP_LEVEL_IMPERSONATE, 
        NULL,                        
        EOAC_NONE                    
    );

    if (FAILED(hres)) {
        std::cerr << "Не удалось установить уровень безопасности прокси. Код ошибки = 0x"
            << std::hex << hres << std::endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return false;
    }

    return true;
}


bool SystemInfo::GetCPUInfo(CPUInfo& cpuInfo) {
    IEnumWbemClassObject* pEnumerator = nullptr;
    HRESULT hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT * FROM Win32_Processor"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator);

    if (FAILED(hres)) {
        std::cerr << "WMI запрос для CPU не удался. Код ошибки = 0x"
            << std::hex << hres << std::endl;
        return false;
    }

    IWbemClassObject* pclsObj = nullptr;
    ULONG uReturn = 0;

    hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
    if (uReturn != 0) {
        VARIANT vtProp;

        hres = pclsObj->Get(L"Manufacturer", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hres) && vtProp.vt == VT_BSTR && vtProp.bstrVal != NULL) {
            cpuInfo.Manufacturer = _bstr_t(vtProp.bstrVal);
        }
        else {
            cpuInfo.Manufacturer = "Unknown";
        }
        VariantClear(&vtProp);

        hres = pclsObj->Get(L"MaxClockSpeed", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hres) && (vtProp.vt == VT_UI4 || vtProp.vt == VT_I4)) {
            cpuInfo.FrequencyGHz = static_cast<double>(vtProp.ulVal) / 1000.0;
        }
        else {
            cpuInfo.FrequencyGHz = 0.0;
        }
        VariantClear(&vtProp);

        hres = pclsObj->Get(L"ProcessorId", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hres) && vtProp.vt == VT_BSTR && vtProp.bstrVal != NULL) {
            cpuInfo.SerialNumber = _bstr_t(vtProp.bstrVal);
        }
        else {
            cpuInfo.SerialNumber = "Unknown";
        }
        VariantClear(&vtProp);

        hres = pclsObj->Get(L"NumberOfCores", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hres) && (vtProp.vt == VT_UI4 || vtProp.vt == VT_I4)) {
            cpuInfo.CoreCount = vtProp.ulVal;
        }
        else {
            cpuInfo.CoreCount = 0;
        }
        VariantClear(&vtProp);

        hres = pclsObj->Get(L"ThreadCount", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hres) && (vtProp.vt == VT_UI4 || vtProp.vt == VT_I4)) {
            cpuInfo.ThreadCount = vtProp.ulVal;
        }
        else {
            cpuInfo.ThreadCount = 0;
        }
        VariantClear(&vtProp);

        hres = pclsObj->Get(L"VirtualizationFirmwareEnabled", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hres) && vtProp.vt == VT_BOOL) {
            cpuInfo.VirtualizationEnabled = (vtProp.boolVal == VARIANT_TRUE);
        }
        else {
            cpuInfo.VirtualizationEnabled = false;
        }
        VariantClear(&vtProp);

        pclsObj->Release();
        pEnumerator->Release();
        return true;
    }

    if (pclsObj) pclsObj->Release();
    pEnumerator->Release();
    return false;
}

bool SystemInfo::GetRAMInfo(RAMInfo& ramInfo) {
    IEnumWbemClassObject* pEnumerator = nullptr;
    HRESULT hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT * FROM Win32_PhysicalMemory"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator);

    if (FAILED(hres)) {
        std::cerr << "WMI запрос для RAM не удался. Код ошибки = 0x"
            << std::hex << hres << std::endl;
        return false;
    }

    IWbemClassObject* pclsObj = nullptr;
    ULONG uReturn = 0;

    uint64_t totalMemory = 0;
    uint32_t usedSlots = 0;
    uint32_t freeSlots = 0;
    std::string memoryType = "Unknown";
    uint32_t speedMHz = 0;

    while (pEnumerator) {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
        if (uReturn == 0) break;

        VARIANT vtProp;

        hr = pclsObj->Get(L"Capacity", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && vtProp.vt == VT_BSTR && vtProp.bstrVal != NULL) {
            uint64_t capacity = _wtoi64(vtProp.bstrVal);
            totalMemory += capacity / (1024 * 1024); 
        }
        VariantClear(&vtProp);

        hr = pclsObj->Get(L"MemoryType", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && (vtProp.vt == VT_UI4 || vtProp.vt == VT_I4)) {
            switch (vtProp.ulVal) {
            case 24:
                memoryType = "DDR3";
                break;
            case 26:
                memoryType = "DDR4";
                break;
            default:
                memoryType = "Other";
                break;
            }
        }
        VariantClear(&vtProp);

        hr = pclsObj->Get(L"Speed", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && (vtProp.vt == VT_UI4 || vtProp.vt == VT_I4)) {
            speedMHz = vtProp.ulVal;
        }
        VariantClear(&vtProp);

        usedSlots++;
        pclsObj->Release();
    }

    hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT * FROM Win32_PhysicalMemoryArray"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator);

    if (SUCCEEDED(hres) && pEnumerator) {
        while (pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn) == WBEM_S_NO_ERROR) {
            VARIANT vtProp;
            hres = pclsObj->Get(L"MemoryDevices", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hres) && (vtProp.vt == VT_UI4 || vtProp.vt == VT_I4)) {
                freeSlots = vtProp.ulVal - usedSlots;
            }
            VariantClear(&vtProp);
            pclsObj->Release();
        }
        pEnumerator->Release();
    }

    ramInfo.TotalMemoryMB = totalMemory;
    ramInfo.MemoryType = memoryType;
    ramInfo.SpeedMHz = speedMHz;
    ramInfo.UsedSlots = usedSlots;
    ramInfo.FreeSlots = freeSlots;

    return true;
}

bool SystemInfo::GetNetworkInfo(std::vector<NetworkInfo>& networks) {
    IEnumWbemClassObject* pEnumerator = nullptr;
    HRESULT hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT * FROM Win32_NetworkAdapter WHERE NetEnabled=true"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator);

    if (FAILED(hres)) {
        std::cerr << "WMI запрос для сетевых адаптеров не удался. Код ошибки = 0x"
            << std::hex << hres << std::endl;
        return false;
    }

    IWbemClassObject* pclsObj = nullptr;
    ULONG uReturn = 0;

    while (pEnumerator) {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
        if (uReturn == 0) break;

        VARIANT vtProp;

        NetworkInfo netInfo;
        netInfo.ConnectionType = NetworkConnectionType::Unknown;
        netInfo.Protocol = "Unknown";

        hr = pclsObj->Get(L"AdapterType", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && vtProp.vt == VT_BSTR && vtProp.bstrVal != NULL) {
            std::wstring adapterType = vtProp.bstrVal;
            if (adapterType.find(L"Wireless") != std::wstring::npos) {
                netInfo.ConnectionType = NetworkConnectionType::Wireless;
                netInfo.Protocol = "802.11";
            }
            else if (adapterType.find(L"Ethernet") != std::wstring::npos) {
                netInfo.ConnectionType = NetworkConnectionType::Wired;
                netInfo.Protocol = "Ethernet"; 
            }
        }
        VariantClear(&vtProp);

        networks.push_back(netInfo);
        pclsObj->Release();
    }
    pEnumerator->Release();
    return true;
}

bool SystemInfo::GetVirtualizationInfo(VirtualizationInfo& virtInfo) {
    IEnumWbemClassObject* pEnumerator = nullptr;
    HRESULT hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT * FROM Win32_ComputerSystem"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator);

    if (FAILED(hres)) {
        std::cerr << "WMI запрос для ComputerSystem не удался. Код ошибки = 0x"
            << std::hex << hres << std::endl;
        return false;
    }

    IWbemClassObject* pclsObj = nullptr;
    ULONG uReturn = 0;

    virtInfo.IsVirtualized = false;
    virtInfo.Type = HypervisorType::NotVirtualized;

    hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
    if (uReturn != 0) {
        VARIANT vtProp;

        hres = pclsObj->Get(L"Manufacturer", 0, &vtProp, 0, 0);
        std::wstring manufacturer = L"";
        if (SUCCEEDED(hres) && vtProp.vt == VT_BSTR && vtProp.bstrVal != NULL) {
            manufacturer = vtProp.bstrVal;
        }
        VariantClear(&vtProp);

        hres = pclsObj->Get(L"Model", 0, &vtProp, 0, 0);
        std::wstring model = L"";
        if (SUCCEEDED(hres) && vtProp.vt == VT_BSTR && vtProp.bstrVal != NULL) {
            model = vtProp.bstrVal;
        }
        VariantClear(&vtProp);

        if (manufacturer.find(L"VMware") != std::wstring::npos ||
            model.find(L"VMware") != std::wstring::npos) {
            virtInfo.IsVirtualized = true;
            virtInfo.Type = HypervisorType::VMware;
        }
        else if (manufacturer.find(L"Microsoft") != std::wstring::npos &&
            model.find(L"Virtual") != std::wstring::npos) {
            virtInfo.IsVirtualized = true;
            virtInfo.Type = HypervisorType::HyperV;
        }
        else if (manufacturer.find(L"VirtualBox") != std::wstring::npos) {
            virtInfo.IsVirtualized = true;
            virtInfo.Type = HypervisorType::VirtualBox;
        }
        else if (manufacturer.find(L"KVM") != std::wstring::npos) {
            virtInfo.IsVirtualized = true;
            virtInfo.Type = HypervisorType::KVM;
        }
        else if (manufacturer.find(L"Xen") != std::wstring::npos) {
            virtInfo.IsVirtualized = true;
            virtInfo.Type = HypervisorType::Xen;
        }

        pclsObj->Release();
    }

    pEnumerator->Release();
    return true;
}
