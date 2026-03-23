Migration & Deprecation Policy
==============================

GTlab regularly evolves its API to improve usability, performance, and design.
As part of this evolution, functions and types are phased out in a controlled
way so module developers and library users can migrate smoothly.

This guide covers:

- GTlab's deprecation lifecycle,
- Migration to GTlab 2.1,
- Compiler and tooling support for migration.

Deprecation Lifecycle
---------------------

GTlab adheres to a two-step deprecation process:

1. An API is first marked as deprecated and remains available for a couple of
   releases, where it emits warnings.
2. In the removal release (for example 2.1 or 3.0), the API is removed.

This gives module developers multiple release cycles to update their code
before breaking changes occur.

Deprecation Warnings
--------------------

When GTlab deprecates an API, users see warnings like:

    warning: This deprecated function will be removed in GTlab version 2.1: Use newFunction instead

If your compiler or build settings suppress warnings by default, enable warnings
so deprecation messages are visible early (see *Compiler & Tooling Tips*).

Migration to 2.1
----------------

The main changes in GTlab 2.1 are:

- GTlab 2.1 uses **Qt6**,
- module CMake projects should follow the updated GTlab integration pattern,
- selected deprecated APIs are removed.

This section is organized as follows:

1. Start with the migration release in 2.0.x.
2. Apply required API migrations.
3. Update Qt and CMake integration.
4. Validate your project before upgrading.

Quick Start
~~~~~~~~~~~

1. Build with a GTlab 2.0.x migration release (starting at 2.0.12).
2. Resolve all deprecation warnings and replace deprecated APIs.
3. Migrate module CMake projects to the 2.0.12+/2.1 approach.
4. Upgrade to GTlab 2.1 only after the migration is complete.

Migration Release: 2.0.12
~~~~~~~~~~~~~~~~~~~~~~~~~

Library versions in the 2.0.x series, starting with 2.0.12, are migration
releases to prepare projects for GTlab 2.1.

During these releases:

- Deprecated APIs remain available for compilation.
- Using a deprecated API emits a compiler warning.
- Warnings include a replacement suggestion and the planned removal version.
- The build does not fail on deprecated API usage by default.

Required Code Changes
~~~~~~~~~~~~~~~~~~~~~

Start with API migration. For every deprecated API listed below:

1. Identify all usages in your codebase.
2. Replace usages with the recommended alternatives.
3. Rebuild your project and verify warnings are gone.

Deprecated APIs Removed in 2.1
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The tables below list deprecated APIs relevant for migration to GTlab 2.1.

The value in **Since** is the first released git tag in this
repository that already contains the deprecation marker.

Dataprocessor-Library Deprecations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. rst-class:: compact-table

.. list-table::
   :header-rows: 1
   :widths: 40 10 50

   * - **API**
     - **Since**
     - **Replacement / Notes**
   * - ``gt::Finally<Functor>::exec()``
     - 2.0.5
     - Use ``finalize()`` instead.
   * - Global ``findObject(uuid, list)`` overload
     - 2.0.0
     - Use ``gt::findObject(...)`` instead.
   * - Global ``isDerivedFromClass(obj, superClassName)`` overload
     - 2.0.0
     - Use ``gt::isDerivedFromClass(...)`` instead.

Core-Library Deprecations
^^^^^^^^^^^^^^^^^^^^^^^^^

.. rst-class:: compact-table

.. list-table::
   :header-rows: 1
   :widths: 40 10 50

   * - **API**
     - **Since**
     - **Replacement / Notes**
   * - ``GtCoreDatamodel::newProject(project)``
     - 2.0.6
     - Use ``newProject(project, doOpen)`` instead.
   * - ``GtProcessData::createNewTaskGroup(..., projectPath)``
     - 2.0.11
     - Use ``createNewTaskGroup(const QString&, GtTaskGroup::SCOPE)`` instead.
   * - ``gt::moduleSettingPath(modID, settingID)``
     - 2.0.0
     - Use ``gt::makeModuleSettingsPath`` instead.
   * - ``gt::deprecated::processExecutor()``
     - 2.0.0
     - Use ``gt::currentProcessExecutor()`` instead.
   * - ``gt::getUniqueName(...)`` (deprecated helper overloads)
     - 2.0.0
     - Use ``gt::makeUniqueName`` instead.

Gui-Library Deprecations
^^^^^^^^^^^^^^^^^^^^^^^^

.. rst-class:: compact-table

.. list-table::
   :header-rows: 1
   :widths: 40 10 50

   * - **API**
     - **Since**
     - **Replacement / Notes**
   * - ``GtObjectUI::addSingleAction(...)`` overloads (>2 args)
     - 2.0.0
     - .. code-block:: cpp

        addSingleAction(actionText, actionMethod)
            .setIcon(...)
            .setShortcut(...)
            ...
   * - ``GtObjectUI::addActionGroup(...)`` overloads (>1 arg)
     - 2.0.0
     - .. code-block:: cpp

          addActionGroup(groupname)
             << makeSingleAction(...)
             << makeSingleAction(...);
   * - ``GtAbstractPostWidget::addConfigAction(...)`` overloads (>2 args)
     - 2.0.0
     - .. code-block:: cpp

        addConfigAction(actionText, actionMethod)
            .setIcon(...)
            .setVerificationMethod(...)
   * - ``GtObjectUIAction(...)`` legacy constructor
     - 2.0.0
     - Set action properties using dedicated setters (``addSingleAction(...)``, ``.setIcon(...)``, etc.).
   * - ``GtAbstractPostWidget::iconString()`` / ``setIconString(...)``
     - 2.0.0
     - Use ``icon()`` / ``setIcon(const QIcon&)`` instead.
   * - ``GtAbstractWizard``
     - 2.0.0
     - Use ``GtWizard`` instead.
   * - ``GtApplication::icon(path)``
     - 2.0.0
     - Use ``gt::gui::icon`` instead.
   * - Some colors in ``gt_colors.h``
     - 2.0.0
     - See ``gt::gui::colors`` namespace and deprecation messages for alternatives.
   * - Icons **with size prefix** in ``gt_icons.h``, like ``gt::gui::icon::open16``
     - 2.0.0
     - Use the icon name without size prefix, e.g. ``gt::gui::icon::open``. Size is now specified at usage time.
   * - Outdated stylesheets in ``gt_stylesheets.h``
     - 2.0.0
     - See ``gt::gui::stylesheet`` namespace and deprecation messages for alternatives.
   * - ``GtShortCuts::initialize(const QMap<QString, QStringList>& tab)``
     - 2.0.0
     - Replace with ``initialize(const QList<GtShortCutSettingsData>& list)``
   * - ``GtShortCuts::emitChange()``
     - 2.0.0
     - Replace with ``emit x->changed()``.

----

Qt6 Migration
~~~~~~~~~~~~~

- Replace direct ``find_package(Qt5 ...)`` / ``find_package(Qt6 ...)`` with ``require_qt(COMPONENTS ...)``.
- ``require_qt`` selects the Qt major version GTlab was compiled with.
- For GTlab 2.1, this means Qt 6.
- Link Qt libraries with version-agnostic targets such as ``Qt${QT_VERSION_MAJOR}::Core``.
- See :ref:`CMake migration example <cmake-migration-example>` for a complete CMake example.

**Regex API note:** Prefer ``gt::rex::*`` over the deprecated ``gt::re::*``
regular-expression helpers; ``gt::rex`` is Qt6-compatible.
``gt::re::*`` will be removed in GTlab 2.2.

----

CMake Project Migration for Modules
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In GTlab 2.0, module projects often copied ``GTlab.cmake`` from the GTlab
source tree and included it manually.

Starting with the 2.0.12 migration release, ``GTlab.cmake`` is provided by the
installed GTlab package and loaded automatically after:

.. code-block:: cmake

  find_package(GTlab REQUIRED)

**Required changes**

- Do not copy ``GTlab.cmake`` into your module project.
- Do not include a local ``GTlab.cmake`` file.
- Use helper macros/functions exposed by ``find_package(GTlab REQUIRED)``.

**GTlab.cmake helpers**

.. list-table::
   :header-rows: 1
   :widths: 35 65

   * - **Name**
     - **Purpose**
   * - ``gtlab_standard_setup()``
     - Applies GTlab build defaults (C++ standard, output/install dirs, debug postfix, automoc).
   * - ``add_gtlab_module(target ...)``
     - Creates and installs a GTlab module target and sets ``GT_MODULE_ID``.
   * - ``require_qt(COMPONENTS ...)``
     - Replaces direct ``find_package(Qt5/Qt6 ...)`` calls and finds the Qt version GTlab was built with.
   * - ``enable_gtlab_devtools()``
     - Deprecated helper; use ``find_package(GTlabDevtools QUIET)`` instead.

.. _cmake-migration-example:

**Minimal migration example**

The old example below reflects common 2.0 module setup patterns. The new
example shows the 2.0.12+/2.1-compatible approach.

Old pattern (2.0):

.. code-block:: cmake

  # avoid in 2.0.12+ and 2.1
  include(path/to/copied/GTlab.cmake)
  gtlab_standard_setup()
  enable_gtlab_devtools()

  find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

New pattern (2.0.12+ / 2.1):

.. code-block:: cmake

  # search for devtools (optional)
  find_package(GTlabDevtools QUIET)

  find_package(GTlab REQUIRED)

  gtlab_standard_setup()
  # use the Qt version that GTlab was built with
  require_qt(COMPONENTS Core Widgets)

  add_gtlab_module(MyModule
    MODULE_ID "My Module"
    SOURCES
      src/mymodule.cpp
      src/mymodule.h
  )

  # Qt version agnostic linking
  target_link_libraries(MyModule
    PRIVATE
      Qt${QT_VERSION_MAJOR}::Core
      Qt${QT_VERSION_MAJOR}::Widgets
  )

----

Upgrade Checklist
^^^^^^^^^^^^^^^^^

Before upgrading to GTlab 2.1:

- Remove all calls to deprecated functions.
- Replace all deprecated types with modern counterparts.

Compiler & Tooling Tips
-----------------------

Enforcing Deprecation Warnings
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Some compilers or build setups suppress warnings by default. To ensure warnings
are visible:

- **GCC / Clang / AppleClang**
  Enable warnings with:

      -Wall -Wextra -Wdeprecated-declarations

- **MSVC**
  Ensure warning level is not too low (e.g. ``/W4``), and enable:

      /w14996

If your build treats warnings as errors (``-Werror`` or similar), enabling this
during migration can catch issues early.

Strict Mode
~~~~~~~~~~~

During migration releases, you may want to treat deprecation warnings as errors.
Add this to your CMake target:

.. code-block:: cmake

  target_compile_definitions(my_target PRIVATE GT_STRICT_DEPRECATIONS=1)

This makes deprecated API usage a compile-time error, even during migration
releases.
