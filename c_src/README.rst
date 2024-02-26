#########################
Simplified API for lib330
#########################

This directory contains the C source and header files for a simplified API for lib330.
The lib330 source files are publicly available in the src/libsrc/lib330 directory of https://gitlab.com/seismic-software/earthworm
Here and everywhere else this simplified API will be referred to as libq330.

##################
Command line build
##################

In order to be able to build libq330, cmake needs to be installed.
The CMake structure is setup such that it first pulls and compiles the earthworm source code.
Then libq330 is built.

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

In case the build fails indicating that there were undefined symbols while linking the C shared library, set the ``Cmake: Parallel Jobs`` option to 1 and try again.
