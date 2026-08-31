Migration to GTlab 2.1
======================

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

.. note::
    Consider using preprocessor-macros for version dependent code:

    .. code-block:: cpp

        #if GT_VERSION < GT_VERSION_CHECK(2, 1, 0)
            grid->setHorizontalRuler(hruler);
            grid->setVerticalRuler(vruler);
        #else
            // nothing to do here
        #endif

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

GUI-Library Deprecations
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
   * - Icons **with size suffix** in ``gt_icons.h``, like ``gt::gui::icon::open16``
     - 2.0.0
     - Use the icon name without size suffix, e.g. ``gt::gui::icon::open``. Size is now specified at usage time.
   * - Outdated stylesheets in ``gt_stylesheets.h``
     - 2.0.0
     - See ``gt::gui::stylesheet`` namespace and deprecation messages for alternatives.
   * - ``GtShortCuts::initialize(const QMap<QString, QStringList>& tab)``
     - 2.0.0
     - Replace with ``initialize(const QList<GtShortCutSettingsData>& list)``
   * - ``GtShortCuts::emitChange()``
     - 2.0.0
     - Replace with ``emit x->changed()``.

Graphics API
""""""""""""

The API of ``GtGraphicsScene``, ``GtGraphicsView``, ``GtGrid``, and ``GtRuler`` received significant updates. The resulting API changes have been summarized here:

API-breaking Changes
********************

.. rst-class:: compact-table

.. list-table::
   :header-rows: 1
   :widths: 40 10 50

   * - **Removed API**
     - **Since**
     - **Replacement / Notes**
   * - Removed ``GtGraphicsView::repaintRuler()``
     - 2.1.0
     - Rulers are now self-drawing. Use ``GtGraphicsView::connectHorizontalRuler`` / ``connectVerticalRuler(ruler)`` or use ``GtRuler::invalidate()`` to force repainting of the ruler if necessary.
   * - Removed ``GtGrid::setShowGridPoints(show)``
     - 2.1.0
     - No replacement. Did not provide any functionality.
   * - Removed ``GtGrid::setGridPointColor(color)``
     - 2.1.0
     - No replacement. Did not provide any functionality.
   * - Removed ``GtGrid::setHorizontalRuler`` / ``setVerticalRuler(ruler)``
     - 2.1.0
     - Use ``GtGraphicsView::connectHorizontalRuler`` / ``GtGraphicsView::connectVerticalRuler(ruler)``.
   * - Removed ``GtGrid::paintRuler(ruler)``
     - 2.1.0
     - Rulers are now self-drawing. Use ``GtGraphicsView::connectHorizontalRuler`` / ``connectVerticalRuler(ruler)`` or use ``GtRuler::paint(...)`` instead.
   * - Removed ``GtRuler::buffer()`` (returned ``QImage``)
     - 2.1.0
     - Replaced with non-public ``GtRuler::cache()`` method (returns ``QPixmap``).

Deprecated API
**************

.. rst-class:: compact-table

.. list-table::
   :header-rows: 1
   :widths: 40 10 50

   * - **Deprecated API**
     - **Since**
     - **Replacement / Notes**
   * - ``GtGraphicsScene``
     - 2.1.0
     - Use ``QGraphicsScene`` directly.
   * - ``GtGraphicsScene::findItems<T>()``
     - 2.1.0
     - Use ``gt::gui::findGraphicsItems<T>()``.
   * - ``GtGraphicsView::setHorizontalRuler`` / ``setVerticalRuler(ruler)``
     - 2.1.0
     - Use ``GtGraphicsView::connectHorizontalRuler`` / ``connectVerticalRuler(ruler)``. Do not take ownership.
   * - ``GtGraphicsView::setScale(double)``
     - 2.1.0
     - Use ``zoomBy(double)``.
   * - ``GtGraphicsView::setScalePercentage(double)``
     - 2.1.0
     - Use ``zoomByPercentage(double)``.
   * - ``GtGrid(QGraphicsView&)``
     - 2.1.0
     - Use ``GtGrid(QObject* parent)`` and ``GtGraphicsView::setGrid(GtGrid*)``.
   * - ``GtGrid::setGridWidth(int)`` / ``setGridHeight(int)``
     - 2.1.0
     - Use ``setHSpacing(unsigned)`` / ``setVSpacing(unsigned)`` or ``setSpacing(unsigned)`` for both.
   * - ``GtGrid::setScaleGrid(bool)``
     - 2.1.0
     - Use ``setScalingStrategy(ScalingStrategy)``. ``GtGrid::ScalingStrategy::Base2`` replicates the old default.
   * - ``GtGrid::hideGrid(bool)`` / ``showGrid(bool)``
     - 2.1.0
     - Use ``GtGrid::enableGrid(bool)`` to enable or disable the grid and ``GtGrid::setVisible(bool)`` to control the visibility of the object itself.
   * - ``GtGrid::setShowAxis(bool)``
     - 2.1.0
     - Use ``GtGrid::setActiveAxis(ActiveAxis)`` to set which axis should be enabled.
   * - ``GtGrid::paintGrid(QPainter*, QRectF)``
     - 2.1.0
     - Use ``paint(QPainter&, QRectF, PaintOptions)``.
   * - ``GtGrid::paintGridLines(QPainter*, QRectF)``
     - 2.1.0
     - Use ``paint()`` with ``PaintGrid`` option.
   * - ``GtGrid::paintAxis(QPainter*, QRectF)``
     - 2.1.0
     - Use ``paint()`` with ``PaintAxis`` option.
   * - ``GtGrid::getScaledGridWidth()`` / ``getScaledGridHeight()``
     - 2.1.0
     - Use ``scaledGridSpacing().hSpacing`` / ``scaledGridSpacing().vSpacing``.
   * - ``GtGrid::update`` signal
     - 2.1.0
     - Use ``GtGrid::updated`` signal.
   * - ``GtRuler()`` constructor
     - 2.1.0
     - Use ``GtRuler(orientation, parent)``.
   * - ``GtRuler::needsRepaint()``
     - 2.1.0
     - Use ``needsRepaint(QRectF, QTransform)``.
   * - ``GtRuler::setNeedsRepaint()``
     - 2.1.0
     - Use ``GtRuler::invalidate()``.
   * - ``GtRuler::getFont()``
     - 2.1.0
     - Use ``GtRuler::font()``.
   * - ``GtRuler::getFontSizeHint(QString)``
     - 2.1.0
     - Use ``GtRuler::textSizeHint(QString)``.

Migration Steps
***************

1. Replace ``new GtGrid(view)`` with ``new GtGrid(parent)`` and use
   ``GtGraphicsView::setGrid(GtGrid*)`` to transfer ownership.
2. Replace ``GtGraphicsView::setHorizontalRuler``/``setVerticalRuler`` with
   ``GtGraphicsView::connectHorizontalRuler``/``connectVerticalRuler`` (no ownership taken).
   Remove calls to ``GtGrid::setHorizontalRuler``/``setVerticalRuler``.
3. Replace ``GtGraphicsScene`` with ``QGraphicsScene``.
4. Update grid scaling API: ``setScaleGrid(bool)`` -> ``setScalingStrategy``.
5. Update ruler constructor: ``GtRuler()`` -> ``GtRuler(Qt::Orientation)``.

**Notes on the Ownership Model**

The ownership model for ``GtGrid`` has been clarified:

- **Old**: ``GtGrid`` was constructed with a ``QGraphicsView&`` reference and
  the grid was managed internally with unclear ownership semantics.
- **New**: ``GtGrid`` uses standard Qt parent-child ownership.
  ``GtGraphicsView::setGrid(GtGrid*)`` takes ownership. The grid is no longer fixed to ``GtGraphicsView``.

**Notes on Grid Visiblity**

The way the grid's visibility has been clarified:

- **Old**:
    - ``GtGrid::hideGrid(bool)`` / ``showGrid(bool)`` influenced the visibility of the grid and axis
    - ``GtGrid::setShowAxis(bool)`` only influenced whether a horizontal axis was shown (if the object was not hidden using ``GtGrid::hideGrid(bool)`` / ``showGrid(bool)``)

- **New**:
    - ``GtGrid::setVisible(bool)`` / ``hide()`` / ``show()`` now sets the global visibility of the object, but the grid remembers its last state.
    - ``GtGrid::enableGrid(bool)`` sets whether grid should be enabled. The grid is only shown if the grid is enabled and the object itself is visible. Use ``GtGrid::isGridVisible`` to check if the grid is actually visible.
    - ``GtGrid::enableMinorGrid(bool)`` sets whether minor grid should be enabled. Minor grid is only shown if the grid is enabled and the object itself is visible. Use ``GtGrid::isMinorGridVisible`` to check if the minor grid is actually visible.
    - ``GtGrid::setActiveAxis(ActiveAxis)`` sets which axis is enabled. Axis are only shown if any axis is enabled and the object itself is visible. Use ``GtGrid::isAxisVisible`` to check if the axis grid is actually visible.

Example Snippets
****************

Example (old):

.. code-block:: cpp

    auto* scene = new GtGraphicsScene();     // had to use GtGraphicsScene before
    auto* view = new GtGraphicsView(scene);  // view destroys its scene, could not work without a scene
    auto* grid = new GtGrid(*view);          // grid is owned by view, but was not clearly expressed
    view->setGrid(grid); 
    grid->setGridWidth(50);                  // no control over minor grid
    grid->setGridHeight(50);
    grid->setShowAxis(true);                 // only showed horizontal axis

    // needed to register ruler on view and grid
    auto* hRuler = new GtRuler(Qt::Horizontal);
    auto* vRuler = new GtRuler(Qt::Vertical);
    view->setHorizontalRuler(hRuler);        // unsure whether view or grid may own rulers
    view->setVerticalRuler(vRuler);
    view->grid()->setHorizontalRuler(hRuler);
    view->grid()->setVerticalRuler(vRuler);

    auto* lay = new QGridLayout(this);       // parent widget ("this") owns rulers and view
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);
    lay->addWidget(hRuler, 0, 1);
    lay->addWidget(vRuler, 1, 0);
    lay->addWidget(view, 1, 1);

Example (new):

.. code-block:: cpp

    auto* scene = new QGraphicsScene();      // Now operates on plain QGraphicsScene objects
    auto* view = new GtGraphicsView(scene, GtGraphicsView::DestroyActiveSceneOnDeletion);
    auto* grid = new GtGrid(view);           // parent (view) now owns grid
    view->setGrid(grid);                     // transfers ownership to view
    grid->setSpacing(50);                    // sets both horizontal and vertical spacing
    grid->setSubdivisions(5);                // configure number of minor grid lines
    grid->setActiveAxis(Qt::Horizontal | Qt::Vertical); // sets which axis to show

    QPen minorPen = grid->minorPen();        // allows control over major, minor, and axis pen
    minorPen.setStyle(Qt::DotLine);
    grid->setMinorPen(minorPen);
    grid->setMajorLineColor(Qt::gray);       // same as QPen::setColor on the major pen

    // sets how grid should adapt to changes in the zoom level
    grid->setScalingStrategy(GtGrid::ScalingStrategy::Base10);

    // need to register ruler on view only
    auto* hruler = new GtRuler(Qt::Horizontal);
    auto* vruler = new GtRuler(Qt::Vertical);
    view->connectHorizontalRuler(hruler);    // does not take ownership
    view->connectVerticalRuler(vruler);      // does not take ownership

    auto* lay = new QGridLayout(this);       // parent widget ("this") owns rulers and view
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);
    lay->addWidget(hruler, 0, 1);
    lay->addWidget(vruler, 1, 0);
    lay->addWidget(view, 1, 1);

**GtGraphicsScene**

``GtGraphicsScene`` is deprecated in 2.1. Use ``QGraphicsScene`` directly:

.. code-block:: cpp

    // old
    auto* scene = new GtGraphicsScene();

    // new
    auto* scene = new QGraphicsScene();

For finding items, use ``gt::gui::findGraphicsItems<T>()`` instead of
``GtGraphicsScene::findItems<T>()``.

**Grid Scaling Strategies**

``GtGrid::setScaleGrid(bool)`` is replaced by ``GtGrid::setScalingStrategy(ScalingStrategy)``:

- ``ScalingStrategy::Fixed`` - No automatic scaling (like ``setScaleGrid(false)``).
- ``ScalingStrategy::Base2`` - Scales in step sizes of 2 (like old ``setScaleGrid(true)`` default).
- ``ScalingStrategy::Base10`` - Scales in step sizes of 10.
- ``ScalingStrategy::OneTwoFive`` - Scales in step sizes of 1, 2, 5, 10, 20, 50, etc. New default.

Grid spacing methods:

- ``GtGrid::setGridWidth`` / ``setGridHeight`` -> ``GtGrid::setHSpacing`` / ``setVSpacing`` (or ``GtGrid::setSpacing`` for both).
- ``GtGrid::scaledGridSpacing`` return a ``GtGridSpacing`` struct with ``hSpacing`` and ``vSpacing`` members denoting the spacing in scene coordinate systems for the current zoom level.

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