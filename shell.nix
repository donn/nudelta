{
    pkgs ? import <nixpkgs> {}
}:

with pkgs; (mkShell.override { stdenv = if stdenv.isDarwin then darwin.apple_sdk_11_0.stdenv else clangStdenv; }) {
    packages = [
        nodejs
        yarn
        cmake
        gnumake
        pkg-config
        electron
    ] ++ (if stdenv.isDarwin then [darwin.apple_sdk_11_0.frameworks.IOKit darwin.apple_sdk_11_0.frameworks.AppKit] else [udev]);
}