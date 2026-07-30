Project context and ``currentProject()`` compatibility
=======================================================

This page records the temporary compatibility baseline for the first
project-context migration. The complete module porting guidance is tracked in
issue 1518; this page documents the API contract needed by the current
implementation.

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
* This is a transition document. Module-specific porting recipes and broader
  integration guidance are intentionally tracked separately in issue 1518.

Execution context API
---------------------

``GtExecutionContext`` provides a borrowed project pointer and an optional
project path for new execution code. A context is valid when it contains either
one. A path-only context is useful for path resolution, but
``currentProject()`` returns ``nullptr`` when no project pointer is present.
The context does not own the project and does not extend its lifetime; the
project must remain alive for as long as the context may be used.

``GtExecutionContextScope`` installs a context only on the current thread.
Scopes can be nested and their destructors restore the previous context, also
when control leaves the scope early or through an exception. The current
context is available through ``GtExecutionContext::current()``. The scope and
context are synchronous and thread-local, and are not automatically propagated
to child threads. Do not retain either the context or its borrowed project
pointer for asynchronous work. During calculator execution, use the context
to identify the execution project; persistent project changes must go through
the normal task/result or datamodel APIs rather than direct mutation.

``currentProject()`` resolution
-------------------------------

``GtCoreApplication::currentProject()`` and
``GtCoreDatamodel::currentProject()`` use the same resolution policy:

1. the project from the active, thread-local ``GtExecutionContext``;
2. otherwise the currently selected project of the active session;
3. ``nullptr`` when neither is available.

This keeps existing calls such as ``gtApp->currentProject()`` source-compatible
while allowing legacy calculator code to observe its execution project. The
context is only available on the thread where its scope was installed; callers
must still use an explicit context for worker-thread execution.

Recommended calculator API
--------------------------

New calculator helpers and services should accept a ``GtProject*`` or
``GtExecutionContext`` from their execution boundary and use that explicit
value for project data. The historical ``GtCalculator::exec()`` interface does
not have a project parameter, so existing calculators may continue to use
``gtApp->currentProject()``. While they run through the Core process executor,
that call resolves to the execution project without source changes.

For new code, make the dependency explicit at the first helper boundary:

.. code-block:: cpp

   bool MyCalculator::updateProjectData(GtProject* project)
   {
       if (!project) return false;
       // Read execution data and prepare calculator results.
       return true;
   }

   bool MyCalculator::exec()
   {
       return updateProjectData(gtApp->currentProject());
   }

The explicit helper can later be called by another execution service without
depending on the GUI-selected project. Calculators must not retain the borrowed
project pointer for delayed work or pass it to a child thread without an
explicit lifetime and ownership policy.

The context is thread-local and is not copied to threads created by a module.
Worker code must therefore receive the project or context explicitly. A worker
must not retain a borrowed project pointer beyond the execution lifetime, and
queued callbacks must not infer a project later through an unscoped global
lookup.

Module component guidance
-------------------------

* **Tasks and task groups:** derive the project from the task hierarchy with
  ``task->findParent<GtProject*>()`` or pass it explicitly from the execution
  boundary. Do not use the GUI-selected project to identify the task's project.
* **Calculators:** keep ``exec()`` compatible, but pass ``GtProject*`` or
  ``GtExecutionContext`` into new calculator helpers and services. Existing
  ``gtApp->currentProject()`` calls remain supported during Core execution.
* **Project-bound MDI widgets:** store the project supplied when the widget is
  opened, for example as a ``QPointer<GtProject>``. Use
  ``gtApp->currentProject()`` only for actions explicitly targeting the
  currently selected GUI project.
* **Worker threads and callbacks:** pass the project or context explicitly;
  thread-local context is not inherited by a new thread and must not be
  inferred later from a global lookup.

Core process execution
----------------------

The Core process executor determines the project from the task/source and
passes an execution context into ``GtRunnable``. The runnable installs that
context on its execution thread for the complete read, calculator, and write
boundary. Its project path therefore comes from execution-specific context
data instead of the GUI-selected project. The scope is removed when the run
finishes, including failed and interrupted runs.

Project-scoped mutation policy
------------------------------

Mutating process executions are serialized per project. The core derives a
stable execution key from the canonical project path, or from the project's
stable UUID for pathless projects, and holds a scoped guard
from task setup through result merging. A second executor targeting the same
project receives a structured ``Busy`` result and is rejected; it is not
silently queued behind an unrelated executor. Executions for different
projects use different keys and can proceed concurrently.

Project save and close operations acquire the same exclusive guard for their
complete operation and fail deterministically while an execution is active.
The guard is released on normal completion,
failure, interruption, setup failure, and executor destruction. This is an
in-process coordination mechanism only; it does not provide distributed
locking or transactional conflict resolution.

End-to-end validation and deferred work
---------------------------------------

The compatibility test suite executes a legacy calculator through the same
``GtTaskRunner``/``GtRunnable`` boundary used by Core execution. It uses two
projects, changes the GUI-selected project while the calculator is running, and
verifies that the execution still observes its own project. Batch-mode tests
continue to verify the headless compatibility path, while project guards cover
same-project write serialization and preserve concurrent execution for distinct
projects.

The following architectural work remains deliberately deferred:

* execution-project snapshots;
* context-aware asynchronous helpers;
* per-context externalization and state services;
* a fully multi-project in-process datamodel; and
* web API and worker orchestration.
