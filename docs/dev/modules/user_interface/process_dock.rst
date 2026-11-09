Extending the Process Dock
==========================

Modules can add process-specific pages and actions to the Process Dock.
Register these contributions from the module's ``init()`` method. The Process
Dock calls page factories after module initialization, when it is ready to
create its pages.

Add a page
----------

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

Add menu actions
----------------

Register actions to add process elements or to extend a process component's
context menu:

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
