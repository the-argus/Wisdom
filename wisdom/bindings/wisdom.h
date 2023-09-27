#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct WisResult WisResult;
typedef struct WisAdapterDesc WisAdapterDesc;
typedef enum WisStatus WisStatus;
typedef enum WisAdapterPreference WisAdapterPreference;
typedef enum WisAdapterFlags WisAdapterFlags;

enum WisStatus : uint32_t {
    WisStatusOk = 0,
    WisStatusError = 1,
    WisStatusInvalidArgument = 2,
    WisStatusOutOfMemory = 3,
    WisStatusDeviceLost = 4,
};

enum WisAdapterPreference : int32_t {
    WisAdapterPreferenceNone = 0,
    WisAdapterPreferenceMinConsumption = 1,
    WisAdapterPreferencePerformance = 2,
};

enum WisAdapterFlags : uint32_t {
    WisAdapterFlagsNone = 0x0,
    WisAdapterFlagsRemote = 1 << 0,
    WisAdapterFlagsSoftware = 1 << 1,
    WisAdapterFlagsDXACGCompatible = 1 << 2,
    WisAdapterFlagsDXSupportsMonitoredFences = 1 << 3,
    WisAdapterFlagsDXSupportsNonMonitoredFences = 1 << 4,
    WisAdapterFlagsDXKeyedMutexConformance = 1 << 5,
    WisAdapterFlagsMax = 0xFFFFFFFF,
};

struct WisResult{
    WisStatus status;
    const char* error;
};

struct WisAdapterDesc{
    const char description[256];
    uint32_t vendor_id;
    uint32_t device_id;
    uint32_t subsys_id;
    uint32_t revision;
    uint64_t dedicated_video_memory;
    uint64_t dedicated_system_memory;
    uint64_t shared_system_memory;
    uint64_t adapter_id;
    WisAdapterFlags flags;
};

//==================================HANDLES==================================

typedef struct DX12Factory_t* DX12Factory;
typedef struct VKFactory_t* VKFactory;

typedef struct DX12Adapter_t* DX12Adapter;
typedef struct VKAdapter_t* VKAdapter;

//=================================FUNCTIONS=================================

WisResult DX12FactoryCreate(DX12Factory* out_handle, bool debug_layer);
WisResult VKFactoryCreate(VKFactory* out_handle, bool debug_layer);
void DX12FactoryDestroy(DX12Factory self);
void VKFactoryDestroy(VKFactory self);
