Extending the Process Dock
==========================

Modules can add process-specific pages and actions to the Process Dock.
Register these contributions from the ``init()`` hook of
:ref:`GtModuleInterface <moduleinterface>`. See the :ref:`init() hook
documentation <moduleinterface-init-hook>` for when GTlab calls it. The
Process Dock calls page factories after module initialization, when it is
ready to create its pages.

Add a page
----------

Derive a page from :cpp:class:`GtProcessDockPage`, return a stable unique ID
and a non-empty title, then register a factory:

.. code-block:: cpp

  #include "gt_processdockextensions.h"

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
          this, [] { return new ResultsPage; });
  }

The Process Dock owns each page returned by a factory. The optional ``icon()``
sets the selector icon. Emit ``activationRequested()`` when the page needs to
become active, for example after the user invokes a related command. GTlab
ignores factories that return null pages and pages with an empty or duplicate
ID or an empty title. The built-in ``processes`` page ID is reserved.

Add actions
-----------

An Add action receives the selected task as its target. If no task is selected,
it receives the current task group. Register it from ``MyModule::init()`` and
pass the module interface as the owner so invokable callbacks use the module
object as their parent.

Context actions
---------------

An action in a process component's context menu receives the clicked process
component. Register it from the same ``MyModule::init()`` hook.

The module interface owns both registrations. GTlab ignores them after the
owner is destroyed. Use the ``GtObjectUIAction`` visibility and verification
methods to hide or disable actions for targets that do not support them. See
:cpp:class:`GtObjectUIAction` for the available callback options.

Register both actions from ``MyModule::init()``:

.. code-block:: cpp

  void MyModule::init()
  {
      gt::gui::registerProcessAddAction(
          this,
          GtObjectUIAction(QStringLiteral("Add result"),
                           GtObjectUIAction::InvokableActionMethod(
                               [this](QObject*, GtObject* target) {
                                   // Add an object to target.
                               })));

      gt::gui::registerProcessContextAction(
          this,
          GtObjectUIAction(QStringLiteral("Inspect process"),
                           GtObjectUIAction::InvokableActionMethod(
                               [this](QObject*, GtObject* component) {
                                   // Inspect the clicked process component.
                               })));
  }
