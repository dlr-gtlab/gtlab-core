Architecture decision 0001 — Executable operations
===================================================

:Status: Accepted
:Date: 2026-09-03
:Normative source: GitHub issue `#1526 <https://github.com/dlr-gtlab/gtlab-core/issues/1526>`_
:Related: `#1515 <https://github.com/dlr-gtlab/gtlab-core/issues/1515>`_, `#1528 <https://github.com/dlr-gtlab/gtlab-core/issues/1528>`_, `#1529 <https://github.com/dlr-gtlab/gtlab-core/issues/1529>`_, `#1530 <https://github.com/dlr-gtlab/gtlab-core/issues/1530>`_, and `#1531 <https://github.com/dlr-gtlab/gtlab-core/issues/1531>`_

Context
-------

GTlab needs to run computational work in different execution locations without
duplicating domain logic or coupling modules to communication between processes
or machines.
The :doc:`../executable_operations` guide introduces the operation model. This
decision records the constraints that implementations must preserve.

Decision
--------

Operation model
~~~~~~~~~~~~~~~

``GtExecutableOperation`` is a normal ``GtObject`` with the lifecycle
``createData()`` → ``execute()`` → ``applyResult()``.

* ``createData()`` and ``applyResult()`` run on the originating side and must
  be fast.
* ``execute()`` is the synchronous, potentially expensive execution-side step.
* ``applyResult()`` receives the full outcome and interprets its domain meaning
  when updating the originating project.
* ``requiresProject()`` states whether the invocation needs an execution-local
  project; it does not select placement.

Operations, detached input, and detached result payloads use
``GtObjectFactory`` and Memento/XML. Modules register operation classes through
``GtOperationInterface``. There is no separate operation factory, registry, or
serializer.

Execution state
~~~~~~~~~~~~~~~

The originating operation, an execution-local reconstructed operation, the
detached input, and the optional detached result payload are separate objects
with separate lifetimes. ``GtOperationExecutionResult`` is a value envelope
containing the operation status, code, message, and optional result payload.
Detached input and result payloads must not contain borrowed pointers into the
originating project.

``GtOperationExecutionContext`` contains input data, the execution identity,
the cancellation state, and the event stream. It does not own project state. A
project at the execution location remains available through
``GtExecutionContext``.

Operation submission is asynchronous for the caller, while ``execute()`` stays
synchronous in the calling thread. ``GtOperationExecutor`` owns preparation,
async lifecycle, backend selection, cancellation requests, and transport of
operation outcomes. The executor also gates result application based on its
lifecycle and cancellation policy. The selected backend owns placement and
provisioning or reconstruction.

``GtExecutionEnvironment`` is the synchronous execution-side boundary. It
borrows an optional pre-provisioned project, establishes the invocation's
``GtExecutionContextScope``, constructs the ``GtOperationExecutionContext``, and
calls ``execute()``. It does not initialize Core or a session, perform project
I/O, create or move objects or threads, schedule work, transport data, apply
an operation outcome, acquire ``GtProjectExecutionGuard``, or roll back
project changes. It can be reused sequentially around the same borrowed project
and has no lifecycle state.

For a project-required invocation, the environment rejects a missing project
and checks the operation, data, and project thread affinity before execution.
For a project-independent invocation, it neither inspects nor uses its optional
project and installs an explicitly empty context. This makes legacy current-
project accessors return ``nullptr`` instead of falling back to a GUI/session
project.

Cancellation remains effective until the originating side starts
``applyResult()``. If cancellation is requested before then, the executor does
not call ``applyResult()``; it may still return or expose the detached result
payload to the client. Once ``applyResult()`` starts, cancellation no longer
interrupts that commit step. The executor owns this lifecycle/cancellation
gate, but it does not interpret the outcome. When called, ``applyResult()``
receives the complete outcome and the operation interprets its statuses and
result payload.

Operation outcome and environment errors
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

An operation outcome is the complete ``GtOperationExecutionResult``; a result
payload is its optional ``GtObject``. ``GtOperationExecutionResult`` is a
transport-neutral Core value type, not a ``GtObject``. It contains
``Success``, ``Failed``, or ``Cancelled``, an optional operation-defined
``code`` and ``message``, and an optional result payload. Any status may contain
a payload; there is no generic ``PartialResult`` status. Generic execution code
does not classify payload completeness or domain applicability.

``GtExecutionResult`` keeps boundary failures separate from operation
outcomes. ``Error::None`` means an operation outcome is present, including a
regular domain failure with ``Status::Failed``. ``ProjectRequired``,
``WrongThread``, and ``UnhandledException`` mean there is no operation outcome.
An exception escaping ``execute()`` becomes ``UnhandledException``.

Transport adapters encode ``status``, ``code``, and ``message`` as scalar
protocol metadata. They serialize and reconstruct only the optional
polymorphic ``GtObject`` payload through Memento and ``GtObjectFactory``. The
complete ``GtOperationExecutionResult`` is not serialized wholesale, and
``GtExecutionEnvironment`` performs no serialization.

Cancellation is cooperative after execution begins. A pre-cancelled token
skips ``execute()`` and returns a regular ``Cancelled`` outcome. During
execution, the operation decides how to react, and the environment preserves
the status it returns. The environment performs no rollback. A higher-level
resident worker decides whether a project changed by a failed, cancelled, or
throwing invocation remains trusted and whether it must be reprovisioned.

Events
~~~~~~

An operation can report events while ``execute()`` is running. For example, it
can report that it started, provide progress, or report that one step finished.
The application that started the operation can show this information before
the final operation outcome is available.

Events do not change the originating project and do not replace the operation
outcome.

During ``execute()``, the operation publishes events through
``GtOperationExecutionContext::events()``. The caller supplies the
``GtExecutionEventStream`` to ``GtExecutionEnvironment``; the environment uses
it for the invocation but does not store or transport events. The stream adds
the execution identity and sequence number. Local observers receive the events
through Qt signals and slots. If the operation runs in another process or on
another machine, a backend or transport adapter can store or forward the same
events. The operation code does not depend on that adapter.

Each event contains:

* ``executionId`` identifies one execution of the operation;
* ``sequence`` starts at zero and increases for each event;
* ``eventType`` is a short name for what happened; and
* ``payload`` contains optional JSON data for the event.

Use events for small status and progress messages. Send larger GTlab data as
the result payload or through a separate data channel.

Event file
^^^^^^^^^^

The file writer stores events in ``events.ndjson``. The file uses NDJSON: each
line contains one complete JSON object. For example:

.. code-block:: json

   {"eventType":"started","executionId":"d68ad56f-6b55-4b67-b170-ccb9a42a7548","payload":null,"sequence":0}
   {"eventType":"progress","executionId":"d68ad56f-6b55-4b67-b170-ccb9a42a7548","payload":{"ratio":0.5},"sequence":1}

Both lines belong to the same execution. The sequence numbers define their
order. The file contains only events. GTlab writes normal log output to a
separate file.

If several threads publish events at the same time, the stream puts them into
one order. All observers receive the events in that order. An observer can also
publish another event while it handles an event. The stream does not call
observer code while its internal lock is held.

An optional encoder can write event records to standard output for
compatibility. The file writer remains the primary event channel.

Task and worker integration
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Existing ``GtTask`` and calculator code remains unchanged. A later
``ProcessTaskOperation`` adapter (#1531) connects it to the operation model and
may reuse ``GtCoreProcessExecutor`` internally. Generic operation and
execution-environment code does not depend on task lookup or task-specific
state.

A worker backend provisions or reconstructs GTlab objects through the normal
factories, configures boundary adapters, and calls ``GtExecutionEnvironment``.
Worker startup and project provisioning remain outside the environment.

Consequences
------------

* Module code can use one operation implementation for local and remote
  execution.
* Operations continue to use the existing GTlab object registration and
  serialization mechanisms.
* The executor can change lifecycle and cancellation gating while operations
  keep ownership of domain-specific outcome interpretation.
* Backend placement, transport, and GUI integrations can evolve without adding
  lifecycle responsibilities to the synchronous environment.
* Broker, queue, cluster, replay, reconnect, resident-session synchronization,
  and GUI interaction are separate work.
