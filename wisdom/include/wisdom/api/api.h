// Generated by wisdom generator on 2023-11-02 00:00:59.4572573 GMT+1
#pragma once
#include <array>

namespace wis {
struct Result;
struct AdapterDesc;
struct RootConstant;
struct PushDescriptor;

enum class ShaderStages {
    All = 0,
    Vertex = 1,
    Hull = 2,
    Domain = 3,
    Geometry = 4,
    Pixel = 5,
    Amplification = 6,
    Mesh = 7,
};

enum class Status : int32_t {
    Ok = 0,
    Timeout = 1,
    Error = -1,
    InvalidArgument = -2,
    OutOfMemory = -3,
    DeviceLost = -4,
};

enum class QueuePriority {
    Common = 0,
    High = 100,
    Realtime = 10000,
};

enum class MutiWaitFlags : uint32_t {
    All = 0,
    Any = 1,
};

enum class DescriptorType {
    None = 0,
    ConstantBuffer = 2,
    ShaderResource = 3,
    UnorderedAccess = 4,
};

enum class QueueType : uint32_t {
    Graphics = 0,
    DX12Bundle = 1,
    Compute = 2,
    Copy = 3,
    VideoDecode = 4,
    DX12VideoProcess = 5,
    DX12VideoEncode = 6,
};

enum class AdapterPreference : int32_t {
    None = 0,
    MinConsumption = 1,
    Performance = 2,
};

enum class Severity {
    Debug = 0,
    Trace = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Critical = 5,
};

enum class InputClass {
    PerVertex = 0,
    PerInstance = 1,
};

enum class DataFormat {
    Unknown = 0,
    RGBA32Float = 2,
    RGBA32Uint = 3,
    RGBA32Sint = 4,
    RGB32Float = 6,
    RGB32Uint = 7,
    RGB32Sint = 8,
    RGBA16Float = 10,
    RGBA16Unorm = 11,
    RGBA16Uint = 12,
    RGBA16Snorm = 13,
    RGBA16Sint = 14,
    RG32Float = 16,
    RG32Uint = 17,
    RG32Sint = 18,
    D32FloatS8Uint = 20,
    RGB10A2Unorm = 24,
    RGB10A2Uint = 25,
    RG11B10Float = 26,
    RGBA8Unorm = 28,
    RGBA8UnormSrgb = 29,
    RGBA8Uint = 30,
    RGBA8Snorm = 31,
    RGBA8Sint = 32,
    RG16Float = 34,
    RG16Unorm = 35,
    RG16Uint = 36,
    RG16Snorm = 37,
    RG16Sint = 38,
    D32Float = 40,
    R32Float = 41,
    R32Uint = 42,
    R32Sint = 43,
    D24UnormS8Uint = 45,
    RG8Unorm = 49,
    RG8Uint = 50,
    RG8Snorm = 51,
    RG8Sint = 52,
    R16Float = 54,
    D16Unorm = 55,
    R16Unorm = 56,
    R16Uint = 57,
    R16Snorm = 58,
    R16Sint = 59,
    R8Unorm = 61,
    R8Uint = 62,
    R8Snorm = 63,
    R8Sint = 64,
    A8Unorm = 65,
    RGB9E5UFloat = 67,
    RG8BG8Unorm = 68,
    GR8GB8Unorm = 69,
    BC1RGBAUnorm = 71,
    BC1RGBAUnormSrgb = 72,
    BC2RGBAUnorm = 74,
    BC2RGBAUnormSrgb = 75,
    BC3RGBAUnorm = 77,
    BC3RGBAUnormSrgb = 78,
    BC4RUnorm = 80,
    BC4RSnorm = 81,
    BC5RGUnorm = 83,
    BC5RGSnorm = 84,
    B5R6G5Unorm = 85,
    B5G5R5A1Unorm = 86,
    BGRA8Unorm = 87,
    BGRA8UnormSrgb = 91,
    BC6HUfloat16 = 95,
    BC6HSfloat16 = 96,
    BC7RGBAUnorm = 98,
    BC7RGBAUnormSrgb = 99,
};

enum class AdapterFlags {
    None = 0x0,
    Remote = 1 << 0,
    Software = 1 << 1,
    DX12ACGCompatible = 1 << 2,
    DX12SupportsMonitoredFences = 1 << 3,
    DX12SupportsNonMonitoredFences = 1 << 4,
    DX12KeyedMutexConformance = 1 << 5,
};

enum class DeviceFeatures {
    None = 0x0,
    PushDescriptors = 1 << 0,
};

struct Result{
    wis::Status status = wis::Status::Ok;
    const char* error = nullptr;
};

struct AdapterDesc{
    std::array<const char, 256> description {};
    uint32_t vendor_id;
    uint32_t device_id;
    uint32_t subsys_id;
    uint32_t revision;
    uint64_t dedicated_video_memory;
    uint64_t dedicated_system_memory;
    uint64_t shared_system_memory;
    uint64_t adapter_id;
    wis::AdapterFlags flags;
};

struct RootConstant{
    wis::ShaderStages stage;
    uint32_t size_bytes;
};

struct PushDescriptor{
    wis::ShaderStages stage;
    uint32_t bind_register;
    wis::DescriptorType type;
    uint32_t reserved;
};

//=================================DELEGATES=================================

typedef void (*DebugCallback)( wis::Severity severity,  const char* message,  void* user_data);
//==============================TYPE TRAITS==============================

template <typename T> struct is_flag_enum : public std::false_type {};
template <> struct is_flag_enum<wis::AdapterFlags>:public std::true_type {};
template <> struct is_flag_enum<wis::DeviceFeatures>:public std::true_type {};
}
