pkgs:
pkgs.mkShell {
  name = "Poincare";
  packages = with pkgs; [
    nixd
    alejandra
    statix
    deadnix
    just
		clang-tools
		clang
  ];
}
