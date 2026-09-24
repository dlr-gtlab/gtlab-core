Architecture decision 0001 — Executable operations
===================================================

:Status: Accepted
:Date: 2026-09-03
:Normative source: GitHub issues `#1526 <https://github.com/dlr-gtlab/gtlab-core/issues/1526>`_ (operation model) and `#1515 <https://github.com/dlr-gtlab/gtlab-core/issues/1515>`_ (runtime)
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
* Only ``applyResult()`` updates the originating project with the result.
* ``requiresProject()`` states whether the execution location needs a project.

Operations, detached input, and detached results use ``GtObjectFactory`` and
Memento/XML. Modules register operation classes through
``GtOperationInterface``. There is no separate operation factory, registry, or
serializer.

Execution state
~~~~~~~~~~~~~~~

The originating operation, an execution-local reconstructed operation, and the
detached input/result are separate objects with separate lifetimes. Detached
objects must not contain borrowed pointers into the originating project.

``GtOperationExecutionContext`` contains input data, the execution identity,
the cancellation state, and the event stream. It does not own project state. A
project at the execution location remains available through
``GtExecutionContext``.

``GtProjectRuntime`` executes the operation synchronously where the host places it.
It opens, saves, and closes one project and calls ``execute()`` directly.
It constructs the operation context, installs the project context and guard
when required, catches execution failures, and returns a detached result.
It does not schedule, marshal, or spawn work. The host must provide compatible
execution-local Core services and Qt thread affinity.

The future ``GtOperationExecutor`` owns client-side preparation, scheduling,
status, cancellation requests, result transport, ``applyResult()``, and
client-visible completion. Its backend chooses a local thread, local process,
or remote worker. The backend provides the event stream and cancellation token
before entering ``GtProjectRuntime::executeOperation()`` so observers can
receive events and request cancellation while that call is blocked. Hard
interruption of uncooperative work belongs to the backend.

.. code-block:: text

   GUI or HTTP client
          |
          v
   GtOperationExecutor (future: scheduling, status, result application)
          |
          v
   backend (future: local thread / local process / remote worker)
          |
          v
   GtProjectRuntime (one project, synchronous execute)

For local-thread placement, the originating project stays with the caller;
the backend provisions a distinct execution-local project and runtime:

.. code-block:: text

   origin thread                     execution thread
   -------------                     ----------------
   project -> createData()           GtProjectRuntime -> execute()
         ^          |                         |             |
         |          +---- detached input ----+             |
         +-- applyResult() <-- detached result ------------+


Runtime completion means ``execute()`` returned with a detached result or
failure. Client completion occurs later, after result transport and
``applyResult()`` on the originating side. These are distinct events.

Events
~~~~~~

An operation can report events while ``execute()`` is running. For example, it
can report that it started, provide progress, or report that one step finished.
The application that started the operation can show this information before
the final result is available.

Events do not change the originating project and do not replace the operation
result.

During ``execute()``, the operation publishes events through
``GtOperationExecutionContext::events()``. The returned
``GtExecutionEventStream`` adds the execution identity and sequence number.
Local observers receive the events through Qt signals and slots. If the
operation runs in another process or on another machine, an adapter can store
or forward the same events. The operation code does not depend on that adapter.

Each event contains:

* ``executionId`` identifies one execution of the operation;
* ``sequence`` starts at zero and increases for each event;
* ``eventType`` is a short name for what happened; and
* ``payload`` contains optional JSON data for the event.

Use events for small status and progress messages. Send larger GTlab data as
the operation result or through a separate data channel.

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

Existing ``GtTask`` and calculator code remains unchanged. A
``ProcessTaskOperation`` adapter connects it to the operation/runtime model and
may reuse ``GtCoreProcessExecutor`` internally. Generic operation code does not
depend on task lookup or task-specific state.

A one-shot worker is an adapter around ``GtProjectRuntime``. It reconstructs
GTlab objects through the normal factories, configures boundary adapters, and
calls ``executeOperation()`` directly. It does not introduce another execution
lifecycle. GUI and HTTP entry points are equivalent clients of the future
executor; neither changes the runtime contract.

Consequences
------------

* Module code can use one operation implementation for local and remote
  execution.
* Operations continue to use the existing GTlab object registration and
  serialization mechanisms.
* Runtime, transport, and GUI integrations can evolve without changing domain
  operations.
* Broker, queue, cluster, replay, reconnect, resident-session synchronization,
  and GUI interaction are separate work.
