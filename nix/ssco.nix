{
  lib,
  stdenv,
  fetchFromGitHub,
  cmake,
}:
stdenv.mkDerivation (attrs': {
  pname = "ssco";
  version = "3.0.1";

  src = fetchFromGitHub {
    owner = "donn";
    repo = "ssco";
    rev = "v${attrs'.version}";
    sha256 = "sha256-IxLs3ZkoJx10XPy8mQfcgK4QwvUJjQSaheuXKv+ikEY=";
  };

  nativeBuildInputs = [cmake];

  meta = {
    homepage = "https://github.com/donn/ssco";
    description = "Tiny Commandline Processing Library for C++17";
    license = lib.licenses.mit;
    # maintainers = [ lib.maintainers.donn ];
    platforms = lib.platforms.all;
  };
})
