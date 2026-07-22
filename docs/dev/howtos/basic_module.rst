.. _create-basic-module:

Create a module from scratch
============================

This guide builds the smallest useful GTlab module. It is intended as a
reference for understanding a generated module or for creating a small module
without the module generator. For larger projects, start with the
:ref:`module generator <getting-started>` and use this page to understand the
generated files.

Prerequisites
-------------

You need a GTlab development installation, a compiler compatible with that
installation, CMake, and the Qt version used to build GTlab. Set
``CMAKE_PREFIX_PATH`` or ``GTlab_DIR`` so that CMake can find the installed
``GTlabConfig.cmake``.

The minimal project has three files::

  MyModule/
  |-- CMakeLists.txt
  `-- src/
      |-- mymodule.cpp
      `-- mymodule.h

Configure the module target
---------------------------

Use the CMake integration installed with GTlab. Do not copy ``GTlab.cmake``
into the module repository.

.. code-block:: cmake

  cmake_minimum_required(VERSION 3.15)
  project(MyModule VERSION 0.1.0 LANGUAGES CXX)

  find_package(GTlab REQUIRED)
  gtlab_standard_setup()
  require_qt(COMPONENTS Core)

  add_gtlab_module(MyModule
    MODULE_ID "My Module"
    SOURCES
      src/mymodule.cpp
      src/mymodule.h
  )

  target_link_libraries(MyModule
    PRIVATE
      GTlab::Core
      Qt${QT_VERSION_MAJOR}::Core
  )

``add_gtlab_module`` creates the shared library, defines ``GT_MODULE_ID``, and
installs the library in GTlab's module directory. Treat ``MODULE_ID`` as a
persistent identifier: it identifies settings, log messages, metadata, and
stored module data. Changing it after releasing the module can make existing
installations and projects inconsistent.

Implement the entry point
-------------------------

The entry-point class is a ``QObject`` and implements
:cpp:class:`GtModuleInterface`. ``GT_MODULE()`` adds the Qt plugin metadata and
implements :cpp:func:`GtModuleInterface::ident` from ``GT_MODULE_ID``.

``src/mymodule.h``:

.. code-block:: cpp

  #pragma once

  #include <QObject>

  #include "gt_moduleinterface.h"
  #include "gt_versionnumber.h"

  class MyModule : public QObject, public GtModuleInterface
  {
      Q_OBJECT
      GT_MODULE()

  public:
      GtVersionNumber version() override;
      QString description() const override;
  };

``src/mymodule.cpp``:

.. code-block:: cpp

  #include "mymodule.h"

  GtVersionNumber MyModule::version()
  {
      return {0, 1, 0};
  }

  QString MyModule::description() const
  {
      return QStringLiteral("Adds my domain-specific functionality to GTlab.");
  }

The module needs no ``main()`` function. GTlab discovers the plugin, verifies
its interface, and creates the entry-point object.

Build and install
-----------------

Configure against the GTlab installation and select a writable install prefix:

.. code-block:: console

  cmake -S . -B build \
    -DCMAKE_PREFIX_PATH=/path/to/gtlab \
    -DCMAKE_INSTALL_PREFIX=/path/to/gtlab
  cmake --build build
  cmake --install build

If you cannot install the module into the GTlab installation, choose a separate
prefix and add its module directory under :menuselection:`Edit --> Preferences
--> Module Directories`. For temporary development runs, the
``GTLAB_MODULE_DIRS`` environment variable can contain additional module
directories. Use the platform path-list separator (``:`` on Linux and macOS,
``;`` on Windows).

Restart GTlab after installing a rebuilt module. Confirm that it appears in
the module overview and that its version and description are correct. A quick
check is :menuselection:`Help --> About GTlab --> Modules` or the
``Alt+M`` shortcut, which opens the module overview directly.

Add one capability at a time
----------------------------

The entry point only makes the module loadable. Add further interfaces based
on what the module contributes:

.. list-table:: Common next steps
   :header-rows: 1
   :widths: 35 65

   * - Goal
     - Interface
   * - Store domain objects in projects
     - :ref:`datamodelinterface`
   * - Provide calculators or workflow tasks
     - :ref:`processinterface`
   * - Add object editors, viewers, or dock widgets
     - :ref:`mdiinterface`
   * - Read or write external file formats
     - :ref:`importerinterface` or :ref:`exporterinterface`

For every additional interface:

#. inherit from the interface class;
#. add it to ``Q_INTERFACES(...)``;
#. implement or override the relevant registration functions;
#. link the GTlab component that owns the interface (for example
   ``GTlab::Gui`` for GUI extensions); and
#. return the metadata for each class the module contributes.

Keep the first working increment small. A loadable module plus one registered
object or calculator is easier to diagnose than implementing all extension
points at once.

Troubleshooting
---------------

If the module does not load, check the GTlab output and then verify:

* the module was built against a binary-compatible GTlab and Qt installation;
* all runtime libraries required by the module can be found;
* the plugin is located in an enabled module directory;
* the module appears in :menuselection:`Help --> About GTlab --> Modules`
  or via the ``Alt+M`` shortcut; and
* ``MODULE_ID`` matches the identity expected by the module; and
* every inherited Qt interface is listed in ``Q_INTERFACES(...)``.

An interface-version mismatch means that the module was compiled against an
incompatible GTlab interface. Reconfigure and rebuild it against the GTlab
installation that will load it.
