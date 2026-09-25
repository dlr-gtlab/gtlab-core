Architecture decision 0001 — Executable operations
===================================================

:Status: Accepted
:Date: 2026-09-03
:Normative source: GitHub issue `#1515 <https://github.com/dlr-gtlab/gtlab-core/issues/1515>`_
                   (within parent epic
                   `#1526 <https://github.com/dlr-gtlab/gtlab-core/issues/1526>`_)
:Related: `#1528 <https://github.com/dlr-gtlab/gtlab-core/issues/1528>`_,
          `#1529 <https://github.com/dlr-gtlab/gtlab-core/issues/1529>`_,
          `#1530 <https://github.com/dlr-gtlab/gtlab-core/issues/1530>`_, and
          `#1531 <https://github.com/dlr-gtlab/gtlab-core/issues/1531>`_

Context
-------

GTlab needs to run computational work in different locations without coupling
operation code to threads, worker processes, or remote communication. The
:doc:`../executable_operations` guide introduces the operation model. This
decision fixes the boundary between asynchronous orchestration and synchronous
operation execution.

Decision
--------

Operation contract
~~~~~~~~~~~~~~~~~~

``GtExecutableOperation`` is a normal ``GtObject`` with the lifecycle
``createData()`` → ``execute()`` → ``applyResult()``.

* ``createData()`` prepares optional detached input on the originating side.
* ``execute()`` performs synchronous computation at the execution location.
* ``applyResult()`` receives the full operation outcome on the originating
  side and interprets its domain meaning.
* ``requiresProject()`` states whether one invocation needs an execution-local
  project. It does not affect placement.

Detached input and result objects must not contain borrowed pointers into the
originating project. Operations use ``GtObjectFactory`` and Memento/XML through
the existing GTlab object mechanisms; there is no operation-specific factory or
serializer.

Execution architecture
~~~~~~~~~~~~~~~~~~~~~~

The responsibilities are separated as follows:

.. code-block:: text

   Client (GUI / Python / HTTP)
           |
           v
   GtOperationExecutor
       createData(), async lifecycle/handle, backend selection,
       cancellation request, result transport, applyResult()
           |
           v
   Execution backend
       placement and provisioning/reconstruction
           |
           v
   GtExecutionEnvironment
       borrowed optional project, GtExecutionContextScope,
       invocation-local GtOperationExecutionContext, synchronous execute()
           |
           v
   GtExecutableOperation

``GtExecutionEnvironment`` is not a project runtime, session, scheduler, worker,
or Core bootstrap component. It can be reused sequentially around the same
borrowed project. It has no lifecycle state. It does not initialize
``GtCoreApplication``, ``GtCoreDatamodel``, or ``GtSession``; perform project
I/O; create or move objects; create threads, queues, processes, or async handles;
select a backend; serialize data; apply results; or manage rollback and
recovery. It does not acquire ``GtProjectExecutionGuard``.

Project scope
~~~~~~~~~~~~~

For a project-required invocation, the environment rejects a missing project,
checks that the operation, data, and project have affinity to the calling
thread, and installs the borrowed project in ``GtExecutionContextScope`` for
``execute()``. The project remains owned and provisioned by the backend.

For a project-independent invocation, the environment does not inspect or use
its optional project. It installs an explicitly empty ``GtExecutionContext``
so that ``gtApp->currentProject()`` and ``gtDataModel->currentProject()`` return
``nullptr`` instead of falling back to a session or GUI project. This also
shadows an outer execution context and makes the invocation placement-neutral.

The environment always runs in the calling thread and never marshals work. The
backend must provision the operation, optional data, event stream, and any
required project so their use is safe in that thread. The event stream supports
thread-safe publication. A project context is thread-local and is not copied to
child threads. Task infrastructure that needs it in a child thread must copy
the ``GtExecutionContext`` and install a scope there. The environment does not
make shared project access thread-safe.

Operation outcome and boundary errors
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``GtOperationExecutionResult`` is a transport-neutral Core value containing:

* a ``Success``, ``Failed``, or ``Cancelled`` status;
* an optional operation-defined code and human-readable message; and
* an optional ``GtObject`` result payload.

Every status may carry a payload. No generic ``PartialResult`` state is added,
and generic execution code does not classify payload completeness or decide
whether it is suitable for application.

``GtExecutionResult`` separates this outcome from environment failures. It
contains exactly one of an operation outcome or a boundary error:

* ``Error::None`` means an operation outcome is present, including a regular
  domain failure with ``Status::Failed``.
* ``ProjectRequired``, ``WrongThread``, and ``UnhandledException`` mean no
  operation outcome is present.

An exception escaping ``execute()`` is caught at the environment boundary and
reported as ``UnhandledException``. Other boundary failures remain distinct
from the operation's status, code, message, and payload.

Events
~~~~~~

An operation publishes progress and status through the supplied
``GtExecutionEventStream``. Each event carries the execution identity, a
zero-based sequence number, an event type, and an optional JSON payload. The
stream orders concurrent publications and notifies local observers in that
order. Events are separate from the final operation outcome.

The environment uses the caller-supplied stream but does not store, forward, or
transport events. A backend or transport adapter may connect the same stream to
local observers or an external event channel.

Event files use NDJSON: each line contains one JSON event with its execution
identity, sequence number, type, and payload. The event file contains no normal
log output. The optional standard-output encoder is a compatibility adapter;
the file writer remains the primary event channel. Event-file writing and
forwarding stay outside the environment.

Cancellation and project state
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Cancellation is cooperative after ``execute()`` starts. A pre-cancelled token
prevents the call and yields an operation outcome with ``Status::Cancelled``.
During execution, the operation sees the shared token and chooses when to
react. The environment never rewrites the outcome after execution: a returned
``Success``, ``Failed``, or ``Cancelled`` status is preserved. Hard interruption
is a process/backend responsibility. Bridging cancellation into task
interruption belongs to the later ``ProcessTaskOperation`` adapter (#1531).

The environment performs no rollback. An operation may already have changed
its execution-local project when it returns ``Failed`` or ``Cancelled`` or
throws. A one-shot worker can discard that project. A resident project worker
must decide at a higher level whether the warm project is still trusted and
whether it must be reprovisioned from a known-good state. Checkpoints and
resident-project recovery are outside this decision's environment boundary.

Transport and application
~~~~~~~~~~~~~~~~~~~~~~~~~

Adapters encode ``status``, ``code``, and ``message`` directly for local,
process, or remote transport. An optional polymorphic ``GtObject`` payload uses
the existing Memento/``GtObjectFactory`` mechanism. The environment does no
serialization and transports neither events nor results.

The operation receives the reconstructed full ``GtOperationExecutionResult``
in ``applyResult()``. Transport and application are separate decisions. The
originating operation owns interpretation of every status and payload; the
generic executor decides whether and when to call ``applyResult()`` as part of
its client-side policy.

Task integration
~~~~~~~~~~~~~~~~

The environment has no dependency on ``GtTask``, calculators, or
``GtCoreProcessExecutor``. Existing tasks enter through a later
``ProcessTaskOperation`` adapter (#1531), which owns task-specific cancellation
bridging and result construction.

Consequences
------------

* One operation contract works across in-process, one-shot, resident, and
  future remote placement.
* Execution code receives deterministic project visibility from an explicit
  scope, including no project for project-independent operations.
* Domain outcomes, environment failures, result transport, and result
  application remain distinct.
* Scheduling, worker lifecycle, transport protocols, and resident-project
  recovery can change without expanding the synchronous environment.
