Headless single-project runtime
================================

The headless runtime provides a small Core-facing facade for a worker process
that owns exactly one loaded project. It is intentionally independent of
``GtMainWindow`` and other GUI classes.

The current API is exposed by:

* ``GtHeadlessProjectRuntime``
* ``GtHeadlessTaskHandle``
* ``GtHeadlessTaskStatus``
* ``GtHeadlessTaskCancellationResult``
* ``GtHeadlessRuntimeResult``

Minimal usage
-------------

The caller owns the ``QCoreApplication``. The runtime uses the existing
``GtCoreApplication`` and Core datamodel but does not own the application
object.

.. code-block:: cpp

   GtHeadlessProjectRuntime runtime;

   auto result = runtime.initialize();
   if (!result)
   {
       // inspect result.code and result.message
   }

   result = runtime.openProject(projectPath);
   if (!result)
   {
       // project could not be loaded
   }

   const auto tasks = runtime.listTasks();

   GtHeadlessRuntimeResult submitResult;
   auto handle = runtime.submitTask("GroupName/TaskId", &submitResult);
   if (submitResult)
   {
       const auto status = handle.wait();
       // status.state and status.error describe completion
   }

   runtime.saveProject();
   runtime.closeProject();

Lifecycle and ownership
-----------------------

The runtime follows this lifecycle::

   Created -> Initialized -> ProjectLoaded -> Closed

If project removal fails after the project was closed, the runtime enters
``CloseFailed`` and retains the project handle so that ``closeProject()`` can
be retried.

Initialization is idempotent. A runtime accepts one project and rejects a
second project or operations without a loaded project. ``openProject()`` adds
the project to the current Core session; ``closeProject()`` closes and removes
that project again.

The runtime does not expose a borrowed ``GtProject*``. Project information is
returned as value data such as ``projectPath()`` and task descriptors. Task
handles are copyable value objects backed by runtime-owned execution state.
They become ``Shutdown`` when the runtime closes or is destroyed.

Threading
---------

Runtime operations and task-handle operations must be called from the thread
owning ``GtCoreApplication`` and its Qt event loop. ``wait()`` processes Qt
events in a local event loop while waiting so that the non-blocking Core
executor can complete its task runner. Calls to ``status()``, ``cancel()``
and ``wait()`` from another thread are rejected. A future worker integration
should marshal calls to this thread explicitly. Shutdown uses a bounded wait
and reports a terminal ``Shutdown`` status if cancellation does not complete.

The runtime does not currently propagate an execution context into arbitrary
module-created threads. Such code must receive its project or context
explicitly.

Task addressing and results
---------------------------

Tasks can be addressed by ``TaskId``, ``GroupName/TaskId`` or UUID. An
unqualified task id uses the default task-group lookup; a qualified id first
looks in the requested custom group and then in the user group. UUID lookup is
independent of task-group selection and does not change the current group.
Handles expose a stable opaque identifier, queued/running/terminal state, the
underlying Core process state, progress in percent where available,
cancellation, and waiting. ``cancel()`` returns
a structured result distinguishing accepted requests from completion,
shutdown, thread, availability and executor-rejection failures. The final
status also exposes a structured result (including execution failure after
post-processing or result merging), while structured error information remains
available after completion. Progress is represented as ``-1`` only when
neither the task nor its process components provide a value.
The runtime retains only active task states; completed states are removed from
the runtime's internal active-task list after the executor signals completion,
while copied handles retain their terminal snapshot.

The status API is deliberately value-based and does not expose ``GtTask*`` or
other Qt pointers across the integration boundary.

``Finished`` is reported only after the complete executor operation has
finished, including post-processing and result merging. A task whose process
component already reports ``FINISHED`` but whose executor is still completing
post-processing is reported as ``Running`` until that final boundary.

Execution context and project coordination
------------------------------------------

Task execution reuses the Core executor integration from PR #1514. The
executor installs a ``GtExecutionContext`` for the selected project and holds
the project mutation guard for the task's execution scope. The runtime always
uses its explicitly opened project; it does not use the GUI-selected project
to choose an execution target.

Project save and close use the same guard-aware Core datamodel operations.
They reject changes while a task is mutating the project and preserve the
executor's pre-existing flags after the runtime has finished using its
non-blocking mode. The current design assumes one runtime/executor consumer per
worker process; another consumer must not change the shared executor flags
while a runtime task is active.

Python and worker integration
-----------------------------

The Python binding should remain a thin adapter over this API. It may convert
task descriptors, statuses, and operation results to Python values, but task
lookup, handle storage, lifecycle, and execution belong to Core.

The existing Python webservice can later preserve its start/poll/cancel HTTP
interaction while replacing its current task-service calls with the runtime.
REST or worker-process orchestration is not part of this Core API.

Prototype migration
-------------------

The implementation reuses the prototype's task descriptors, stable task
handles, lifecycle polling and non-blocking execution model. It redesigns the
boundary as a value-based Core API with an explicit project, execution context,
project guard, structured operation/cancellation results and owner-thread
lifecycle. Prototype-specific webservice and worker orchestration, as well as
private implementation details, are intentionally not part of this API.
