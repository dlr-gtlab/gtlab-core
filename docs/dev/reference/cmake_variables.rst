CMake configuration
===================

Common CMake options for building GTlab Core and external modules. Pass
options to CMake using ``-DVARIABLE=value``.

Build options
-------------

.. list-table::
   :header-rows: 1
   :widths: 34 13 53

   * - Variable
     - Default
     - Purpose
   * - ``CMAKE_PREFIX_PATH``
     - Unset
     - Search path for dependencies, including an installed GTlab.
   * - ``BUILD_WITH_HDF5``
     - ``ON``
     - Enable HDF5 support in GTlab Core.
   * - ``BUILD_UNITTESTS``
     - ``OFF``
     - Build Core unit tests.
   * - ``BUILD_TESTMODULES``
     - ``OFF``
     - Build the test modules.
   * - ``GTLAB_USE_BUNDLED_LOGGING``
     - ``ON``
     - Download and build GTlab Logging instead of using an external installation.
   * - ``GTLAB_USE_BUNDLED_GENH5``
     - ``ON``
     - Download and build GenH5 when HDF5 support is enabled.

Installation paths
------------------

.. list-table::
   :header-rows: 1
   :widths: 34 24 42

   * - Variable
     - Default
     - Purpose
   * - ``CMAKE_INSTALL_PREFIX``
     - CMake default
     - Installation root for the current project.
   * - ``CMAKE_INSTALL_BINDIR``
     - Usually ``bin``
     - Standard executable directory (GNUInstallDirs).
   * - ``CMAKE_INSTALL_LIBDIR``
     - Platform-dependent
     - Standard library directory (e.g. ``lib`` or ``lib64``).
   * - ``GTLAB_INSTALL_BINDIR``
     - Core's ``CMAKE_INSTALL_BINDIR``
     - Release runtime directory.
   * - ``GTLAB_INSTALL_DEBUG_BINDIR``
     - ``binDebug``
     - Debug runtime directory.
   * - ``GTLAB_INSTALL_LIBDIR``
     - Core's ``CMAKE_INSTALL_LIBDIR``
     - Directory containing installed GTlab libraries.
   * - ``GTLAB_INSTALL_RUNTIME_DIR``
     - Release/Debug runtime directory
     - Override to use one custom runtime directory for both configurations.
   * - ``GTLAB_INSTALL_MODULEDIR``
     - ``bin/modules`` (Release), ``binDebug/modules`` (Debug)
     - Directory where modules are installed.
   * - ``GTLAB_BUILD_MODULEDIR``
     - ``${CMAKE_BINARY_DIR}/modules``
     - Directory where ``add_gtlab_module()`` places module binaries.

An installed GTlab package exports its own ``GTLAB_INSTALL_*`` defaults to
external module projects without changing their ``CMAKE_INSTALL_*`` values.
Set a ``GTLAB_INSTALL_*`` override **before** ``find_package(GTlab)``.
Use the matching install prefix when installing modules alongside GTlab.

Library search paths (Linux and macOS)
--------------------------------------

.. list-table::
   :header-rows: 1
   :widths: 34 13 53

   * - Variable
     - Default
     - Purpose
   * - ``CMAKE_INSTALL_RPATH``
     - Empty
     - Specify a custom install RPATH.
   * - ``CMAKE_SKIP_INSTALL_RPATH``
     - ``OFF``
     - Disable installation RPATHs.
   * - ``CMAKE_SKIP_RPATH``
     - ``OFF``
     - Disable RPATHs in build and installed binaries.

By default, GTlab generates relative install RPATHs so modules can find GTlab
libraries and adjacent shared dependencies. An explicit target
``INSTALL_RPATH`` property or ``CMAKE_INSTALL_RPATH`` overrides this default.
