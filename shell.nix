{ pkgs ? import <nixpkgs> { } }:
pkgs.mkShell rec {
  buildInputs = with pkgs; [
    gnumake
    avrdude
    screen
    pkgs.pkgsCross.avr.buildPackages.gcc
  ];
}
