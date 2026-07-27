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
not automatically propagated to child threads.

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

New calculators should accept a ``GtExecutionContext`` (or the project accessor
derived from it) from their execution boundary and use that explicit value for
project data. This makes the project dependency visible and avoids coupling new
code to the compatibility singleton. Existing calculators may continue to use
``gtApp->currentProject()``; while they run through the Core process executor,
that call resolves to the execution project without source changes.

The context is thread-local and is not copied to threads created by a module.
Worker code must therefore receive the project or context explicitly. A worker
must not retain a borrowed project pointer beyond the execution lifetime, and
queued callbacks must not infer a project later through an unscoped global
lookup.

Core process execution
----------------------

The Core process executor determines the project from the task/source and
passes an execution context into ``GtRunnable``. The runnable installs that
context on its worker thread for the complete read, calculator, and write
boundary. Its project path therefore comes from execution-specific context
data instead of the GUI-selected project. The scope is removed when the run
finishes, including failed and interrupted runs.

Project-scoped mutation policy
------------------------------

Mutating process executions are serialized per project. The core derives a
stable execution key from the canonical project path and holds a scoped guard
from task setup through result merging. A second executor targeting the same
project receives a structured ``Busy`` result and is rejected; it is not
silently queued behind an unrelated executor. Executions for different
projects use different keys and can proceed concurrently.

Project save and close operations check the same guard and fail deterministically
while an execution is active. The guard is released on normal completion,
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
