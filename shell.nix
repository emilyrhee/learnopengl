{ pkgs ? import (fetchTarball
"https://github.com/NixOS/nixpkgs/tarball/nixos-24.05") { config = {}; overlays
= []; } }:

pkgs.mkShell {
  packages = builtins.attrValues {
    inherit (pkgs)
      cmakeWithGui
      gnumake
      glfw;
  };
  shellHook = ''
    export LD_LIBRARY_PATH="''${LD_LIBRARY_PATH}''${LD_LIBRARY_PATH:+:}${pkgs.libglvnd}/lib"
  '';
}
