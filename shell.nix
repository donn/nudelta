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
  stdenv =
    if clangStdenv.isDarwin
    then darwin.apple_sdk_11_0.stdenv
    else clangStdenv;
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
    ++ lib.optionals clangStdenv.isDarwin [darwin.apple_sdk_11_0.frameworks.IOKit darwin.apple_sdk_11_0.frameworks.AppKit]
    ++ lib.optionals clangStdenv.isLinux [udev];
}
