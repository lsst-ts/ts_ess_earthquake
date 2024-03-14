#################
ts_ess_earthquake
#################

``ts_ess_earthquake`` is a package in the `LSST Science Pipelines <https://pipelines.lsst.io>`_.

This package contains code to read telemetry from an earthquake sensor.

A Linux dynamically linked library libq330.so is included, as well as C source code and a CMake file to build it.
No macOS dynamically linked library is included.
The CMake file can be used to manually build libq330 on macOS if needed.
For more info, see the README in the `c_src` directory.
