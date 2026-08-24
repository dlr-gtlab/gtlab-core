.. _label_migration_2-1_graphics_api:

Graphics API
============

This guide covers the breaking changes in GTlab 2.1 for ``GtGraphicsView``,
``GtGrid``, ``GtRuler``, and related graphics API components.

The new API allows more control over minor grid and axis, including styling the grid and axis with custom pens.

Quick Start
~~~~~~~~~~~

1. Replace ``new GtGrid(view)`` with ``new GtGrid(parent)`` and use
   ``GtGraphicsView::setGrid(GtGrid*)`` to transfer ownership.
2. Replace ``GtGraphicsView::setHorizontalRuler``/``setVerticalRuler`` with
   ``GtGraphicsView::connectHorizontalRuler``/``connectVerticalRuler`` (no ownership taken).
   Remove calls to ``GtGrid::setHorizontalRuler``/``setVerticalRuler``.
3. Replace ``GtGraphicsScene`` with ``QGraphicsScene``.
4. Update grid scaling API: ``setScaleGrid(bool)`` -> ``setScalingStrategy``.
5. Update ruler constructor: ``GtRuler()`` -> ``GtRuler(Qt::Orientation)``.

.. note::
    Use preprocessor-macros for version dependent code:

    .. code-block:: cpp

        #if GT_VERSION < GT_VERSION_CHECK(2, 1, 0)
            grid->setVerticalRuler(hruler);
            grid->setHorizontalRuler(hruler);
        #else
            // nothing to do here
        #endif

Ownership Model
~~~~~~~~~~~~~~~

GtGraphicsView and GtGrid
^^^^^^^^^^^^^^^^^^^^^^^^^

The ownership model for ``GtGrid`` has been clarified:

- **Old**: ``GtGrid`` was constructed with a ``QGraphicsView&`` reference and
  the grid was managed internally with unclear ownership semantics.
- **New**: ``GtGrid`` uses standard Qt parent-child ownership.
  ``GtGraphicsView::setGrid(GtGrid*)`` takes ownership. The grid is no longer fixed to ``GtGraphicsView``.

Example (old)::

    auto* scene = new QGraphicsScene();
    auto* view = new GtGraphicsView(scene);  // view destroys its scene, could not work without a scene
    
    auto* grid = new GtGrid(*view);          // grid is owned by view, but was not clearly expressed
    view->setGrid(grid); 
    grid->setShowAxis(true);                 // only showed horizontal axis
    grid->setGridWidth(50);                  // no control over minor grid
    grid->setGridHeight(50);
    auto* hRuler = new GtRuler(Qt::Horizontal);
    auto* vRuler = new GtRuler(Qt::Vertical);
    
    // need to register ruler on view and grid
    view->setHorizontalRuler(hRuler);        // unsure whether view or grid may own rulers
    view->setVerticalRuler(vRuler);
    view->grid()->setHorizontalRuler(hRuler);
    view->grid()->setVerticalRuler(vRuler);

    auto* lay = new QGridLayout();
    lay->addWidget(hRuler, 0, 1);
    lay->addWidget(vRuler, 1, 0);
    lay->addWidget(view, 1, 1);              // (layout owns rulers and view)

Example (new)::

    auto* scene = new QGraphicsScene();
    auto* view = new GtGraphicsView(scene, GtGraphicsView::DestroyActiveSceneOnDeletion);

    auto* grid = new GtGrid(view);           // parent (view) now own grid
    view->setGrid(grid);                     // transfers ownership to view

    grid->setSpacing(50);                    // sets both horizontal and vertical spacing
    grid->setSubdivisions(5);                // configure number of minor grid lines
    
    QPen minorPen = grid->minorPen();        // allows control over major, minor, and axis pen
    minorPen.setStyle(Qt::DotLine);
    grid->setMinorPen(minorPen);
    
    grid->setLineColor(Qt::grey);            // same as QPen::setColor on the major pen
    
    // sets which axis to show
    grid->setVisibleAxis(Qt::Horizontal | Qt::Vertical);
    // sets how grid should adapt to changes in the zoom level
    grid->setScalingStrategy(GtGrid::ScalingStrategy::Base10);

    auto* hruler = new GtRuler(Qt::Horizontal);
    auto* vruler = new GtRuler(Qt::Vertical);
    view->connectHorizontalRuler(hruler);    // does not take ownership
    view->connectVerticalRuler(vruler);      // does not take ownership

    auto* lay = new QGridLayout();
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);
    lay->addWidget(hruler, 0, 1);
    lay->addWidget(vruler, 1, 0);
    lay->addWidget(view, 1, 1);              // layout owns rulers and view

GtGraphicsScene
^^^^^^^^^^^^^^^

``GtGraphicsScene`` is deprecated in 2.1. Use ``QGraphicsScene`` directly::

    // old
    auto* scene = new GtGraphicsScene();

    // new
    auto* scene = new QGraphicsScene();

For finding items, use ``gt::gui::findGraphicsItems<T>()`` instead of
``GtGraphicsScene::findItems<T>()``.

Grid Scaling Strategies
~~~~~~~~~~~~~~~~~~~~~~~

``GtGrid::setScaleGrid(bool)`` is replaced by ``GtGrid::setScalingStrategy(ScalingStrategy)``:

- ``ScalingStrategy::Fixed`` - No automatic scaling (like ``setScaleGrid(false)``).
- ``ScalingStrategy::Base2`` - Scales in step sizes of 2 (like old ``setScaleGrid(true)`` default).
- ``ScalingStrategy::Base10`` - Scales in step sizes of 10.
- ``ScalingStrategy::OneTwoFive`` - Scales in step sizes of 1, 2, 5, 10, 20, 50, etc. New default.

Grid spacing methods:

- ``GtGrid::setGridWidth`` / ``setGridHeight`` -> ``GtGrid::setHSpacing`` / ``setVSpacing`` (or ``GtGrid::setSpacing`` for both).
- ``GtGrid::scaledGridSpacing`` return a ``GtGridSpacing`` struct with ``hSpacing`` and ``vSpacing`` members denoting the spacing in pixels for the current zoom level.
