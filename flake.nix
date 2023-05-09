{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";

    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = {
    nixpkgs,
    flake-utils,
    ...
  }: flake-utils.lib.eachDefaultSystem (system: let
    pkgs = nixpkgs.legacyPackages.${system};
  in {
    formatter = pkgs.alejandra;

    devShells.default = pkgs.mkShell.override {
        stdenv = pkgs.clang14Stdenv;
      } {
        packages = with pkgs; [
          xorg.libX11
          xorg.libXrandr
          xorg.libXinerama
          xorg.libXcursor
          xorg.libXi
          libxkbcommon

          pkgconfig

          just
          ninja
          meson
        ];
      };
  });
}
