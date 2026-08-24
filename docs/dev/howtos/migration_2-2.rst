Migration to GTlab 2.2
======================

This page is work in progress. Relevant changes in the API for the GTlab Version 2.2. are noted below.

Deprecated APIs Removed in 2.2
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The tables below list deprecated APIs relevant for migration to GTlab 2.2.

The value in **Since** is the first released git tag in this
repository that already contains the deprecation marker.

Gui-Library Deprecations
^^^^^^^^^^^^^^^^^^^^^^^^

.. rst-class:: compact-table

.. list-table::
   :header-rows: 1
   :widths: 40 10 50

   * - **API**
     - **Since**
     - **Replacement / Notes**
   * - ``GtGraphicsScene`` 
     - 2.1.0
     - Use ``QGraphicsScene`` instead.
   * - ``GtGraphicsScene::findItems<T>()`` 
     - 2.1.0
     - Use ``gt::gui::findGraphicsItems<T>()`` instead.
   * - ``GtGraphicsView::setHorizontalRuler(GtRuler*)``, ``GtGraphicsView::setVerticalRuler(GtRuler*)``
     - 2.1.0
     - Use ``GtGraphicsView::connectHorizontalRuler(GtRuler*)`` and ``GtGraphicsView::connectVerticalRuler(GtRuler*)`` instead.
   * - some methods in ``GtGraphicsView``
     - 2.1.0
     - Apply the suggested changes
   * - ``GtGrid::setScaleGrid(bool)``
     - 2.1.0
     - Use ``GtGrid::setScalingStrategy(ScalingStrategy)`` instead. E.g. ``GtGrid::ScalingStrategy::Fixed`` disables adaptive scaling and ``GtGrid::ScalingStrategy::Base2`` implements previous default behavior.
   * - multiple methods in ``GtGrid``
     - 2.1.0
     - Apply the suggested changes
   * - Constructor ``GtRuler::GtRuler()``
     - 2.1.0
     - Use ``GtRuler::GtRuler(Qt::Orientation)`` instead.
   * - some methods in ``GtRuler``
     - 2.1.0
     - Apply the suggested changes
