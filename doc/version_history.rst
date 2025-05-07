.. py:currentmodule:: lsst.ts.ess.earthquake

.. _lsst.ts.ess.earthquake.version_history:

###############
Version History
###############

v0.1.3
======

* Started looping over all entries to empty the telemetry queue.
  This avoids filling up all available memory.
* Fix version module generation.

Requires:

* ts_ess_common
* ts_salobj (for type checking)

v0.1.2
======

* Fixed a bug with disconnecting from the earthquake device.
* Fixed a bug in the telemetry loop.
* Removed telemetry thread and process telemetry with asyncio.

Requires:

* ts_ess_common
* ts_salobj (for type checking)

v0.1.1
======

* Included sources for lib330 and libmseed instead of pulling them from gitlab.
* Made the C code always build for x86_64 on macOS.
* Made C test application and Python code correctly disconnect from the earthquake device.

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
