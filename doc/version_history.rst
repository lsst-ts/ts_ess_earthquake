.. py:currentmodule:: lsst.ts.ess.earthquake

.. _lsst.ts.ess.earthquake.version_history:

###############
Version History
###############

.. towncrier release notes start

v0.3.2 (2026-03-30)
===================

Bug Fixes
---------

- Fixed earthquake sensor telemetry values conversion. (`OSW-2082 <https://rubinobs.atlassian.net//browse/OSW-2082>`_)


v0.3.1 (2026-01-28)
===================

Performance Enhancement
-----------------------

- Set conda build string. (`OSW-982 <https://rubinobs.atlassian.net//browse/OSW-982>`_)
- Updated ts_conda_build dependency version. (`OSW-982 <https://rubinobs.atlassian.net//browse/OSW-982>`_)
- Made code compatible with python 3.13. (`OSW-1754 <https://rubinobs.atlassian.net//browse/OSW-1754>`_)


Other Changes and Additions
---------------------------

- Fixed the documentation build. (`OSW-1754 <https://rubinobs.atlassian.net//browse/OSW-1754>`_)


v0.3.0 (2025-08-19)
===================

Bug Fixes
---------

- Added conversion from raw samples to values in m/s2. (`OSW-882 <https://rubinobs.atlassian.net//browse/OSW-882>`_)


v0.2.0 (2025-06-12)
===================

New Features
------------

- Switched to towncrier and ruff. (`DM-50895 <https://rubinobs.atlassian.net//browse/DM-50895>`_)


Performance Enhancement
-----------------------

- Forced always to reconnect. (`DM-50895 <https://rubinobs.atlassian.net//browse/DM-50895>`_)


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
