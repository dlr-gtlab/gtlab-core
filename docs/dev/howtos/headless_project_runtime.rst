Headless single-project runtime
===============================

``GtHeadlessProjectRuntime`` owns the lifecycle of one project and provides
asynchronous submission of ``GtExecutableOperation`` objects. It does not own
``QCoreApplication`` and does not expose a borrowed ``GtProject*``.

The caller supplies an execution-local operation and optional detached input.
The runtime takes ownership, returns a copyable handle immediately, and later
stores the detached result or a structured failure. The runtime does not call
``applyResult()``; applying the result belongs to the originating side.

Minimal usage
-------------

.. code-block:: cpp

   GtHeadlessProjectRuntime runtime;
   auto result = runtime.initialize();
   if (result)
   {
       auto handle = runtime.submitOperation(std::move(operation),
                                             std::move(input), &result);
       // Submission returns before execute() starts.
       // Observe handle.status(), handle.events(), and handle.result().
   }

Project-bound operations require a loaded project. Project-free operations can
run after initialization without opening one. During execution, the runtime
installs ``GtExecutionContext`` for project-bound work and provides a separate
``GtOperationExecutionContext`` for input data, cancellation, execution ID,
and events.

Lifecycle and threading
-----------------------

The project lifecycle is ``Created -> Initialized -> ProjectLoaded -> Closed``.
The runtime rejects a second project. Save and close reject active project
execution. Project and runtime lifecycle methods must run on the GTlab owner
thread.

Submission queues synchronous ``execute()`` on the GTlab owner thread. Handle
status, cancellation, and result snapshots use shared synchronized state and
can be queried from other threads. Waiting on the owner thread is rejected
while an operation is active because it could prevent the queued execution
from making progress. Operations should cooperate with cancellation through
``GtOperationExecutionContext::cancellation()``.

Because ``execute()`` runs synchronously on the owner thread, an operation that
does not return also prevents that thread from processing shutdown or project
lifecycle requests. Cancellation remains requestable from another thread, but
it is cooperative; this implementation cannot forcibly stop an uncooperative
operation or guarantee bounded shutdown while it is executing.

Task lookup, task progress, and ``GtCoreProcessExecutor`` integration are not
part of this generic runtime API. A later ``ProcessTaskOperation`` adapter
connects existing task execution to the operation model.
