{
  src ? ./.,
  lib,
  nodejs,
  electron,
  yarnConfigHook,
  yarnBuildHook,
  fetchYarnDeps,
  fetchFromGitHub,
  cmake,
  clangStdenv,
  zip,
  pkg-config,
  udev,
  fmt_11,
  yaml-cpp,
  hidapi,
}:
clangStdenv.mkDerivation {
  name = "nudelta";

  inherit src;

  nativeBuildInputs = [
    yarnConfigHook
    yarnBuildHook
    cmake
    pkg-config
  ];
  
  buildInputs = [fmt_11 yaml-cpp hidapi] ++ lib.optionals clangStdenv.isLinux [udev];

  dontUseCmakeConfigure = true;

  packageJSON = ./package.json;
  yarnLock = ./yarn.lock;

  offlineCache = fetchYarnDeps {
    yarnLock = ./yarn.lock;
    hash = "sha256-SyPGcKRaiA7OjS6nAflY0+jqoBn8YycdHPvnHkvlHsk=";
  };

  env = {
    ELECTRON_SKIP_BINARY_DOWNLOAD = "1";
    NIX_CFLAGS_COMPILE = "-I${nodejs.dev}/include/node";
  };

  preBuild = ''
    substituteInPlace node_modules/cmake-js/lib/dist.js \
        --replace-fail '!this.downloaded' 'false'
    cp -R ${electron.dist} electron_dist
    chmod -R u+w electron_dist
  '';

  buildPhase = ''
    runHook preBuild
    yarn build-native
    yarn build-ui
    yarn electron-builder \
        --dir \
        -c.electronDist=electron_dist \
        -c.electronVersion=${electron.version}
    runHook postBuild
  '';

  installPhase =
    ''
      runHook preInstall
      mkdir -p $out/bin
      cp -r build/nudelta $out/bin/nudelta-cli
    ''
    + lib.optionalString clangStdenv.isDarwin ''
      mkdir -p $out/Applications
      cp -r dist/mac-*/nudelta.app $out/Applications
      ln -s $out/Applications/nudelta.app/Contents/MacOS/nudelta $out/bin/nudelta
    ''
    + lib.optionalString clangStdenv.isLinux ''
      mkdir -p "$out/share/nudelta"
      cp -r pack/*-unpacked/{locales,resources{,.pak}} "$out/share/nudelta"

      pushd assets/generated/icons/png
      for file in *.png; do
        install -Dm0644 $file $out/share/icons/hicolor/''${file//.png}/apps/nudelta.png
      done
      popd
    '';

  postFixup = lib.optionalString clangStdenv.isLinux ''
    makeWrapper ${electron}/bin/electron $out/bin/nudelta \
      --add-flags $out/share/nudelta/resources/app.asar \
      --add-flags "\''${NIXOS_OZONE_WL:+\''${WAYLAND_DISPLAY:+--ozone-platform-hint=auto --enable-features=WaylandWindowDecorations --enable-wayland-ime=true --wayland-text-input-version=3}}" \
      --set-default ELECTRON_FORCE_IS_PACKAGED 1 \
      --set-default ELECTRON_IS_DEV 0 \
      --inherit-argv0
  '';

  meta = {
    license = lib.licenses.gpl3Plus;
    mainProgram = "nudelta";
  };
}
