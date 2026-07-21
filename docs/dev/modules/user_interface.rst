GUI design for modules
======================

GTlab modules can add object actions, editors and viewers, dock widgets, and
post-processing components. Keep these GUI classes separate from persistent
data and calculations so the module's core functionality also works in batch
execution and remains testable without widgets.

Choose the smallest contribution
--------------------------------

.. list-table:: Common GUI needs
   :header-rows: 1
   :widths: 30 70

   * - Need
     - Extension point
   * - Icon, context actions, or double-click behavior for a data object
     - Map the object class to a ``GtObjectUI`` implementation through
       ``GtMdiInterface::uiItems()``.
   * - Editor or viewer for one selected object
     - Register a ``GtMdiItem`` class through ``mdiItems()``.
   * - Module-wide tool that follows project changes
     - Register a ``GtDockWidget`` class through ``dockWidgets()``.
   * - Custom post-processing template or plot
     - Register it through ``postItems()`` or ``postPlots()``.

The complete registration pattern is documented in
:ref:`mdiinterface`. GUI modules link ``GTlab::Gui`` and register concrete
classes as Qt metadata; GTlab creates instances when users invoke the feature.

Separate responsibilities
-------------------------

Persistent :cpp:class:`GtObject` classes should contain domain state and
validation, not widgets. Process elements should contain calculations, not
dialogs. GUI classes read and edit those objects through their public API.
This separation prevents project loading and batch execution from depending on
a running graphical interface.

For edits that should participate in undo/redo, use GTlab's command mechanisms
and scope a command to the lowest object whose subtree contains the complete
change. Avoid using the whole project as the command root for a local edit.

Match the application
---------------------

Use GTlab's shared :doc:`user_interface/icons` and
:doc:`user_interface/stylesheets` instead of copying Core resources or fixing
colors for one theme. Prefer standard Qt layouts, size policies, and palettes;
apply a custom stylesheet only where GTlab provides a matching helper or the
feature genuinely needs a specialized presentation.

Test GUI contributions in both light and dark themes, with no project open,
and after switching or closing the current project. Object-specific widgets
must also handle their target object being removed outside the widget.

.. toctree::
   :maxdepth: 1

   user_interface/icons
   user_interface/stylesheets
