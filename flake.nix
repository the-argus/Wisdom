{
  description = "Wisdom build environment";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    kdab-flake.url = "github:the-argus/kdab-flake";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = {
    nixpkgs,
    flake-utils,
    kdab-flake,
    ...
  }: let
    supportedSystems = let
      inherit (flake-utils.lib) system;
    in [
      system.aarch64-linux
      system.x86_64-linux
    ];
  in
    flake-utils.lib.eachSystem supportedSystems (system: let
      pkgs = import nixpkgs {inherit system;};
      vulkan-sdk = kdab-flake.packages.${pkgs.system}.software.vulkan-sdk;
    in {
      devShell = pkgs.mkShell {
        packages = with pkgs; [
          zig_0_11
          xorg.libxcb
          wayland
          libxkbcommon
          vulkan-sdk
        ];
        shellHook = ''
          export VULKAN_SDK="${vulkan-sdk}"
        '';
      };
    });
}
