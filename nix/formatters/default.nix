_: {
  projectRootFile = "flake.nix";
  programs = {
    nixfmt.enable = true;
    deadnix.enable = true;
		clang-format.enable = true;
    shfmt = {
      enable = true;
      indent_size = 4;
    };
  };
}
