const std = @import("std");

const NugetDependency = struct {
    // optional runstep (may not be needed if the dependency is already installed)
    runstep: ?*std.Build.Step.Run,
    // path to the cache directory where the nuget dep is installed
    path: []u8,
};

pub fn loadWinRT(b: *std.Build) *std.Build.Step {
    const nuget_executable: []u8 = std.process.getEnvVarOwned(b.allocator, "NUGET_EXE") catch |err| switch (err) {
        error.OutOfMemory => @panic("OOM"),
        error.InvalidUtf8, error.EnvironmentVariableNotFound => block: {
            std.log.warn("Attempted to find the path to nuget.exe using env var NUGET_EXE, but it was not set properly. Assuming nuget is in path...");
            break :block "nuget";
        },
    };

    const nuget_winrt = loadNugetDependency(b, nuget_executable, "Microsoft.Windows.CppWinRT");
    const winrt_out_dir = std.fs.path.join(b.allocator, &.{ b.install_prefix, "nuget" });
    const winrt = b.addSystemCommand(&.{
        std.fs.path.join(b.allocator, &.{ nuget_winrt.path, "bin", "cppwinrt.exe" }),
        "-input",
        "sdk",
        "-output",
        "include",
        b.build_root.path orelse ".",
        winrt_out_dir,
    });

    if (nuget_winrt.runstep) |runstep| winrt.step.dependOn(runstep.step);
    return &winrt.step;
}

fn loadNugetDependency(
    b: *std.Build,
    nuget_executable: []const u8,
    nuget_plugin_name: []const u8,
) NugetDependency {
    const hash_length = 32;
    const hash: [hash_length]u8 = undefined;
    if (!b.cache_root.path) @panic("cache root not defined");
    std.crypto.hash.Blake3.hash(nuget_plugin_name, &hash, .{});
    const dirname = &hash;
    const output_dir = std.fs.path.join(b.allocator, &.{ b.cache_root.path.?, dirname });
    const runstep: ?*std.Build.Step.Run = null;

    const stat_result: ?std.fs.Dir.Stat = b.cache_root.handle.statFile(dirname) orelse null;
    if (stat_result) |result| {
        if (result.kind != .directory) {
            @panic("Cache directory for nuget dep already exists, but it is not a directory. Clear your cache and retry.");
        }
    } else {
        runstep = b.addSystemCommand(&.{
            nuget_executable,
            "install",
            nuget_plugin_name,
            "-OutputDirectory",
            output_dir,
        });
    }

    return .{
        .runstep = runstep,
        .path = output_dir,
    };
}
