const std = @import("std");

const dir = "examples/hello-triangle-kdgui/";

/// Build examples (inherits targets and optimize from wisdom)
pub fn build(b: *std.Build, wisdom: *std.Build.Step.Compile) []*std.Build.Step.Compile {
    var targets = std.ArrayList(*std.Build.Step.Compile).init(b.allocator);
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

        var flags = std.ArrayList([]const u8).init(b.allocator);
        flags.append("-std=c++20") catch @panic("OOM");

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
        }, flags.toOwnedSlice() catch @panic("OOM"));

        targets.append(hello_triangle_kdgui) catch @panic("OOM");
    }

    return targets.toOwnedSlice() catch @panic("OOM");
}
