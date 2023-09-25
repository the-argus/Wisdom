const std = @import("std");

const dir = "examples/hello-triangle-kdgui/";

/// Build examples (inherits targets and optimize from wisdom)
pub fn build(
    b: *std.Build,
    wisdom: *std.Build.Step.Compile,
    flags: []const []const u8,
) []*std.Build.Step.Compile {
    var targets = std.ArrayList(*std.Build.Step.Compile).init(b.allocator);

    //TODO: get these from wisdom
    const windows_store = false;
    const win32 = false;

    // build and install shaders
    const shaders = @import("shaders/build.zig").build(b, wisdom.optimize == .Debug, windows_store, win32);
    b.getInstallStep().dependOn(&shaders.install.step);

    // hello_triangle_kdgui
    {
        const hello_triangle_kdgui = b.addExecutable(.{
            .target = wisdom.target,
            .optimize = wisdom.optimize,
            .name = "example_hello_triangle_kdgui",
        });

        hello_triangle_kdgui.linkLibC();
        hello_triangle_kdgui.linkLibCpp();
        hello_triangle_kdgui.linkLibrary(wisdom);

        // TODO: avoid having to do this...
        for (wisdom.include_dirs.items) |include| {
            switch (include) {
                .path => |lazypath| {
                    hello_triangle_kdgui.addIncludePath(.{ .path = lazypath.path });
                },
                else => {},
            }
        }

        var local_flags = std.ArrayList([]const u8).init(b.allocator);
        local_flags.append("-std=c++20") catch @panic("OOM");
        local_flags.appendSlice(flags) catch @panic("OOM");
        local_flags.append(std.fmt.allocPrint(b.allocator, "-DSHADER_DIR=\"{s}\"", .{shaders.install_dir}) catch @panic("OOM")) catch @panic("OOM");

        switch (wisdom.target.getOsTag()) {
            .linux => {
                hello_triangle_kdgui.linkSystemLibrary("m");
            },
            else => {},
        }

        hello_triangle_kdgui.addCSourceFiles(&.{
            dir ++ "app.cpp",
            dir ++ "entry_main.cpp",
            dir ++ "window.cpp",
        }, local_flags.toOwnedSlice() catch @panic("OOM"));

        hello_triangle_kdgui.addIncludePath(.{ .path = dir });

        const kdgui = b.dependency("kdutils", .{}).artifact("KDGui");
        // TODO: probably don't need to actually link kdfoundation here. would be
        // better to just include its headers?
        const kdfoundation = b.dependency("kdutils", .{}).artifact("KDFoundation");
        const glm = b.dependency("glm", .{});

        // glm is header-only
        hello_triangle_kdgui.step.dependOn(glm.builder.getInstallStep());
        hello_triangle_kdgui.addIncludePath(.{ .path = std.fs.path.join(
            b.allocator,
            &.{ glm.builder.install_path, "include" },
        ) catch @panic("OOM") });
        hello_triangle_kdgui.linkLibrary(kdgui);
        hello_triangle_kdgui.linkLibrary(kdfoundation);

        targets.append(hello_triangle_kdgui) catch @panic("OOM");
    }

    return targets.toOwnedSlice() catch @panic("OOM");
}
