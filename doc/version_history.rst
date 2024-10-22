.. py:currentmodule:: lsst.ts.ess.earthquake

.. _lsst.ts.ess.earthquake.version_history:

###############
Version History
###############

v0.1.1
======

* Include sources for lib330 and libmseed instead of pulling them from gitlab.
* Make the C code always build for x86_64 on macOS.
* Make C test application and Python code correctly disconnect from the earthquake device.

Requires:

* ts_ess_common
* ts_salobj (for type checking)

v0.1.0
======

First release of the earthquake sensor code package.

* A simplified C API for interacting with the Q330 C library.
* Python code that uses the simplified C API.
* Handling of the telemetry.
* Use LFS for binary files.
* Update the version of ts-conda-build to 0.4 in the conda recipe.

Requires:

* ts_ess_common
* ts_salobj (for type checking)
