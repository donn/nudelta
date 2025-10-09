{
  lib,
  mkShell,
  clangStdenv,
  darwin,
  nodejs,
  yarn,
  cmake,
  gnumake,
  pkg-config,
  electron,
  udev,
}:
(mkShell.override {
  stdenv = clangStdenv;
}) {
  packages =
    [
      nodejs
      yarn
      cmake
      gnumake
      pkg-config
      electron
    ]
    ++ lib.optionals clangStdenv.isLinux [udev];
}
