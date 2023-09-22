const std = @import("std");
const builtin = @import("builtin");
const zcc = @import("compile_commands");
const interface = @import("build_interface.zig");

const LogLevel = @import("build_interface.zig").LogLevel;

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});
    const mode = b.standardOptimizeOption(.{});

    const header_only = b.option(bool, "header_only", "Don't compile wisdom- only install headers for inlining. Defaults to false.") orelse false;
    const wayland_support = b.option(bool, "wayland_support", "Whether to add wayland support. Defaults to true on linux, false otherwise.") orelse (target.getOsTag() == .linux);
    const force_vulkan = b.option(bool, "force_vulkan", "Whether to force the use of vulkan. Useful on Windows. Defaults to false.") orelse false;
    const windows_store = b.option(bool, "windows_store", "Whether to build for the windows store. Defaults to false.") orelse false;
    const runtime_asserts = b.option(bool, "runtime_asserts", "Whether to make asserts at runtime. Defaults to true in debug mode, false otherwise.") orelse (mode == .Debug);
    const log_level = b.option(LogLevel, "log_level", "The minimum log level to display.") orelse (if (mode == .Debug) LogLevel.Debug else LogLevel.Critical);
    const build_examples = b.option(bool, "build_examples", "Whether to build examples. Defaults to false.") orelse false;

    const wisdom = interface.buildWisdom(b, interface.WisdomConfiguration{
        .backend = interface.deduceBackend(target, force_vulkan, windows_store),
        .windows_store = windows_store,
        .wayland_support = wayland_support,
        .build_mode = if (!header_only)
            .{ .WithLibs = .{
                .target = target,
                .optimize = mode,
            } }
        else
            .HeaderOnly,
        .log_level = log_level,
        .runtime_asserts = runtime_asserts,
        .debug = mode == .Debug,
    });

    if (wisdom.static_lib) |lib| {
        std.debug.assert(!header_only);
        b.installArtifact(lib);
        if (build_examples) {
            @import("examples/build.zig").build(b, lib);
        }

        // compile commands step
        var targets = std.ArrayList(*std.Build.CompileStep).init(b.allocator);
        targets.append(wisdom.static_lib.?) catch @panic("OOM");
        zcc.createStep(b, "cdb", try targets.toOwnedSlice());
    } else {
        std.debug.assert(header_only);
    }

    if (build_examples and header_only) @panic("examples not set up to build header-only");
}
