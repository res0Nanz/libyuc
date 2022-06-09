{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/22.05";
    flake-utils.url = "github:numtide/flake-utils/v1.0.0";
  };

  outputs = inputs:
    inputs.flake-utils.lib.eachDefaultSystem (system:
      with import inputs.nixpkgs { inherit system; }; rec {

        packages.default = (stdenv.mkDerivation rec {
          name = "libyuc";
          src = inputs.self;

          # outputs = [ "out" "doc" ]; # possible components: lib bin
          # buildFlags = [ "all" "doc" ];
          cmakeFlags = lib.optional doCheck "-DBUILD_TESTING=1";

          # compile-time dependencies
          nativeBuildInputs = [ cmake doxygen ]
                           ++ lib.optionals doCheck [ gtest gsl ];

          # compile-time dependencies that propagates to downstream packages
          propagatedNativeBuildInputs = [ ];

          # runtime dependencies
          buildInputs = [ ];

          # runtime dependencies that propagates to downstream packages
          propagatedBuildInputs = [ ];

          doCheck = true;

        });

        # Developtment environment for this package
        devShells.default = pkgs.mkShell {
          buildInputs = with packages.default;
            [ gtest ccache gcc gsl lcov ]
            ++ nativeBuildInputs ++ propagatedBuildInputs
            ++ buildInputs ++ propagatedNativeBuildInputs;
        };
      });
}
