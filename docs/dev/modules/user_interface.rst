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
   * - A module page inside the Process Dock
     - Register a :cpp:class:`GtProcessDockPage` factory with
       ``gt::gui::registerProcessDockPage()``.
   * - An action in the Process Dock's Add or component context menu
     - Register a ``GtObjectUIAction`` with
       ``gt::gui::registerProcessAddAction()`` or
       ``gt::gui::registerProcessContextAction()``.
   * - Custom post-processing template or plot
     - Register it through ``postItems()`` or ``postPlots()``.

The complete registration pattern is documented in
:ref:`mdiinterface`. GUI modules link ``GTlab::Gui`` and register concrete
classes as Qt metadata; GTlab creates instances when users invoke the feature.

Extend the Process Dock
-----------------------

The Process Dock provides module extension points for process-specific pages
and actions. Register contributions from the module's ``init()`` method. The
page factory is called after module initialization, when the Process Dock is
ready to create its pages.

Derive a page from :cpp:class:`GtProcessDockPage`, return a stable unique ID
and a non-empty title, then register a factory:

.. code-block:: cpp

  #include "gt_processdockpage.h"

  class ResultsPage : public GtProcessDockPage
  {
  public:
      QString pageId() const override
      {
          return QStringLiteral("my-module-results");
      }

      QString title() const override
      {
          return tr("Results");
      }

      // Build the page's widgets in this constructor.
  };

  void MyModule::init()
  {
      gt::gui::registerProcessDockPage(
          [] { return new ResultsPage; });
  }

The Process Dock owns each page returned by a factory. The optional ``icon()``
sets the selector icon. Emit ``activationRequested()`` when the page needs to
become active, for example after the user invokes a related command. GTlab
ignores factories that return null pages and pages with an empty or duplicate
ID or an empty title. The built-in ``processes`` page ID is reserved.

Modules can also add actions to the Process Dock menus:

.. code-block:: cpp

  gt::gui::registerProcessAddAction(
      GtObjectUIAction(QStringLiteral("Add result"),
                       [](GtObject* target) {
                           // Add an object to target.
                       }));

  gt::gui::registerProcessContextAction(
      GtObjectUIAction(QStringLiteral("Inspect process"),
                       [](GtObject* component) {
                           // Inspect the clicked process component.
                       }));

An Add action receives the selected task as its target, or the current task
group when no task is selected. A component context action receives the
process component whose context menu is open. Use the ``GtObjectUIAction``
visibility and verification methods to hide or disable actions for targets
that do not support them. These contributions use the existing object UI
action mechanism; see :cpp:class:`GtObjectUIAction` for its callback options.

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
