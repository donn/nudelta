{
  lib,
  stdenv,
  fetchFromGitHub,
  cmake,
}:
stdenv.mkDerivation (finalAttrs: {
  pname = "scope_guard";
  version = "0.9.1";

  src = fetchFromGitHub {
    owner = "Neargye";
    repo = "scope_guard";
    rev = "v${finalAttrs.version}";
    sha256 = "sha256-Bs8dr5t9LQ6FyM6btpDBAEvyiKoCF2H99RRFAQfx7/I=";
  };

  nativeBuildInputs = [cmake];

  meta = {
    homepage = "https://github.com/neargye/scope_guard";
    description = "Scope Guard & Defer C++11 Library";
    license = lib.licenses.mit;
    # maintainers = [ lib.maintainers.donn ];
    platforms = lib.platforms.all;
  };
})
