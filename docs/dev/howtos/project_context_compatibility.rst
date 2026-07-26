Project context compatibility baseline
=======================================

This page records the compatibility baseline for the first project-context
migration. It describes the behavior that existing desktop code and legacy
calculators rely on before an execution-scoped context is introduced.

Compatibility matrix
--------------------

``currentProject()`` is used through several existing Core access paths:

.. list-table:: Core call-site classification
   :header-rows: 1
   :widths: 24 28 48

   * - Area
     - Representative files
     - Current contract
   * - GUI selection and commands
     - ``src/app/gt_mainwin.cpp``; ``src/app/dock_widgets/``;
       ``src/gui/object_ui/gt_projectui.cpp``;
       ``src/gui/dock_widgets/properties/editors/``
     - Resolve the project selected by the desktop session for display,
       editing, saving, closing, and undo-command creation.
   * - GUI data-model coordination
     - ``src/core/gt_coreapplication.cpp``;
       ``src/core/gt_coredatamodel.cpp``;
       ``src/core/gt_session.cpp``
     - ``GtCoreApplication::currentProject()`` and
       ``GtCoreDatamodel::currentProject()`` return the session's selected
       project; no project returns ``nullptr``.
   * - Process execution
     - ``src/gui/process_runner/gt_processrunnertransceiver.cpp``;
       ``src/core/gt_runnable.cpp``;
       ``src/core/process_management/gt_processdata.cpp``
     - Existing process infrastructure uses the selected project for the
       project path, process bookkeeping, and execution cleanup.
   * - Legacy calculator behavior
     - ``src/core/process_management/calculators/gt_exporttomementocalculator.cpp``
     - Calculators may read ``gtApp->currentProject()`` without receiving a
       project argument. This source-level behavior must remain valid.
   * - Data access and path lookup
     - ``src/gui/gt_datamodel.cpp``;
       ``src/gui/gt_textfilterdelegate.cpp``;
       ``src/core/gt_runnable.cpp``
     - Callers use the selected project to find common parents, resolve
       project paths, or update UI state.

The executable baseline is covered by
``CurrentProjectCompatibility.guiFallbackTracksSelectedProject`` and
``CurrentProjectCompatibility.legacyCalculatorReadsSelectedProject``. The
second test represents a calculator that keeps the existing
``gtApp->currentProject()`` access and verifies that it observes the selected
project during execution.

Known limitations
-----------------

* A project context is not implicitly propagated to module-created worker
  threads. A worker must receive the project explicitly until a context-aware
  helper is introduced.
* Delayed callbacks can run after the initiating process has returned. They
  must not retain or later infer a project from an unscoped global lookup.
* The current GUI supports one selected project at a time. This baseline does
  not promise multiple projects being edited concurrently in the desktop
  application.
* No production behavior is changed by this baseline. The later execution
  context work must preserve the GUI/main-thread fallback documented here.

Execution context API
---------------------

The next migration step provides ``GtExecutionContext`` for new execution
code. It carries a borrowed project pointer, the execution data root, a source
identifier, the project path, and an optional job identifier. The context does
not own the project and does not extend its lifetime; the project must remain
alive for as long as the context may be used.

``GtExecutionContextScope`` installs a context only on the current thread.
Scopes can be nested and their destructors restore the previous context, also
when control leaves the scope early or through an exception. The current
context is available through ``GtExecutionContext::current()``. Contexts are
not automatically propagated to child threads, and installing a scope does
not change ``GtCoreApplication::currentProject()``.
