/// TODO: this is way more complicated than it needs to be because it finds files
/// procedurally. Would be better to just list the shaders you want compiled here
const std = @import("std");

const here = "examples/shaders/";

// directories by full name which should be ignored
const ignore_dirs = &[_][]const u8{
    "zig-cache",
};

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
    std.fs.makeDirAbsolute(shader_output_folder) catch |err| {
        switch (err) {
            error.PathAlreadyExists => {},
            else => {
                std.log.err("Failed to created inteded shader cache at {s} due to {any}", .{ shader_output_folder, err });
                @panic("failed to create shader cache directory");
            },
        }
    };
    defer shader_compilations.deinit();

    const files = getShaderFiles(b.allocator);
    inline for (@typeInfo(@TypeOf(files)).Struct.fields) |field| {
        for (@field(files, field.name).items) |shader| {
            const stem = std.fs.path.stem(shader);
            std.log.debug("got shader stem of {s} to be {s}", .{ shader, stem });

            const version = std.fmt.allocPrint(b.allocator, "-T{s}_6_1", .{field.name}) catch @panic("OOM");
            const output_flag = std.fmt.allocPrint(b.allocator, "-Fo{s}/{s}.spv", .{ shader_output_folder, stem }) catch @panic("OOM");
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

        walk: while (walker_entry) |entry| {
            defer walker_entry = (walker.next() catch |err| {
                std.log.err("failed to walk directory: {any}", .{err});
                @panic("Directory traversal error. Maybe the nested directories go too deep?");
            });

            // determine if entry contains an ignored directory name
            var subslice = entry.path;
            while (subslice.len > 0) {
                for (ignore_dirs) |pattern| {
                    if (std.mem.eql(u8, subslice, pattern)) {
                        continue :walk;
                    }
                }
                subslice = std.fs.path.dirname(subslice) orelse break;
            }

            const ext = std.fs.path.extension(entry.basename);
            if (!std.mem.eql(u8, ext, ".hlsl")) {
                continue;
            }

            // check if the extension *before* hlsl (for example something.ps.hlsl)
            // is one of the names of the fields of the shader struct.
            const second_ext = block: {
                var secondary = std.fs.path.extension(std.fs.path.stem(entry.basename));
                if (secondary.len > 0) {
                    break :block secondary[1..];
                }
                std.log.warn(
                    "no secondary file extension found on file {s}, skipping. consider categorizing with .ps.hlsl or .hs.hlsl, etc.",
                    .{entry.basename},
                );
                continue;
            };

            inline for (@typeInfo(@TypeOf(result)).Struct.fields) |field| {
                if (std.mem.eql(u8, field.name, second_ext)) {
                    var buffer: [1000]u8 = undefined;
                    const fullpath = std.fs.path.join(ally, &.{ (std.fs.cwd().realpath(".", &buffer) catch @panic("no realpath found")), here, entry.path }) catch @panic(
                        "OOM",
                    );
                    @field(result, field.name).append(fullpath) catch @panic("OOM");
                    continue :walk;
                }
            }

            std.log.warn(
                "hlsl file {s} has inner extension {s}, which is not one of the accepted shader types. skipping.",
                .{ entry.path, second_ext },
            );
        }
    }
    inline for (@typeInfo(@TypeOf(result)).Struct.fields) |field| {
        for (@field(result, field.name).items) |shader_path| {
            std.log.debug("shader in field {s} at {s}", .{ field.name, shader_path });
        }
    }
    return result;
}
