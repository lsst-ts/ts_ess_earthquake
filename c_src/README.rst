#########################
Simplified API for lib330
#########################

This directory contains the C source and header files for a simplified API for lib330.
The lib330 and libmseed source files are publicly available in the src/libsrc/lib330 directory of the earthworm_ gitlab repo.
For this project the source files for tag v7.10_ have been pulled in for convenience.
Here and everywhere else this simplified API will be referred to as libq330.

.. _earthworm: https://gitlab.com/seismic-software/earthworm
.. _v7.10: https://gitlab.com/seismic-software/earthworm/-/tags/v7.10

##################
Command line build
##################

In order to be able to build libq330, cmake needs to be installed.
The CMake structure is setup such that it compiles the libq330 simplefied API code using the lib330 and libmseed code.

The steps to build the dynamically linked library on the command line are then

.. code-block:: bash

    cd <ts_ess_earthquake clone path>/c_src
    mkdir build
    cd build
    cmake ..
    cmake --build .
    cmake --install .

This will install the dynamically linked library into the <ts_ess_earthquake clone path>/python/lsst/ts/ess/earthquake/data/libq330 directory.
On Linux this will create the file libq330.so.
On macOS this will create the file libq330.dylib.

Note that the target platform for TSSW projects is Linux.
Therefore the libq330.dylib is included in the project .gitignore file and should not be pushed to GitHub.

#############
CLion Support
#############

The project is designed to be loaded in CLion.
CLion uses ninja instead of make but for this simple project that doesn't make a difference.
When changes to the `CMakeLists.txt` file are made and CLion doesn't automatically pick them up, right click the project name and choose `Reload CMake Project`.
Note that removing the `cmake-build-debug` directory requires right clicking the project and choosing `Reload CMake Project` as well since CLion doesn't automatically do that.

##########################
Visual Studio Code support
##########################

The C code and CMake file can be loaded in Visual Studio Code as well.
First the following plugins need to be installed in Visual Studio Code:

    * C/C++
    * CMake Tools
    * C/C++ Extension Pack (optional)

Then the c_src folder can be opened in Visual Studio Code.
Using the CMake build infrastructure, the project can be built.
