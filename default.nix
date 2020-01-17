{ pkgs ? (import <nixpkgs> {})
, source ? ./.
, version ? "dev"
} :

with pkgs;

stdenvNoCC.mkDerivation {
    name = "rainbowattack-${version}";
    version = "0.5";
    src = source; # lib.cleanSource source;
    # enableParallelBuilding = true;

    buildInputs = [ clang_7 makeWrapper lld cryptopp ];

    # LD_LIBRARY_PATH = "${cryptopp}/lib"; # useless?

    buildPhase = ''
      mkdir -p build
      make mktable
    '';

    installPhase = ''
      mkdir -p $out
      cp mktable $out/
      # TODO get rid of this... LD_LIBRARY_PATH doesn't help
      patchelf --set-rpath ${cryptopp}/lib $out/mktable
    '';
}
