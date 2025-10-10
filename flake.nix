{
  inputs = {
    nix-eda.url = "github:fossi-foundation/nix-eda";
  };

  outputs = {
    self,
    nix-eda,
    ...
  }: let
    nixpkgs = nix-eda.inputs.nixpkgs;
    lib = nixpkgs.lib;
  in {
    overlays = {
      default = pkgs': pkgs: let
        callPackage = lib.callPackageWith pkgs';
      in {
        scope_guard = callPackage ./nix/scope_guard.nix {};
        ssco = callPackage ./nix/ssco.nix {};
        nudelta = callPackage ./default.nix {src = self;};
      };
    };

    legacyPackages = nix-eda.forAllSystems (
      system:
        import nixpkgs {
          inherit system;
          overlays = [
            self.overlays.default
          ];
        }
    );

    # Outputs
    packages = nix-eda.forAllSystems (
      system: let
        pkgs = self.legacyPackages."${system}";
      in {
        inherit (pkgs) nudelta;
        default = pkgs.nudelta;
      }
    );

    devShells = nix-eda.forAllSystems (
      system: let
        pkgs = self.legacyPackages."${system}";
      in {
        online = pkgs.mkShell {
          buildInputs = with pkgs;
            [
              cmake
              yarn
              pkg-config
            ]
            ++ lib.optionals pkgs.stdenv.isLinux [udev];
        };
      }
    );
  };
}
