Building from Source
====================


Prerequisites
-------------

Our own libraries required to build GTlab are bundled with GTlab and will be automatically
downloaded during the CMake configuration phase. Whether to use the bundled libraries can be
adjusted using the CMake variable ``GTLAB_USE_BUNDLED_XXX=ON``.

GTlab comes with optional support for HDF5. To enable, set ``BUILD_WITH_HDF5=ON`` during the config step.

.. list-table:: Dependencies of the GTlab Core Framework
   :widths: 21 13 8 8 50
   :header-rows: 1

   * - Library
     - Version
     - Required
     - Bundled
     - Where to get
   * - Qt
     - 5.15.x 
     - Yes
     - No
     - https://download.qt.io/official_releases/qt/
   * - GtLogging
     - >= 4.4.0
     - Yes
     - Yes
     - https://github.com/dlr-gtlab/gt-logging
   * - GenH5
     - >= 3.3.0 
     - No
     - Yes
     - https://github.com/dlr-gtlab/genh5 
   * - HDF5
     - >= 1.12.0
     - No
     - No
     - https://github.com/HDFGroup/hdf5/releases  



Building
--------

GTlab requires a recent ``CMake`` (>3.15) to build. The configuration and build process is
similar to other CMake builds:

.. code-block:: bash

   cmake -S . -B build -DQt5_DIR=<path/to/cmake/Qt5> -DHDF5_DIR=<path/to/cmake/hdf5> -DCMAKE_INSTALL_PREFIX=<path/to/install>
   cmake --build build
   cmake --build build --target install


In case you own GTlab's development tools, you can simplify CMake's configure step via

.. code-block:: bash
    
   cmake -S . -B build -DGTLAB_DEVTOOLS_DIR=<path/to/devtools> -DQt5_DIR=<path/to/cmake/Qt5> -DCMAKE_INSTALL_PREFIX=<path/to/install>


Build with Conan
----------------

To simplify the dependency management, GTlab can also be built using Conan.

.. code-block:: bash

   conan install . -if build
   cmake -S . -B build -DCMAKE_INSTALL_PREFIX=<path/to/install>
   cmake --build build
   cmake --build build --target install

Conan with Visual Studio and Multi-Config generators
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Note, when using the **Visual Studio** code generator, you might need to specify the 
build configuration. To build a release, the commands change to

.. code-block:: bash

   conan install . -if build -s build_type=Release
   cmake -S . -B build -DCMAKE_INSTALL_PREFIX=<path/to/install>
   cmake --build build --config=Release
   cmake --build build --target install

, and for the debug configuration accordingly.