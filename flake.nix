{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";
    nixgl.url = "github:guibou/nixGL";
  };

  outputs = inputs @ {flake-parts, ...}:
    flake-parts.lib.mkFlake {inherit inputs;} {
      systems = ["x86_64-linux"];
      perSystem = {
        config,
        self',
        inputs',
        pkgs,
        system,
        ...
      }: {
        _module.args.pkgs = import inputs.nixpkgs {
          inherit system;
          overlays = [inputs.nixgl.overlay];
        };

        formatter = pkgs.alejandra;

        devShells.default =
          pkgs.mkShell.override {
            stdenv = pkgs.clang16Stdenv;
          } {
            buildInputs = [
              ### opengl and such
              pkgs.xorg.libX11
              pkgs.xorg.libXrandr
              pkgs.xorg.libXinerama
              pkgs.xorg.libXcursor
              pkgs.xorg.libXi
              pkgs.libxkbcommon
              pkgs.libGL
              pkgs.libglvnd
              pkgs.freetype

              ### cpp general
              pkgs.llvmPackages_16.libllvm
              pkgs.gdb
              pkgs.cmake
              pkgs.clang-tools_16
              pkgs.nixgl.auto.nixGLDefault
              pkgs.pkg-config
              pkgs.mold

              #### this project specific
              pkgs.just
              pkgs.ninja
              pkgs.meson
            ];

            env = {
              ASAN_SYMBOLIZER_PATH = "${pkgs.llvmPackages_16.libllvm}/bin/llvm-symbolizer";
              CLANGD_PATH = "${pkgs.clang-tools_16}/bin/clangd";
              CXX_LD = "mold";
            };
          };
      };
    };
}
