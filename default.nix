{
  src ? ./.,
  lib,
  yarnConfigHook,
  yarnBuildHook,
  fetchYarnDeps,
  fetchFromGitHub,
  cmake,
  pkg-config,
  ruby,
  makeWrapper,
  clangStdenv,
  nodejs,
  electron,
  fmt_11,
  yaml-cpp,
  hidapi,
  ssco,
  scope_guard,
}:
clangStdenv.mkDerivation {
  name = "nudelta";

  inherit src;

  nativeBuildInputs = [
    yarnConfigHook
    cmake
    pkg-config
    ruby
    makeWrapper
  ];

  buildInputs = [fmt_11 yaml-cpp hidapi ssco scope_guard];

  dontUseCmakeConfigure = true;

  packageJSON = ./package.json;
  yarnLock = ./yarn.lock;

  offlineCache = fetchYarnDeps {
    yarnLock = ./yarn.lock;
    hash = "sha256-SyPGcKRaiA7OjS6nAflY0+jqoBn8YycdHPvnHkvlHsk=";
  };

  env =
    {
      NIX_CFLAGS_COMPILE = "-I${nodejs.dev}/include/node -I${hidapi}/include/hidapi";
    }
    // lib.optionalAttrs clangStdenv.isDarwin {
      NIX_LDFLAGS = "-framework IOKit";
    };

  buildPhase = ''
    runHook preBuild

    substituteInPlace node_modules/cmake-js/lib/dist.js \
        --replace-fail '!this.downloaded' 'false'
    cp -R ${electron.dist} electron_dist
    chmod -R u+w electron_dist

    substituteInPlace  node_modules/.bin/rollup \
      --replace-fail "#!/usr/bin/env node" "#!${nodejs}/bin/node"
    substituteInPlace  node_modules/.bin/electron-builder \
      --replace-fail "#!/usr/bin/env node" "#!${nodejs}/bin/node"

    yarn build_native
    yarn run rollup -c
    yarn run electron-builder \
        --dir \
        -c.electronDist=electron_dist \
        -c.electronVersion=${electron.version}
    runHook postBuild
  '';

  installPhase =
    ''
      runHook preInstall
      mkdir -p $out/bin
      cp -r build/nudelta-cli $out/bin/nudelta-cli
    ''
    + lib.optionalString clangStdenv.isDarwin ''
      mkdir -p $out/Applications
      cp -r dist/mac-*/nudelta.app $out/Applications
    ''
    + lib.optionalString clangStdenv.isLinux ''
      mkdir -p "$out/share/nudelta"
      cp -r dist/*-unpacked/{locales,resources{,.pak}} "$out/share/nudelta"
    '';

  postFixup =
    lib.optionalString clangStdenv.isDarwin ''
      makeWrapper $out/Applications/nudelta.app/Contents/MacOS/nudelta $out/bin/nudelta
    ''
    + lib.optionalString clangStdenv.isLinux ''
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
