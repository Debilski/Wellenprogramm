# Wellenprogramm

(Written to run simulations of reaction–diffusion systems for my diploma thesis.)

For historical reasons only. Slightly updated to use current libraries.

# Prerequisites

    * Qt 4.x (tested with 4.8)
    * Qwt 5.2 (will be downloaded)
    * CMake 2.8 or later
    * blitz++
    * fftw3, fftw3-threads
    * Boost (system, threads)

# Build info

    # create build directory
    $ mkdir build

    # init CMake
    $ cmake ..

    # build it
    $ make -j

    # copy Qwt shared library
    $ cp external/Qwt/src/Qwt-build/lib/libqwt.5.2.4.dylib libqwt.5.dylib

# Run

    $ ./src/ui/plot
   