const std = @import("std");
const builtin = @import("builtin");
const zcc = @import("compile_commands");
const app_name = "wisdom";

const release_flags = [_][]const u8{ "-DNDEBUG", "-DRELEASE" };
const debug_flags = [_][]const u8{};

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

pub const Backend = enum { DX, Vulkan };

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});
    const mode = b.standardOptimizeOption(.{});

    // keep track of any targets we create
    var targets = std.ArrayList(*std.Build.CompileStep).init(b.allocator);

    const header_only = b.option(bool, "header_only", "Don't compile wisdom- only install headers for inlining. Defaults to false.") orelse false;
    const wayland_support = b.option(bool, "wayland_support", "Whether to add wayland support. Defaults to true on linux, false otherwise.") orelse (target.getOsTag() == .linux);
    const force_vulkan = b.option(bool, "force_vulkan", "Whether to force the use of vulkan. Useful on Windows. Defaults to false.") orelse false;
    const windows_store = b.option(bool, "windows_store", "Whether to build for the windows store. Defaults to false.") orelse false;

    const backend: Backend = block: {
        if (force_vulkan) {
            break :block .Vulkan;
        } else if (windows_store) {
            break :block .DX;
        }

        switch (target.getOsTag()) {
            .ios, .tvos => {
                std.log.err("Metal is not implemented.", .{});
                return error.MetalNotImplemented;
            },
            .windows => break :block .DX,
            // TODO: what is the intended behavior on mac?
            .linux, .macos => break :block .Vulkan,
            else => break :block .Vulkan,
        }
        break :block .DX;
    };

    {
        // create the static library target
        const lib = b.addStaticLibrary(.{
            .name = app_name,
            .optimize = mode,
            .target = target,
        });
        try targets.append(lib);

        var flags = std.ArrayList([]const u8).init(b.allocator);

        // also add some flags
        flags.appendSlice(if (mode == .Debug) &debug_flags else &release_flags) catch @panic("OOM");
        // this adds intellisense for any headers which are not present in the source of dependencies, but are built and installed
        flags.append(includePrefixFlag(b.allocator, b.install_prefix)) catch @panic("OOM");

        flags.appendSlice(&.{
            "-std=c++20",
            // "-DDEBUG_MODE=$<IF:$<CONFIG:Debug>,1,0>",
            // "-DRUNTIME_ASSERTS=$<BOOL:${RUNTIME_ASSERTS}>",
            // "-DWISDOMDX12=$<BOOL:${WISDOMDX12}>",
            // "-DWISDOMVK=$<BOOL:${WISDOMVK}>",
            // "-DWISDOMMTL=$<BOOL:${WISDOMMTL}>",
            // "-DWISDOM_VERSION=${WISDOM_VERSION}",
            // "-DWISDOM_LOG_LEVEL=${SEV_INDEX}",
            "-DNOMINMAX",
        }) catch @panic("OOM");

        // header only defines
        if (header_only) {
            flags.appendSlice(&.{
                "-DWISDOM_HEADER_ONLY", "-DWIS_EXPORT=", "WIS_INLINE=inline",
            }) catch @panic("OOM");
        }

        if (force_vulkan) {
            flags.appendSlice(&.{"-DWISDOM_FORCE_VULKAN"}) catch @panic("OOM");
        }

        switch (target.getOsTag()) {
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

                if (wayland_support) {
                    flags.appendSlice(&.{
                        "-DVK_USE_PLATFORM_WAYLAND_KHR",
                    }) catch @panic("OOM");
                }
            },
            else => {},
        }

        lib.addCSourceFiles(&.{ "wisdom/src/wisdom/empty.cpp", switch (backend) {
            .Vulkan => "wisdom/src/wisdom/vulkan.cpp",
            .DX => "wisdom/src/wisdom/dx12.cpp",
        } }, flags.items);
    }

    for (targets.items) |t| {
        t.linkLibC();
        t.linkLibCpp();

        // link libraries from build.zig.zon
        switch (backend) {
            .Vulkan => {
                const dep = b.dependency("vulkan_memory_allocator", .{
                    .target = target,
                    .optimize = mode,
                });
                t.linkLibrary(dep.artifact("VulkanMemoryAllocator"));
            },
            .DX => {
                t.linkSystemLibrary("WinRT");
                t.linkSystemLibrary("DX12Agility");
                t.linkSystemLibrary("DX12Allocator");
                t.linkSystemLibrary("d3d12");
                t.linkSystemLibrary("d3d11");
                t.linkSystemLibrary("DXGI");
                t.linkSystemLibrary("DXGUID");
            },
        }
    }

    // links and includes which are shared across platforms
    include(targets, "wisdom/include");

    // platform-specific additions
    switch (target.getOsTag()) {
        .windows => {},
        .macos => {},
        .linux => {},
        else => {},
    }

    for (targets.items) |t| {
        b.installArtifact(t);
    }

    // compile commands step
    zcc.createStep(b, "cdb", try targets.toOwnedSlice());
}

fn link(
    targets: std.ArrayList(*std.Build.CompileStep),
    lib: []const u8,
) void {
    for (targets.items) |target| {
        target.linkSystemLibrary(lib);
    }
}

fn include(
    targets: std.ArrayList(*std.Build.CompileStep),
    path: []const u8,
) void {
    for (targets.items) |target| {
        target.addIncludePath(std.Build.LazyPath{ .path = path });
    }
}

fn includePrefixFlag(ally: std.mem.Allocator, path: []const u8) []const u8 {
    return std.fmt.allocPrint(ally, "-I{s}/include", .{path}) catch @panic("OOM");
}
