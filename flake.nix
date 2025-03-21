{
  inputs.nixpkgs.url = "github:nixos/nixpkgs/nixos-24.11";
  outputs = {
    self,
    nixpkgs,
    ...
  }: let
    lib = nixpkgs.lib;
  in {
    overlays = {
      default = pkgs': pkgs: let
        callPackage = lib.callPackageWith pkgs';
      in {
        nudelta = callPackage ./default.nix {};
      };
    };

    legacyPackages =
      lib.genAttrs [
        "x86_64-linux"
        "aarch64-linux"
        "x86_64-darwin"
        "aarch64-darwin"
      ] (
        system:
          import nixpkgs {
            inherit system;
            overlays = [
              #   self.overlays.default
            ];
          }
      );

    # Outputs
    packages =
      lib.genAttrs [
        "x86_64-linux"
        "aarch64-linux"
        "x86_64-darwin"
        "aarch64-darwin"
      ] (
        system: let
          pkgs = self.legacyPackages."${system}";
        in {
          #   inherit (pkgs) nudelta;
          #   default = pkgs.nudelta;
        }
      );

    devShells =
      lib.genAttrs [
        "x86_64-linux"
        "aarch64-linux"
        "x86_64-darwin"
        "aarch64-darwin"
      ] (
        system: let
          pkgs = self.legacyPackages."${system}";
        in {
          default = (lib.callPackageWith pkgs) ./shell.nix {};
        }
      );
  };
}
