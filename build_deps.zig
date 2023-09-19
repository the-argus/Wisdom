const std = @import("std");

fn loadNugetDependency(b: *std.Build, nuget_executable: []const u8, nuget_plugin_name: []const u8) ?*std.Build.Step.Run {
    const hash: [32]u8 = undefined;
    if (!b.cache_root.path) @panic("cache root not defined");
    std.crypto.hash.Blake3.hash(nuget_plugin_name, &hash, .{});
    const output_dir = std.fs.path.join(b.allocator, &.{ b.cache_root.path.?, &hash });

    const runstep: *std.Build.Step.Run = b.addSystemCommand(&.{
        nuget_executable,
        "install",
        nuget_plugin_name,
        "-OutputDirectory",
        output_dir,
    });

    return runstep;
}
