{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";

    flake-utils.url = "github:numtide/flake-utils";

    nixgl.url = "github:guibou/nixGL";
  };

  outputs = {
    nixpkgs,
    flake-utils,
    ...
  } @ inputs:
    flake-utils.lib.eachDefaultSystem (system: let
      pkgs = import nixpkgs {
        inherit system;
        overlays = [ inputs.nixgl.overlay ];
      };
    in {
      formatter = pkgs.alejandra;

      devShells.default = with pkgs;
        mkShell.override {
          stdenv = lowPrio llvmPackages_15.stdenv;
        } {
          buildInputs = [
            ### opengl and such
            xorg.libX11
            xorg.libXrandr
            xorg.libXinerama
            xorg.libXcursor
            xorg.libXi
            libxkbcommon
            libGL
            libglvnd

            ### cpp general
            clang-tools_15
            nixgl.auto.nixGLDefault
            pkgconfig
            mold

            #### this project specific
            just
            ninja
            meson
          ];

          env = {
            CLANGD_PATH = "${pkgs.clang-tools_15}/bin/clangd";
            CXX_LD = "mold";
          };
        };

    });
}
