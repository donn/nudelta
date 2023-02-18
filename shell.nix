{
    pkgs ? import <nixpkgs> {}
}:

with pkgs; (mkShell.override { stdenv = clangStdenv; }) {
    packages = [
        nodejs
        yarn
        cmake
        gnumake
        libusb
        pkg-config
        electron
    ];
}