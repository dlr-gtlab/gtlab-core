Executable operations
=====================

Executable operations separate computational work from the place and lifecycle
that run it. An operation contains domain logic. An executor and its selected
backend prepare the invocation, choose where it runs, and manage its asynchronous
lifecycle. At the execution location, ``GtExecutionEnvironment`` provides a
small synchronous boundary around ``GtExecutableOperation::execute()``.

The same environment contract works in a dedicated GUI execution thread, a
one-shot worker, a resident project worker, or a future remote worker. The
environment itself does not choose among these placements.

Operation lifecycle
-------------------

The operation separates input preparation, computation, and updates to the
originating project:

.. code-block:: text

   Originating side          Executor/backend             Execution location
   -----------------         -----------------             ------------------
   createData()  ---------->  provision operation/data --> GtExecutionEnvironment
   applyResult() <----------  return full outcome  <------ execute()

``createData()`` prepares optional detached input on the originating side.
``applyResult()`` interprets the complete execution outcome there. Both should
remain lightweight. The later ``GtOperationExecutor`` owns asynchronous
submission and completion, backend selection, cancellation requests, result
transport, and the decision when to call ``applyResult()``.

``GtExecutionEnvironment::execute()`` runs synchronously in its caller's thread.
It does not create threads or processes, schedule work, or move objects between
threads. The selected backend must provision the operation, optional input,
event stream, and any required project for that thread. For normal GUI-local
execution, the backend should use a dedicated execution thread so that long
computations do not block the GUI.

Project access
--------------

``requiresProject()`` states whether the invocation needs an execution-local
``GtProject``. It does not select the execution location.

* If ``requiresProject()`` is ``true``, the environment requires its borrowed
  project and installs it in a ``GtExecutionContextScope`` for the call.
  ``gtApp->currentProject()`` and ``gtDataModel->currentProject()`` then return
  that execution-local project.
* If ``requiresProject()`` is ``false``, the environment installs an explicitly
  empty execution context. Both current-project accessors return ``nullptr``
  during the call, even if the environment has a project or a GUI session has a
  selected project. The environment does not inspect that unused project,
  including its thread affinity.

The environment borrows the optional project; it does not open, reconstruct,
save, close, or own it. It does not initialize ``GtCoreApplication``, the data
model, or a session. The caller must keep the project alive for every call that
uses it.

An operation may start its own child threads. The environment does not copy the
execution context into them. Task infrastructure that needs legacy current-
project access from a child thread must copy the ``GtExecutionContext`` and
install a ``GtExecutionContextScope`` there. Shared project access is not made
thread-safe by the environment.

Execution outcomes
------------------

``execute()`` returns a ``GtOperationExecutionResult`` containing a status, an
optional operation-defined code, an optional message, and an optional
``GtObject`` payload. The status is ``Success``, ``Failed``, or ``Cancelled``.
A payload may accompany any status. Generic execution code must not infer from
the status whether the payload is complete, partial, diagnostic, or suitable
for application.

The environment returns a ``GtExecutionResult`` with two distinct levels:

* ``error() == Error::None`` means an operation outcome is present. A regular
  domain failure has ``Status::Failed`` at this level.
* A non-``None`` error, such as ``ProjectRequired``, ``WrongThread``, or
  ``UnhandledException``, means no operation outcome is present.

The environment catches an exception escaping ``execute()`` and reports an
``UnhandledException`` boundary error. It does not convert a returned operation
failure into an environment error.

Cancellation is cooperative. If cancellation was already requested before the
call enters ``execute()``, the environment skips the operation and returns a
regular ``Cancelled`` outcome. During execution, the operation can inspect the
shared token and decides how to react. The environment preserves the status the
operation returns, including ``Success`` or ``Failed`` after a cancellation
request. Hard interruption belongs to the process or backend boundary.

Result transport and application
--------------------------------

``GtOperationExecutionResult`` is a Core value type and is independent of a
transport protocol. A local or remote adapter encodes its status, code, and
message as required. It serializes an optional ``GtObject`` payload through the
existing Memento and ``GtObjectFactory`` mechanisms. The environment performs
no serialization.

Transporting an outcome and applying it to the originating project are
separate steps. The originating operation receives the full outcome in
``applyResult()`` and decides how its status and optional payload affect the
project. The generic executor does not assume that only successful outcomes
contain useful data or that failed and cancelled outcomes must be discarded.

Project state after an invocation
---------------------------------

The environment does not roll back project changes. A project-bound operation
may have changed its execution-local project before it fails, is cancelled, or
throws. A one-shot worker can discard that project. A resident project worker
must decide at a higher level whether to trust its warm project and whether to
restore or provision it again before another call. Recovery and checkpoints are
outside the environment.

GTlab integration
-----------------

``GtExecutableOperation`` is a normal ``GtObject``. Operations, input data, and
result payloads use GTlab properties, ``GtObjectFactory``, and Memento/XML.
Modules declare operation classes through ``GtOperationInterface``. Existing
``GtTask`` and calculator implementations can later be exposed through an
adapter; the environment does not depend on ``GtTask`` or
``GtCoreProcessExecutor``.

The :doc:`architecture decision <decisions/0001-executable-operations>` records
the constraints for the executor, environment, operation outcome, and future
worker adapters.
