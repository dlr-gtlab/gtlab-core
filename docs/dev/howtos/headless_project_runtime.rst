Headless single-project runtime
================================

The headless runtime provides a small Core-facing facade for a worker process
that owns exactly one loaded project. It is intentionally independent of
``GtMainWindow`` and other GUI classes.

The current API is exposed by:

* ``GtHeadlessProjectRuntime``
* ``GtHeadlessTaskHandle``
* ``GtHeadlessTaskStatus``
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
events while waiting so that the non-blocking Core executor can complete its
task runner. A future worker integration should marshal calls to this thread
explicitly.

The runtime does not currently propagate an execution context into arbitrary
module-created threads. Such code must receive its project or context
explicitly.

Task addressing and results
---------------------------

Tasks can be addressed by ``TaskId`` or ``GroupName/TaskId``. Handles expose a
stable opaque identifier, queued/running/terminal state, the underlying Core
process state, cancellation, and waiting. Progress is represented as
unavailable until the executor provides a task-specific value.

The status API is deliberately value-based and does not expose ``GtTask*`` or
other Qt pointers across the integration boundary.

PR #1514 integration
--------------------

The current implementation uses the existing Core executor and contains a
localized integration point for the execution context and project mutation
guard introduced by PR #1514. Once that PR is merged, task submission must
acquire ``GtProjectExecutionGuard`` and install ``GtExecutionContext`` at that
boundary. The runtime must continue to use the explicit runtime project and
must not use the GUI-selected project to choose its execution target.

Python and worker integration
-----------------------------

The Python binding should remain a thin adapter over this API. It may convert
task descriptors, statuses, and operation results to Python values, but task
lookup, handle storage, lifecycle, and execution belong to Core.

The existing Python webservice can later preserve its start/poll/cancel HTTP
interaction while replacing its current task-service calls with the runtime.
REST or worker-process orchestration is not part of this Core API.
