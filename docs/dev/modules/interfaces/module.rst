.. _moduleinterface:

Module interface
================

:cpp:class:`GtModuleInterface` is the entry point GTlab requires from every
module. The entry-point class is a ``QObject`` and uses ``GT_MODULE()`` to add
the Qt plugin metadata and module identity.

.. code-block:: cpp

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

``GT_MODULE()`` implements :cpp:func:`GtModuleInterface::ident` using the
``GT_MODULE_ID`` compile definition set by ``add_gtlab_module``. Module code
therefore does not implement ``ident()`` itself. The macro can optionally name
a Qt plugin metadata file, for example ``GT_MODULE("mymodule.json")``.

Identity, version, and description
----------------------------------

The module ID is configured once in CMake:

.. code-block:: cmake

  add_gtlab_module(MyModule
    MODULE_ID "My Module"
    SOURCES src/mymodule.cpp src/mymodule.h
  )

Keep this ID stable after releasing the module. GTlab uses it for plugin
identity, settings, logs, metadata locations, and stored module information.
Use semantic versioning for the module version so users can see at a glance
whether a release is backward compatible or requires attention during an
update.

Return a three-part module version and a short, user-facing description:

.. code-block:: cpp

  GtVersionNumber MyModule::version()
  {
      return {1, 2, 0};
  }

  QString MyModule::description() const
  {
      return QStringLiteral("Creates and evaluates example engine models.");
  }

Increment the version whenever distributing changed module binaries. If a
release changes persisted project data, provide explicit upgrade routines so
older projects can be migrated.

User-visible metadata
---------------------

Override ``metaInformation()`` to show author, contact, and short license
information in GTlab's module details. ``add_gtlab_module`` can also install a
README and changelog into the module's metadata directory:

.. code-block:: cmake

  add_gtlab_module(MyModule
    MODULE_ID "My Module"
    README_FILE "${PROJECT_SOURCE_DIR}/README.md"
    CHANGELOG_FILE "${PROJECT_SOURCE_DIR}/CHANGELOG.md"
    SOURCES src/mymodule.cpp src/mymodule.h
  )

These files help users identify the installed module and understand changes
without requiring access to its source repository.
The same metadata mechanism can also be used to install bundled examples, so a
module can ship tutorial content alongside its runtime plugin.

Loading hooks
-------------

The interface provides two optional startup hooks:

``onLoad()``
  Called immediately after GTlab has accepted and registered the plugin. Use
  it only for work that must happen during module loading.

``init()``
  Called later while the application starts. Use it for module initialization
  that depends on GTlab services, such as registering module settings.

Keep both hooks lightweight and deterministic. Avoid creating feature widgets
or opening projects during startup; registered factories and metadata allow
GTlab to create feature objects when users request them.

Module-wide optional features
-----------------------------

:cpp:class:`GtModuleInterface` also offers default-empty registration methods
for cross-module shared functions, batch command-line functions, and project
data upgrade routines. Override only the feature you provide:

* ``sharedFunctions()`` exposes functions without a compile-time dependency
  between modules; see :doc:`../shared_functions`;
* ``commandLineFunctions()`` adds commands to the GTlab batch application; and
* ``upgradeRoutines()`` migrates persisted module data to newer versions.

For a complete minimal project, continue with :ref:`create-basic-module`.
