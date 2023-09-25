const std = @import("std");

const here = "examples/shaders/";

const Shaders = struct {
    vs: std.ArrayList([]const u8),
    ps: std.ArrayList([]const u8),
    ds: std.ArrayList([]const u8),
    hs: std.ArrayList([]const u8),
    gs: std.ArrayList([]const u8),
    pub fn init(ally: std.mem.Allocator) @This() {
        var res: @This() = undefined;
        res.vs = std.ArrayList([]const u8).init(ally);
        res.ps = std.ArrayList([]const u8).init(ally);
        res.ds = std.ArrayList([]const u8).init(ally);
        res.hs = std.ArrayList([]const u8).init(ally);
        res.gs = std.ArrayList([]const u8).init(ally);
        return res;
    }
};

pub const ShaderStep = struct {
    cache_dir: []const u8,
    install_dir: []const u8,
    install: *std.Build.Step.InstallDir,
};

pub fn build(b: *std.Build, debug: bool, windows_store: bool, win32: bool) ShaderStep {
    if (win32 or windows_store) {
        @panic("not implemented");
    }
    var shader_compilations = std.ArrayList(*std.Build.Step.Run).init(b.allocator);
    if (b.cache_root.path == null) {
        @panic("no cache root set for builder, unable to give dxc/shader compilation a cache directory");
    }
    const shader_output_folder = std.fs.path.join(b.allocator, &.{ b.cache_root.path.?, "wisdom_shaders" }) catch @panic("OOM");
    defer b.allocator.free(shader_output_folder);
    defer shader_compilations.deinit();

    const files = getShaderFiles(b.allocator);
    inline for (@typeInfo(@TypeOf(files)).Struct.fields) |field| {
        for (@field(files, field.name).items) |shader| {

            // call stem twice to get rid of both .ps and .hlsl
            const stem = std.fs.path.stem(std.fs.path.stem(shader));
            std.log.debug("got shader stem of {s} to be {s}", .{ shader, stem });

            const version = std.fmt.allocPrint(b.allocator, "-T{s}_6_1", .{field.name}) catch @panic("OOM");
            const output_flag = std.fmt.allocPrint(b.allocator, "-Fo${s}/{s}.spv", .{ shader_output_folder, stem }) catch @panic("OOM");
            defer b.allocator.free(version);
            defer b.allocator.free(output_flag);

            const dxc_command = b.addSystemCommand(&.{
                "dxc",
                "-Emain",
                version,
                "-Zi",
                if (debug) "-Od" else "-O3",
                "-spirv",
                "-fspv-target-env=vulkan1.3",
                "-I" ++ here ++ "headers/",
                output_flag,
                shader,
            });

            shader_compilations.append(dxc_command) catch @panic("OOM");
        }
    }

    const install = b.addInstallDirectory(.{
        .source_dir = .{ .path = shader_output_folder },
        .install_dir = .{ .custom = "shaders" },
        .install_subdir = "",
    });

    for (shader_compilations.items) |shader_step| {
        install.step.dependOn(&shader_step.step);
    }

    return .{
        .cache_dir = shader_output_folder,
        .install = install,
        .install_dir = std.fs.path.join(b.allocator, &.{ b.install_path, "shaders" }) catch @panic("OOM"),
    };
}

fn getShaderFiles(ally: std.mem.Allocator) Shaders {
    var result = Shaders.init(ally);
    var here_dir = std.fs.cwd().openIterableDir(here, .{}) catch |err| {
        std.log.err("Failed to open examples/shaders directory: {any}", .{err});
        std.os.exit(1);
    };
    defer here_dir.close();

    {
        var walker = here_dir.walk(ally) catch @panic("OOM");
        defer walker.deinit();

        var walker_entry = walker.next() catch |err| {
            std.log.err("failed to walk directory: {any}", .{err});
            @panic("Directory traversal error. Maybe the nested directories go too deep?");
        };
        while (walker_entry) |entry| {
            defer walker_entry = (walker.next() catch |err| {
                std.log.err("failed to walk directory: {any}", .{err});
                @panic("Directory traversal error. Maybe the nested directories go too deep?");
            });

            const ext = std.fs.path.extension(entry.basename);
            if (!std.mem.eql(u8, ext, ".hlsl")) {
                std.log.debug("file {s} has extension {s}, which is not .hlsl", .{ entry.path, ext });
                continue;
            }

            // check if the extension *before* hlsl (for example something.ps.hlsl)
            // is one of the names of the fields of the shader struct.
            const second_ext = std.fs.path.extension(std.fs.path.stem(entry.basename));
            inline for (@typeInfo(@TypeOf(result)).Struct.fields) |field| {
                if (std.mem.eql(u8, field.name, second_ext)) {
                    @field(result, field.name).append(entry.path) catch @panic("OOM");
                }
            }

            std.log.warn(
                "hlsl file {s} has inner extension {s}, which is not one of the accepted shader types. skipping.",
                .{ entry.path, second_ext },
            );
        }
    }
    return result;
}
