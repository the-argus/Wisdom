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
pub const LogLevel = enum(u8) {
    Debug = 0,
    Trace = 1,
    Info = 2,
    Warn = 3,
    Error = 4,
    Critical = 5,
};

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});
    const mode = b.standardOptimizeOption(.{});

    // keep track of any targets we create
    var targets = std.ArrayList(*std.Build.CompileStep).init(b.allocator);

    const header_only = b.option(bool, "header_only", "Don't compile wisdom- only install headers for inlining. Defaults to false.") orelse false;
    const wayland_support = b.option(bool, "wayland_support", "Whether to add wayland support. Defaults to true on linux, false otherwise.") orelse (target.getOsTag() == .linux);
    const force_vulkan = b.option(bool, "force_vulkan", "Whether to force the use of vulkan. Useful on Windows. Defaults to false.") orelse false;
    const windows_store = b.option(bool, "windows_store", "Whether to build for the windows store. Defaults to false.") orelse false;
    const runtime_asserts = b.option(bool, "runtime_asserts", "Whether to make asserts at runtime. Defaults to true in debug mode, false otherwise.") orelse (mode == .Debug);
    const log_level = b.option(LogLevel, "log_level", "The minimum log level to display.") orelse (if (mode == .Debug) LogLevel.Debug else LogLevel.Critical);

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
        break :block .Vulkan;
    };

    if (!header_only) {
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

        const debug_int: i32 = if (mode == .Debug) 1 else 0;
        const asserts_int: i32 = if (runtime_asserts) 1 else 0;
        const vulkan_int: i32 = if (backend == .Vulkan) 1 else 0;
        const dx12_int: i32 = if (backend == .DX) 1 else 0;

        flags.appendSlice(&.{
            "-std=c++20",
            std.fmt.allocPrint(b.allocator, "-DDEBUG_MODE={}", .{debug_int}) catch @panic("OOM"),
            std.fmt.allocPrint(b.allocator, "-DRUNTIME_ASSERTS={}", .{asserts_int}) catch @panic("OOM"),
            std.fmt.allocPrint(b.allocator, "-DWISDOM_LOG_LEVEL={}", .{@intFromEnum(log_level)}) catch @panic("OOM"),
            std.fmt.allocPrint(b.allocator, "-DWISDOMVK={}", .{vulkan_int}) catch @panic("OOM"),
            std.fmt.allocPrint(b.allocator, "-DWISDOMDX12={}", .{dx12_int}) catch @panic("OOM"),
            // "-DWISDOMMTL=$<BOOL:${WISDOMMTL}>",
            // "-DWISDOM_VERSION=${WISDOM_VERSION}",
            "-DNOMINMAX",
        }) catch @panic("OOM");

        // header only defines
        if (header_only) {
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

        // we only need to link libraries when building the static lib, not
        // header-only
        // TODO: vulkan memory allocator and etc should have their headers
        // installed into the out, if we're building header-only. maybe for
        // static lib as well? people might want to access the impl
        for (targets.items) |t| {
            // link libraries from build.zig.zon
            switch (backend) {
                .Vulkan => {
                    // vulkan memory allocator
                    const vma = b.dependency("vulkan_memory_allocator", .{
                        .target = target,
                        .optimize = mode,
                    });
                    // add vma headers
                    t.step.dependOn(vma.builder.getInstallStep());
                    t.addIncludePath(.{ .path = std.fs.path.join(
                        b.allocator,
                        &.{ vma.builder.install_path, "include" },
                    ) catch @panic("OOM") });

                    // add vma static lib
                    var vma_lib = vma.artifact("VulkanMemoryAllocator");
                    t.linkLibrary(vma_lib);

                    // add the VULKAN_SDK to the library search path for VMA
                    const vulkan_sdk = std.process.getEnvVarOwned(b.allocator, "VULKAN_SDK") catch |err| switch (err) {
                        error.OutOfMemory => @panic("OOM"),
                        error.InvalidUtf8 => @panic("VULKAN_SDK invalid environment var name"),
                        error.EnvironmentVariableNotFound => @panic("You have no set the VULKAN_SDK environment variable- usually this means you haven't installed it."),
                    };
                    vma_lib.addLibraryPath(.{ .path = vulkan_sdk });

                    // fmt
                    const fmt = b.dependency("fmt", .{});
                    t.step.dependOn(fmt.builder.getInstallStep());
                    t.addIncludePath(.{
                        .path = std.fs.path.join(b.allocator, &.{ fmt.builder.install_path, "include" }) catch @panic("OOM"),
                    });

                    // link system libraries
                    switch (target.getOsTag()) {
                        .linux => {
                            t.linkSystemLibrary("xcb");
                        },
                        else => {},
                    }
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
    } else {
        // header-only
        const headers = switch (backend) {
            .DX => dx_headers,
            .Vulkan => vulkan_headers,
        };
        for (headers) |h| {
            b.installFile(h, std.fs.path.join(b.allocator, &.{ "include", "wisdom", std.fs.path.basename(h) }) catch @panic("OOM"));
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
