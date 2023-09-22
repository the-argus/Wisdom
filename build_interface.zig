const std = @import("std");
const app_name = "wisdom";

const vulkan_header_prefix = "wisdom/include/wisdom/vulkan/";
const vulkan_headers = &[_][]const u8{
    vulkan_header_prefix ++ "vk_factory.h",
    vulkan_header_prefix ++ "vk_adapter.h",
    vulkan_header_prefix ++ "vk_managed_handles.h",
    vulkan_header_prefix ++ "vk_dynamic_loader.h",
    vulkan_header_prefix ++ "vk_device.h",
    vulkan_header_prefix ++ "vk_allocator.h",
    vulkan_header_prefix ++ "vk_allocator_handles.h",
    vulkan_header_prefix ++ "vk_command_queue.h",
    vulkan_header_prefix ++ "vk_fence.h",
    vulkan_header_prefix ++ "vk_checks.h",
    vulkan_header_prefix ++ "vk_swapchain.h",
    vulkan_header_prefix ++ "vk_format.h",
    vulkan_header_prefix ++ "vk_resource.h",
    vulkan_header_prefix ++ "vk_rtv.h",
    vulkan_header_prefix ++ "vk_command_list.h",
    vulkan_header_prefix ++ "vk_render_pass.h",
    vulkan_header_prefix ++ "vk_pipeline_state.h",
    vulkan_header_prefix ++ "vk_state_builder.h",
    vulkan_header_prefix ++ "vk_shader.h",
    vulkan_header_prefix ++ "vk_root_signature.h",
    vulkan_header_prefix ++ "vk_buffer_views.h",
    vulkan_header_prefix ++ "vk_xshared_handle.h",
    vulkan_header_prefix ++ "vk_views.h",
    vulkan_header_prefix ++ "vk_descriptor_heap.h",
    vulkan_header_prefix ++ "vk_handle_traits.h",
};

const dx_header_prefix = "wisdom/include/wisdom/dx12/";
const dx_headers = &[_][]const u8{
    dx_header_prefix ++ "dx12_factory.h",
    dx_header_prefix ++ "dx12_checks.h",
    dx_header_prefix ++ "dx12_info.h",
    dx_header_prefix ++ "dx12_adapter.h",
    dx_header_prefix ++ "dx12_device.h",
    dx_header_prefix ++ "dx12_command_queue.h",
    dx_header_prefix ++ "dx12_swapchain.h",
    dx_header_prefix ++ "dx12_resource.h",
    dx_header_prefix ++ "dx12_command_list.h",
    dx_header_prefix ++ "dx12_fence.h",
    dx_header_prefix ++ "dx12_rtv.h",
    dx_header_prefix ++ "dx12_pipeline_state.h",
    dx_header_prefix ++ "dx12_root_signature.h",
    dx_header_prefix ++ "dx12_shader.h",
    dx_header_prefix ++ "dx12_allocator.h",
    dx_header_prefix ++ "dx12_state_builder.h",
    dx_header_prefix ++ "dx12_render_pass.h",
    dx_header_prefix ++ "dx12_format.h",
    dx_header_prefix ++ "dx12_views.h",
    dx_header_prefix ++ "dx12_descriptor_heap.h",
};

const release_flags = [_][]const u8{ "-DNDEBUG", "-DRELEASE" };
const debug_flags = [_][]const u8{};

const VulkanBackendOptions = struct {
    // TODO: figure out if force is necessary
    force: bool,
};

pub const Backend = union(enum) {
    DX: void,
    Vulkan: VulkanBackendOptions,
};

pub const LibInfo = struct {
    target: std.zig.CrossTarget,
    optimize: std.builtin.Mode,
};

pub const BuildMode = union(enum) {
    HeaderOnly: void,
    WithLibs: LibInfo,
};

pub const LogLevel = enum(u8) {
    Debug = 0,
    Trace = 1,
    Info = 2,
    Warn = 3,
    Error = 4,
    Critical = 5,
};

pub const WisdomConfiguration = struct {
    backend: Backend,
    debug: bool = true,
    runtime_asserts: bool = true,
    wayland_support: bool = true,
    windows_store: bool = false,
    build_mode: BuildMode,
    log_level: LogLevel = .Info,
};

pub const WisdomBuildResources = struct {
    // Flags (mostly compiler defines) that should be added to C libraries
    // which include wisdom headers.
    // TODO: figure out an abstraction for public/private flags for making
    // something that can compile to an artifact *and* header-only. there might
    // already be something in the zig build api
    flags: []const []const u8,
    static_lib: ?*std.Build.Step.Compile,
};

pub fn buildWisdom(b: *std.Build, config: WisdomConfiguration) WisdomBuildResources {
    var result = WisdomBuildResources{
        .static_lib = null,
        .flags = undefined,
    };

    var flags = std.ArrayList([]const u8).init(b.allocator);

    // also add some flags
    flags.appendSlice(if (config.debug) &debug_flags else &release_flags) catch @panic("OOM");
    // this adds intellisense for any headers which are not present in the source of dependencies, but are built and installed
    flags.append(includePrefixFlag(b.allocator, b.install_prefix)) catch @panic("OOM");

    const debug_int: i32 = if (config.debug) 1 else 0;
    const asserts_int: i32 = if (config.runtime_asserts) 1 else 0;
    const vulkan_int: i32 = if (config.backend == .Vulkan) 1 else 0;
    const dx12_int: i32 = if (config.backend == .DX) 1 else 0;

    flags.appendSlice(&.{
        "-std=c++20",
        std.fmt.allocPrint(b.allocator, "-DDEBUG_MODE={}", .{debug_int}) catch @panic("OOM"),
        std.fmt.allocPrint(b.allocator, "-DRUNTIME_ASSERTS={}", .{asserts_int}) catch @panic("OOM"),
        std.fmt.allocPrint(b.allocator, "-DWISDOM_LOG_LEVEL={}", .{@intFromEnum(config.log_level)}) catch @panic("OOM"),
        std.fmt.allocPrint(b.allocator, "-DWISDOMVK={}", .{vulkan_int}) catch @panic("OOM"),
        std.fmt.allocPrint(b.allocator, "-DWISDOMDX12={}", .{dx12_int}) catch @panic("OOM"),
        // "-DWISDOMMTL=$<BOOL:${WISDOMMTL}>",
        // "-DWISDOM_VERSION=${WISDOM_VERSION}",
        "-DNOMINMAX",
    }) catch @panic("OOM");

    if (config.backend == .Vulkan) {
        flags.appendSlice(&.{
            "-DVULKAN_HPP_NO_EXCEPTIONS",
            "-DWISDOM_VULKAN_FOUND",
        }) catch @panic("OOM");
    }

    // header only defines
    if (config.build_mode == .HeaderOnly) {
        flags.appendSlice(&.{
            "-DWISDOM_HEADER_ONLY", "-DWIS_INLINE=inline",
        }) catch @panic("OOM");
    } else {
        flags.appendSlice(&.{"-DWIS_INLINE="}) catch @panic("OOM");
    }

    // the only time this is "export" is when we're doing modules which uh.
    // aren't going to be a thing soon
    flags.appendSlice(&.{"-DWIS_EXPORT="}) catch @panic("OOM");
    flags.appendSlice(&.{"-DWISDOM_USE_FMT"}) catch @panic("OOM");

    switch (config.backend) {
        .Vulkan => |vulkan| {
            if (vulkan.force) {
                flags.appendSlice(&.{"-DWISDOM_FORCE_VULKAN"}) catch @panic("OOM");
            }
        },
        else => {},
    }

    // install headers always
    {
        const headers = switch (config.backend) {
            .DX => dx_headers,
            .Vulkan => vulkan_headers,
        };
        for (headers) |h| {
            b.installFile(h, std.fs.path.join(
                b.allocator,
                &.{ "include", "wisdom", std.fs.path.basename(h) },
            ) catch @panic("OOM"));
        }
    }

    switch (config.build_mode) {
        .WithLibs => |lib_info| {
            switch (lib_info.target.getOsTag()) {
                .macos => {
                    flags.appendSlice(&.{
                        "-DWISDOM_MAC",
                        "-DVK_USE_PLATFORM_METAL_EXT",
                    }) catch @panic("OOM");
                },
                .linux => {
                    flags.appendSlice(&.{
                        "-DWISDOM_LINUX",
                        "-DVK_USE_PLATFORM_XCB_KHR",
                    }) catch @panic("OOM");

                    if (config.wayland_support) {
                        flags.appendSlice(&.{
                            "-DVK_USE_PLATFORM_WAYLAND_KHR",
                        }) catch @panic("OOM");
                    }
                },
                else => {},
            }

            const lib = b.addStaticLibrary(.{
                .name = app_name,
                .optimize = lib_info.optimize,
                .target = lib_info.target,
            });

            lib.addCSourceFiles(&.{ "wisdom/src/wisdom/empty.cpp", switch (config.backend) {
                .Vulkan => "wisdom/src/wisdom/vulkan.cpp",
                .DX => "wisdom/src/wisdom/dx12.cpp",
            } }, flags.items);

            lib.addIncludePath(.{ .path = "wisdom/include" });

            linkWisdomStep(b, lib, config.backend);

            result.static_lib = lib;
        },

        .HeaderOnly => {
            @panic("header only currently unsupported");
        },
    }

    result.flags = flags.toOwnedSlice() catch @panic("OOM");
    return result;
}

pub fn linkWisdomStep(b: *std.Build, cstep: *std.Build.Step.Compile, backend: Backend) void {
    // link libraries from build.zig.zon
    switch (backend) {
        .Vulkan => |_| {
            // vulkan memory allocator
            const vma = b.dependency("vulkan_memory_allocator", .{
                .target = cstep.target,
                .optimize = cstep.optimize,
            });
            // add vma headers
            cstep.step.dependOn(vma.builder.getInstallStep());
            cstep.addIncludePath(.{ .path = std.fs.path.join(
                b.allocator,
                &.{ vma.builder.install_path, "include" },
            ) catch @panic("OOM") });

            // add vma static lib
            var vma_lib = vma.artifact("VulkanMemoryAllocator");
            cstep.linkLibrary(vma_lib);

            // add the VULKAN_SDK to the library search path for VMA
            const vulkan_sdk = std.process.getEnvVarOwned(b.allocator, "VULKAN_SDK") catch |err| switch (err) {
                error.OutOfMemory => @panic("OOM"),
                error.InvalidUtf8 => @panic("VULKAN_SDK invalid environment var name"),
                error.EnvironmentVariableNotFound => @panic("You have no set the VULKAN_SDK environment variable- usually this means you haven't installed it."),
            };
            vma_lib.addLibraryPath(.{ .path = vulkan_sdk });

            // fmt
            const fmt = b.dependency("fmt", .{});
            cstep.step.dependOn(fmt.builder.getInstallStep());
            cstep.addIncludePath(.{
                .path = std.fs.path.join(b.allocator, &.{ fmt.builder.install_path, "include" }) catch @panic("OOM"),
            });

            // link system libraries
            switch (cstep.target.getOsTag()) {
                .linux => {
                    cstep.linkSystemLibrary("xcb");
                },
                else => {},
            }
        },
        .DX => {
            cstep.linkSystemLibrary("WinRT");
            cstep.linkSystemLibrary("DX12Agility");
            cstep.linkSystemLibrary("DX12Allocator");
            cstep.linkSystemLibrary("d3d12");
            cstep.linkSystemLibrary("d3d11");
            cstep.linkSystemLibrary("DXGI");
            cstep.linkSystemLibrary("DXGUID");
        },
    }
}

/// Based on platform, forcing vulkan, and building for windows store, create a
/// Backend object. windows_store is basically force_dx.
pub fn deduceBackend(target: std.zig.CrossTarget, force_vulkan: bool, windows_store: bool) Backend {
    if (force_vulkan) {
        return Backend{ .Vulkan = .{ .force = true } };
    } else if (windows_store) {
        return .DX;
    }

    switch (target.getOsTag()) {
        .ios, .tvos => {
            @panic("Metal is not implemented.");
        },
        .windows => return .DX,
        // TODO: what is the intended behavior on mac?
        .linux, .macos => return Backend{ .Vulkan = .{ .force = false } },
        else => return Backend{ .Vulkan = .{ .force = false } },
    }
    return Backend{ .Vulkan = .{ .force = false } };
}

fn includePrefixFlag(ally: std.mem.Allocator, path: []const u8) []const u8 {
    return std.fmt.allocPrint(ally, "-I{s}/include", .{path}) catch @panic("OOM");
}
